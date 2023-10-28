@echo off

echo "Installing Ubuntu..."
call wsl --install -d Ubuntu    :: Install Ubuntu wsl
echo "Installing gcc..."
call wsl -d Ubuntu sudo apt install gcc :: Install gcc (RECOMMENDED) - you can also install clang and replace CC with it in the makefile
echo "Installing xorriso for generating iso image..."
call wsl -d Ubuntu sudo apt install xorriso :: Install xorriso (REQUIRED)!
echo "Installing zip for backups OPTIONAL"
call wsl -d Ubuntu sudo apt install zip :: If you want to use the backup system
echo "Setting WSL linux distro to Ubuntu"
echo "WINDOWS_LINUX_DISTRO := Ubuntu" > WSLPlat.make
