[![Website](https://raw.githubusercontent.com/HyperDbg/graphics/master/Badges/Link-Website-orange.svg)](https://hyperdbg.com)
[![Docs](https://raw.githubusercontent.com/HyperDbg/graphics/master/Badges/Link-Docs-yellow.svg)](https://docs.hyperdbg.com)
[![Doxygen](https://raw.githubusercontent.com/HyperDbg/graphics/master/Badges/Link-Doxygen-lightgreen.svg)](https://doxygen.hyperdbg.com)
[![License: GPL v3](https://raw.githubusercontent.com/HyperDbg/graphics/master/Badges/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

<a href="https://hyperdbg.com/"><img align="right" width="150" height="150" src="https://raw.githubusercontent.com/HyperDbg/graphics/master/Icons/hyperdbg-icon.png" alt="HyperDbg Debugger"></a></br>

# HyperDbg Debugger 
#### (is not yet released/finished)

## Description


HyperDbg is an open-source, user mode and kernel mode Windows debugger with a focus on using hardware technologies to provide new features to the debuggers’ world.

It is designed on top of Windows by virtualizing an already running system using Intel VT-x and Intel PT. This debugger aims not to use any APIs and software debugging mechanisms, but instead, it uses Second Layer Page Table (a.k.a. Extended Page Table or EPT) extensively to monitor both kernel and user executions.
<p align="center"><a href="https://hyperdbg.com/"><img align="center" width="600" height="500" src="https://raw.githubusercontent.com/HyperDbg/graphics/master/Art%20Board/Artboard%201.png" alt="HyperDbg Debugger"></a></br>
</p>
HyperDbg comes with features like hidden hooks, which is as fast as old inline hooks, but also stealth. It mimics hardware debug registers for (read & write) to a specific location, but this time entirely invisible for both Windows kernel and the programs, and of course without any limitation in size or count!

Using TLB-splitting, and having features such as measuring code coverage and monitoring all mov(s) to/from memory by a function, makes HyperDbg a unique debugger.

Although it has novel features, HyperDbg tries to be as stealth as possible. It doesn’t use any debugging APIs to debug Windows or any application, so classic anti-debugging methods won’t detect it. Also, it resists the exploitation of time delta methods (e.g., RDTSC/RDTSCP) to detect the presence of hypervisors, therefore making it much harder for applications, packers, protectors, malware, anti-cheat engines, etc. to discover the debugger.

## Installation

## How does it work?

We explained about how HyperDbg internally works and how we designed its features in details, take a look at : </br>
<p align="center">(https://docs.hyperdbg.com/how-does-it-work)</p>

Here's a diagram that shows how HyperDbg works !
</br>

<p align="center"><a href="https://hyperdbg.com/"><img align="center" width="70%" height="100%" src="https://raw.githubusercontent.com/HyperDbg/graphics/master/Diagrams/Diagram_v1.jpg" alt="HyperDbg Design"></a></br>
</p>
</br>


## Plugins



## Credits
 Developers :<br />
- <a href="https://twitter.com/Intel80x86">Sina Karvandi</a><br />
- <a href="https://twitter.com/mammadataei">Mohammad Ataei</a> (for making the website)


 Special Thanks to these guys for their helps and contributions :<br />
 - <a href="https://twitter.com/PetrBenes">Petr Beneš</a> (for his great helps in designing hypervisor)

- ...and many other people who helped to solve the problems</a><br />

## Contributing
Contributing in HyperDbg is super appreciated.

If you want to create a pull request or contribute in HyperDbg please read [Contribution Guide](https://github.com/SinaKarvandi/HyperDbg/blob/master/CONTRIBUTING.md).


## License
HyperDbg is under GPLv3 LICENSE.
