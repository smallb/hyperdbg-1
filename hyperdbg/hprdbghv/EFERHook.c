/**
 * @file EFERHook.c
 * @author Sina Karvandi (sina@rayanfam.com)
 * @brief Implenetation of the fucntions related to the EFER Syscall Hook
 * @details This is derived by the method demonstrated at
 * - https://revers.engineering/syscall-hooking-via-extended-feature-enable-register-efer/
 * 
 * also some of the functions derived from hvpp
 * - https://github.com/wbenny/hvpp
 * 
 * @version 0.1
 * @date 2020-04-10
 * 
 * @copyright This project is released under the GNU Public License v3.
 * 
 */
#include <ntddk.h>
#include <Windef.h>
#include "Common.h"
#include "Msr.h"
#include "Hooks.h"
#include "Invept.h"
#include "Events.h"
#include "HypervisorRoutines.h"
#include "GlobalVariables.h"
#include "MemoryMapper.h"
#include "Vmx.h"
#include "Logging.h"

/**
 * @brief This function enables or disables EFER syscall hoo
 * @details This function should be called for the first time
 * that we want to enable EFER hook because after calling this 
 * function EFER MSR is loaded from GUEST_EFER instead of loading
 * from the regular EFER MSR.
 * 
 * @param EnableEFERSyscallHook Determines whether we want to enable syscall hook or disable syscall hook
 * @return VOID 
 */
VOID
SyscallHookConfigureEFER(BOOLEAN EnableEFERSyscallHook)
{
    EFER_MSR           MsrValue;
    IA32_VMX_BASIC_MSR VmxBasicMsr     = {0};
    UINT32             VmEntryControls = 0;
    UINT32             VmExitControls  = 0;

    //
    // Reading IA32_VMX_BASIC_MSR
    //
    VmxBasicMsr.All = __readmsr(MSR_IA32_VMX_BASIC);

    //
    // Read previous VM-Entry and VM-Exit controls
    //
    __vmx_vmread(VM_ENTRY_CONTROLS, &VmEntryControls);
    __vmx_vmread(VM_EXIT_CONTROLS, &VmExitControls);

    MsrValue.Flags = __readmsr(MSR_EFER);

    if (EnableEFERSyscallHook)
    {
        MsrValue.SyscallEnable = FALSE;

        //
        // Set VM-Entry controls to load EFER
        //
        __vmx_vmwrite(VM_ENTRY_CONTROLS, HvAdjustControls(VmEntryControls | VM_ENTRY_LOAD_IA32_EFER, VmxBasicMsr.Fields.VmxCapabilityHint ? MSR_IA32_VMX_TRUE_ENTRY_CTLS : MSR_IA32_VMX_ENTRY_CTLS));

        //
        // Set VM-Exit controls to save EFER
        //
        __vmx_vmwrite(VM_EXIT_CONTROLS, HvAdjustControls(VmExitControls | VM_EXIT_SAVE_IA32_EFER, VmxBasicMsr.Fields.VmxCapabilityHint ? MSR_IA32_VMX_TRUE_EXIT_CTLS : MSR_IA32_VMX_EXIT_CTLS));

        //
        // Set the GUEST EFER to use this value as the EFER
        //
        __vmx_vmwrite(GUEST_EFER, MsrValue.Flags);
    }
    else
    {
        MsrValue.SyscallEnable = TRUE;

        //
        // Set VM-Entry controls to load EFER
        //
        __vmx_vmwrite(VM_ENTRY_CONTROLS, HvAdjustControls(VmEntryControls & ~VM_ENTRY_LOAD_IA32_EFER, VmxBasicMsr.Fields.VmxCapabilityHint ? MSR_IA32_VMX_TRUE_ENTRY_CTLS : MSR_IA32_VMX_ENTRY_CTLS));

        //
        // Set VM-Exit controls to save EFER
        //
        __vmx_vmwrite(VM_EXIT_CONTROLS, HvAdjustControls(VmExitControls & ~VM_EXIT_SAVE_IA32_EFER, VmxBasicMsr.Fields.VmxCapabilityHint ? MSR_IA32_VMX_TRUE_EXIT_CTLS : MSR_IA32_VMX_EXIT_CTLS));

        //
        // Set the GUEST EFER to use this value as the EFER
        //
        __vmx_vmwrite(GUEST_EFER, MsrValue.Flags);
    }
}

/**
 * @brief Set the Guest Cs selector
 * 
 * @param Cs The CS Selector for the guest
 * @return VOID 
 */
VOID
SetGuestCs(PSEGMENT_SELECTOR Cs)
{
    __vmx_vmwrite(GUEST_CS_BASE, Cs->BASE);
    __vmx_vmwrite(GUEST_CS_LIMIT, Cs->LIMIT);
    __vmx_vmwrite(GUEST_CS_AR_BYTES, Cs->ATTRIBUTES.UCHARs);
    __vmx_vmwrite(GUEST_CS_SELECTOR, Cs->SEL);
}

/**
 * @brief Set the Guest Ss selector
 * 
 * @param Ss The SS Selector for the guest
 * @return VOID 
 */
VOID
SetGuestSs(PSEGMENT_SELECTOR Ss)
{
    __vmx_vmwrite(GUEST_SS_BASE, Ss->BASE);
    __vmx_vmwrite(GUEST_SS_LIMIT, Ss->LIMIT);
    __vmx_vmwrite(GUEST_SS_AR_BYTES, Ss->ATTRIBUTES.UCHARs);
    __vmx_vmwrite(GUEST_SS_SELECTOR, Ss->SEL);
}

/**
 * @brief This function emulates the SYSCALL execution 
 * 
 * @param Regs Guest registers
 * @return BOOLEAN
 */
BOOLEAN
SyscallHookEmulateSYSCALL(PGUEST_REGS Regs)
{
    SEGMENT_SELECTOR Cs, Ss;
    UINT32           InstructionLength;
    UINT64           MsrValue;
    ULONG64          GuestRip;
    ULONG64          GuestRflags;

    //
    // Reading guest's RIP
    //
    __vmx_vmread(GUEST_RIP, &GuestRip);

    //
    // Reading instruction length
    //
    __vmx_vmread(VM_EXIT_INSTRUCTION_LEN, &InstructionLength);

    //
    // Reading guest's Rflags
    //
    __vmx_vmread(GUEST_RFLAGS, &GuestRflags);

    //
    // Save the address of the instruction following SYSCALL into RCX and then
    // load RIP from MSR_LSTAR.
    //
    MsrValue  = __readmsr(MSR_LSTAR);
    Regs->rcx = GuestRip + InstructionLength;
    GuestRip  = MsrValue;
    __vmx_vmwrite(GUEST_RIP, GuestRip);

    //
    // Save RFLAGS into R11 and then mask RFLAGS using MSR_FMASK
    //
    MsrValue  = __readmsr(MSR_FMASK);
    Regs->r11 = GuestRflags;
    GuestRflags &= ~(MsrValue | X86_FLAGS_RF);
    __vmx_vmwrite(GUEST_RFLAGS, GuestRflags);

    //
    // Load the CS and SS selectors with values derived from bits 47:32 of MSR_STAR
    //
    MsrValue             = __readmsr(MSR_STAR);
    Cs.SEL               = (UINT16)((MsrValue >> 32) & ~3); // STAR[47:32] & ~RPL3
    Cs.BASE              = 0;                               // flat segment
    Cs.LIMIT             = (UINT32)~0;                      // 4GB limit
    Cs.ATTRIBUTES.UCHARs = 0xA09B;                          // L+DB+P+S+DPL0+Code
    SetGuestCs(&Cs);

    Ss.SEL               = (UINT16)(((MsrValue >> 32) & ~3) + 8); // STAR[47:32] + 8
    Ss.BASE              = 0;                                     // flat segment
    Ss.LIMIT             = (UINT32)~0;                            // 4GB limit
    Ss.ATTRIBUTES.UCHARs = 0xC093;                                // G+DB+P+S+DPL0+Data
    SetGuestSs(&Ss);

    return TRUE;
}

/**
 * @brief This function emulates the SYSRET execution 
 * 
 * @param Regs Guest registers
 * @return BOOLEAN
 */
BOOLEAN
SyscallHookEmulateSYSRET(PGUEST_REGS Regs)
{
    SEGMENT_SELECTOR Cs, Ss;
    UINT64           MsrValue;
    ULONG64          GuestRip;
    ULONG64          GuestRflags;

    //
    // Load RIP from RCX
    //
    GuestRip = Regs->rcx;
    __vmx_vmwrite(GUEST_RIP, GuestRip);

    //
    // Load RFLAGS from R11. Clear RF, VM, reserved bits
    //
    GuestRflags = (Regs->r11 & ~(X86_FLAGS_RF | X86_FLAGS_VM | X86_FLAGS_RESERVED_BITS)) | X86_FLAGS_FIXED;
    __vmx_vmwrite(GUEST_RFLAGS, GuestRflags);

    //
    // SYSRET loads the CS and SS selectors with values derived from bits 63:48 of MSR_STAR
    //
    MsrValue             = __readmsr(MSR_STAR);
    Cs.SEL               = (UINT16)(((MsrValue >> 48) + 16) | 3); // (STAR[63:48]+16) | 3 (* RPL forced to 3 *)
    Cs.BASE              = 0;                                     // Flat segment
    Cs.LIMIT             = (UINT32)~0;                            // 4GB limit
    Cs.ATTRIBUTES.UCHARs = 0xA0FB;                                // L+DB+P+S+DPL3+Code
    SetGuestCs(&Cs);

    Ss.SEL               = (UINT16)(((MsrValue >> 48) + 8) | 3); // (STAR[63:48]+8) | 3 (* RPL forced to 3 *)
    Ss.BASE              = 0;                                    // Flat segment
    Ss.LIMIT             = (UINT32)~0;                           // 4GB limit
    Ss.ATTRIBUTES.UCHARs = 0xC0F3;                               // G+DB+P+S+DPL3+Data
    SetGuestSs(&Ss);

    return TRUE;
}

/**
 * @brief Detect whether the #UD was because of Syscall or Sysret or not
 * 
 * @param Regs Guest register
 * @param CoreIndex Logical core index
 * @return BOOLEAN Shows whther the caller should inject #UD on the guest or not
 */
BOOLEAN
SyscallHookHandleUD(PGUEST_REGS Regs, UINT32 CoreIndex)
{
    UINT64  GuestCr3;
    UINT64  OriginalCr3;
    UINT64  Rip;
    BOOLEAN Result;

    //
    // Reading guest's RIP
    //
    __vmx_vmread(GUEST_RIP, &Rip);

    //
    // Due to KVA Shadowing, we need to switch to a different directory table base
    // if the PCID indicates this is a user mode directory table base.
    //

    NT_KPROCESS * CurrentProcess = (NT_KPROCESS *)(PsGetCurrentProcess());
    GuestCr3                     = CurrentProcess->DirectoryTableBase;

    if ((GuestCr3 & PCID_MASK) != PCID_NONE)
    {
        OriginalCr3 = __readcr3();

        __writecr3(GuestCr3);
        //
        // Read the memory
        //
        CHAR * InstructionBuffer[3] = {0};
        MemoryMapperReadMemorySafe(Rip, InstructionBuffer, 3);

        if (IS_SYSRET_INSTRUCTION(InstructionBuffer))
        {
            __writecr3(OriginalCr3);
            goto EmulateSYSRET;
        }
        if (IS_SYSCALL_INSTRUCTION(InstructionBuffer))
        {
            __writecr3(OriginalCr3);
            goto EmulateSYSCALL;
        }
        __writecr3(OriginalCr3);
        return FALSE;
    }
    else
    {
        if (IS_SYSRET_INSTRUCTION(Rip))
            goto EmulateSYSRET;
        if (IS_SYSCALL_INSTRUCTION(Rip))
            goto EmulateSYSCALL;
        return FALSE;
    }

    //----------------------------------------------------------------------------------------

    //
    // Emulate SYSRET instruction
    //
EmulateSYSRET:
    //
    // Test
    //

    //
    // LogInfo("SYSRET instruction => 0x%llX", Rip);
    //

    //
    // We should trigger the event of SYSRET here
    //
    DebuggerTriggerEvents(SYSCALL_HOOK_EFER_SYSRET, Regs, Rip);

    Result                               = SyscallHookEmulateSYSRET(Regs);
    g_GuestState[CoreIndex].IncrementRip = FALSE;
    return Result;

    //
    // Emulate SYSCALL instruction
    //
EmulateSYSCALL:
    //
    // Test
    //

    //
    // LogInfo("SYSCALL instruction => 0x%llX , Process Id : 0x%x", Rip, PsGetCurrentProcessId());
    //

    //
    // We should trigger the event of SYSCALL here
    //
    DebuggerTriggerEvents(SYSCALL_HOOK_EFER_SYSCALL, Regs, Rip);

    Result = SyscallHookEmulateSYSCALL(Regs);

    g_GuestState[CoreIndex].IncrementRip = FALSE;
    return Result;
}
