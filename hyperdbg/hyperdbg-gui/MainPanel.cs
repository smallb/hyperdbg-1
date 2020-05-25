﻿using System;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;

namespace hyperdbg_gui
{

    public partial class MainPanel : Form
    {

        public MainPanel() { InitializeComponent(); }

        public void commandText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                string Command = commandSection1.commandText.Text.Replace("\n", "");
                string CommandWithoutSpace = Command.Replace(" ", "");

                // check if it's a .cls
                if (CommandWithoutSpace == ".cls" || CommandWithoutSpace == "cls" ||
                    CommandWithoutSpace == "clear")
                {
                    hyperdbg_gui.Details.GlobalVariables.CommandWindow.richTextBox1.Clear();
                    commandSection1.commandText.Text = string.Empty;
                    return;
                }
                hyperdbg_gui.Details.GlobalVariables.CommandWindow.richTextBox1
                    .AppendText("\n" + commandSection1.richTextBox2.Text.Remove(0, 1) +
                                Command + "\n");
                KernelmodeRequests.KernelRequests.HyperdbgCommandInterpreter(Command);
                commandSection1.commandText.Text = string.Empty;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Text = "hyperdbg debugger (" +
                        hyperdbg_gui.Details.HyperdbgInformation.HyperdbgVersion +
                        ") x86-64";
            ControlMoverOrResizer.Init(panel1);
            ControlMoverOrResizer.WorkType = ControlMoverOrResizer.MoveOrResize.Resize;

            commandSection1.commandText.KeyDown +=
                new System.Windows.Forms.KeyEventHandler(commandText_KeyDown);
        }

        private void richTextBox1_Enter(object sender, EventArgs e) { }

        private void richTextBox1_Leave(object sender, EventArgs e) { }

        private void commandWindowsToolStripMenuItem_Click(object sender,
                                                           EventArgs e)
        { }

        private void toolStripButton11_Click(object sender, EventArgs e)
        {
            bool IsDark = !hyperdbg_gui.Details.GlobalVariables.IsInDarkMode;
            hyperdbg_gui.Details.GlobalVariables.IsInDarkMode =
                !hyperdbg_gui.Details.GlobalVariables.IsInDarkMode;

            foreach (Control c in this.Controls)
            {
                UpdateColorControls(c, IsDark);
            }
        }
        public void UpdateColorControls(Control myControl, bool IsDark)
        {
            if (IsDark)
            {
                myControl.BackColor = Color.FromArgb(37, 37, 38);
                myControl.ForeColor = Color.White;
            }
            else
            {
                myControl.BackColor = Color.White;
                myControl.ForeColor = Color.Black;
            }
            foreach (Control subC in myControl.Controls)
            {
                UpdateColorControls(subC, IsDark);
            }
        }

        private void registersToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Create a new instance of the MDI child template form
            RegsWindow chForm = new RegsWindow();

            // Set parent form for the child window
            chForm.MdiParent = this;

            // Display the child window
            chForm.Show();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutWindow about = new AboutWindow();
            about.ShowDialog();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show(
                "Not yet supported, support will be available in the future versions");
        }

        private int ReceivedMessagesHandler(string Text)
        {
            hyperdbg_gui.Details.GlobalVariables.CommandWindow.richTextBox1.Invoke(
                new Action(() =>
                {
                    hyperdbg_gui.Details.GlobalVariables.CommandWindow.richTextBox1
                .AppendText(Text);
                }));
            return 0;
        }

        public void LoadDriver()
        {
            hyperdbg_gui.KernelAffairs.CtrlNativeCallbacks.SetCallback(
                ReceivedMessagesHandler);

            if (hyperdbg_gui.KernelmodeRequests.KernelRequests
                    .HyperdbgDriverInstaller() != 0)
            {
                MessageBox.Show("There was an error installing the driver", "Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (hyperdbg_gui.KernelmodeRequests.KernelRequests.HyperdbgLoader() != 0)
            {
                MessageBox.Show(
                    "Failed to load hyperdbg's hypervisor driver, see logs for more information",
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            hyperdbg_gui.Details.GlobalVariables.IsDriverLoaded = true;
            toolStripButton21.Image = hyperdbg_gui.Properties.Resources.GreenCircle;
        }
        public void UnloadDriver()
        {
            hyperdbg_gui.KernelmodeRequests.KernelRequests.HyperdbgUnloader();

            if (hyperdbg_gui.KernelmodeRequests.KernelRequests
                    .HyperdbgDriverUninstaller() != 0)
            {
                MessageBox.Show("There was an error unloading the driver", "Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            hyperdbg_gui.Details.GlobalVariables.IsDriverLoaded = false;
        }

        private void toolStripButton21_Click(object sender, EventArgs e)
        {
            if (!hyperdbg_gui.Details.GlobalVariables.IsDriverLoaded)
            {
                if (true)
                {
                    // Make it globally available
                    hyperdbg_gui.Details.GlobalVariables.CommandWindow =
                        new CommandWindow();

                    // Show command View
                    hyperdbg_gui.Details.GlobalVariables.CommandWindow.MdiParent = this;

                    hyperdbg_gui.Details.GlobalVariables.CommandWindow.Show();

                    Details.GlobalVariables.VmxInitThread = new Thread(LoadDriver);
                    Details.GlobalVariables.VmxInitThread.Start();
                }

            }
            else
            {
                UnloadDriver();
                toolStripButton21.Image = hyperdbg_gui.Properties.Resources.RedCircle;
            }
        }

        private void toolStripButton6_Click(object sender, EventArgs e)
        {
            AboutWindow about = new AboutWindow();
            about.ShowDialog();
        }

        private void test2ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AttachWindow attach = new AttachWindow();
            attach.ShowDialog();
        }

        private void recentSessionsToolStripMenuItem_Click(object sender,
                                                           EventArgs e)
        {
            RecentSessions r = new RecentSessions();
            r.ShowDialog();
        }

        private void toolStripButton16_Click(object sender, EventArgs e)
        {
            DisassmblerWindow disassmbler = new DisassmblerWindow();

            // Set parent form for the child window
            disassmbler.MdiParent = this;

            // Display the child window
            disassmbler.Show();
        }

        private void toolStripButton17_Click(object sender, EventArgs e)
        {
            RegsWindow regs = new RegsWindow();

            // Set parent form for the child window
            regs.MdiParent = this;

            // Display the child window
            regs.Show();
        }

        private void toolStripButton22_Click(object sender, EventArgs e)
        {
            // Make it globally available
            hyperdbg_gui.Details.GlobalVariables.CommandWindow = new CommandWindow();

            // Show command View
            hyperdbg_gui.Details.GlobalVariables.CommandWindow.MdiParent = this;

            hyperdbg_gui.Details.GlobalVariables.CommandWindow.Show();
        }

        private void memoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MemoryWindow memoryWindow = new MemoryWindow();
            memoryWindow.MdiParent = this;
            memoryWindow.Show();
        }
    }
}
