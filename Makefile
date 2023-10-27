# NOTE we use  -fvect-cost-model=very-cheap to disable g++ from optimizing away our infinite loops :D
# NOTE clang++ breaks with PHDRs overlapping!
CC:=g++
NASM:=nasm
QEMU=qemu-system-x86_64
# NOTE: Original one is qemu-system-x86_64
LINKER:=ld
NASM_SOURCE_FILES:=$(wildcard src/BukoOS/libs/*.nasm)
SOURCE_FILES:=$(wildcard src/BukoOS/**.c src/BukoOS/**.cpp) $(wildcard src/BukoOS/libs/*.cpp)
CC_FLAGS:=-nostdlib -march=x86-64 -ffreestanding -static -Wall -Wno-reorder -fomit-frame-pointer -fno-builtin -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow 
OBJDIR:=out/int
OBJDEST:=out/int/kernel
INCLUDES:=vendor/limine
LIMINECFG:=src/limine.cfg
LINUX_ENV:= 
# WINDOWS_LINUX_DISTRO := Ubuntu # Note: This does not take into effect if the platform is not windows!
ifeq ($(OS), Windows_NT) 
	include install/windows/WSLPlat.make
	LINUX_ENV:=wsl -d $(WINDOWS_LINUX_DISTRO)
endif
LINKERSCRIPT:=linker/link.ld
OUTFILE:=out/bin/kernel.iso

all: init
# NOTE: Recommended for faster compilation time
test:
	$(foreach f, $(NASM_SOURCE_FILES), @echo $(LINUX_ENV) $(NASM) $(f) -o $(basename $(notdir $(f))).no;)
eenv:
	$(LINUX_ENV)
#  -fversion-loops-for-strides -ftree-partial-pre -ftree-loop-distribution -fsplit-paths -fsplit-loops -fpredictive-commoning -fpeel-loops -floop-unroll-and-jam -floop-interchange -fipa-cp-clone -funswitch-loops 
compile_debug:
	$(foreach f, $(NASM_SOURCE_FILES), $(LINUX_ENV) $(NASM) -f elf64 $(f) -o $(OBJDIR)/$(basename $(notdir $(f))).no)
	$(LINUX_ENV) $(CC) -g $(CC_FLAGS) $(SOURCE_FILES) $(wildcard $(OBJDIR)/*.no) -o $(OBJDEST) -D BUKO_DEBUG -T $(LINKERSCRIPT) -I $(INCLUDES)
compile_release:
	$(foreach f, $(NASM_SOURCE_FILES), $(LINUX_ENV) $(NASM) -f elf64 $(f) -o $(OBJDIR)/$(basename $(notdir $(f))).no)
	$(LINUX_ENV) $(CC) -g -O3 $(CC_FLAGS) $(SOURCE_FILES) $(wildcard $(OBJDIR)/*.no) -o $(OBJDEST) -D BUKO_RELEASE -T $(LINKERSCRIPT) -I $(INCLUDES) # -fvect-cost-model=very-cheap                
compile_dist: 
	$(foreach f, $(NASM_SOURCE_FILES), $(LINUX_ENV) $(NASM) -f elf64 $(f) -o $(OBJDIR)/$(basename $(notdir $(f))).no)
	$(LINUX_ENV) $(CC) -O5 $(CC_FLAGS) $(SOURCE_FILES) $(wildcard $(OBJDIR)/*.no) -o $(OBJDEST) -D BUKO_DIST -T $(LINKERSCRIPT) -I $(INCLUDES) -fvect-cost-model=very-cheap
build_iso:
	rm -f out/bin/OS.iso
	$(LINUX_ENV) cp vendor/limine/limine-bios.sys vendor/limine/limine-bios-cd.bin $(LIMINECFG) vendor/limine/limine-uefi-cd.bin $(OBJDEST) out/bin/
	$(LINUX_ENV) xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image $(OBJDEST) out/bin -o out/bin/OS.iso
run_qemu:
	$(QEMU) -cpu max -smp 2 -m 128 -cdrom out/bin/OS.iso
debugger_qemu_testing:
	$(QEMU) -S -s -cpu max -smp 2 -m 128 -cdrom out/bin/OS.iso
	#$(QEMU) -kernel out/bin/OS.iso -cpu max -smp 2 -m 128 -cdrom out/bin/OS.iso -S -s 
debugger_bochs_testing:
	bochsdbg -f wintest.bxrc -q 
egdb:
	gdb -tui -ex "target remote :1234" -ex "symbol-file out/int/kernel"
# debugger_qemu_testing:
#	qemu-system-x86_64.exe -cpu max -smp 2 -m 128 -cdrom out/bin/OS.iso -S -s
install_limine:
	vendor/limine/limine bios-install out/bin/OS.iso
clean:
	$(LINUX_ENV) find out -type f -delete
backup:
	$(LINUX_ENV) zip -r backups/backup$(shell date --iso=seconds) . -x out/**\* backups/**\* vendor/**\* .git/**\*
debug:   compile_debug build_iso
release: compile_release build_iso
dist:    compile_dist build_iso 
init:
	$(LINUX_ENV) mkdir -p out
	$(LINUX_ENV) mkdir -p out/bin
	$(LINUX_ENV) mkdir -p out/int
	$(LINUX_ENV) mkdir -p backups


