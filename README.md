# BukoOS
BukoOS is a really basic operating system using Limine as its bootloader. 
All source code falls under the MIT license. 

Honorable mention to:
- [CorruptedByCPU](https://github.com/CorruptedByCPU) who helped me understand his setup, and inspired me to make the OS (Check his github out, he does really awesome OS things)
# Requirements:
- make                               *Note you can also install it in the Linux environment and use it that way, but you will have to enter the environment every time*
**Linux environment requirements**:
- xorriso
- g++
- mkdir + rm
- (OPTIONAL) zip - for making backups
## Windows
- WSL

# Quickstart
## Windows
**Quickinstall**:
```cmd
> cd install/windows
> basicInstall.bat                    # Installs Ubuntu WSL and all of the things we need
```
**Build**:
```cmd
> wsl -d Ubuntu                         # Optionally if you want to build faster
> make init
> make dist                             # Compiles for dist and makes OS.iso, You could also compile for release and debug - debug with no optimisation and debug symbols, release with some optimisation and debug symbols
> make run_qemu                         # Optionally run OS directly in qemu
```
## Linux
```sh
> sudo apt install xorriso && sudo apt install g++  # apt install or use your local package manager if you're not on Ubuntu
> make init
> make dist
> make run_qemu                         # Optionally run the OS directly in qemu
```
## MacOS
- Haven't tested it yet, but should hopefully work :|

