# Default make target.
all : os-image

run : all
	qemu-system-i386 -fda os-image

C_SOURCES=$(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c common/*.c)
HEADERS=$(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h common/*.h)

OBJ=${C_SOURCES:.c=.o cpu/interrupt.o cpu/gdt_load.o}

os-image : boot/boot_sect.bin kernel.bin
	cat $^ > os-image

# $ ^ is substituted with all of the target ’ s dependancy files
#kernel.bin : kernel/kernel_entry.o ${OBJ}
#ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.bin : kernel/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Tlink.lds $^ --oformat binary

%.bin : %.asm
	nasm $< -f bin -I '/home/yiheng/桌面/Master2/os/X86/boot/' -o $@

%.o : %.asm
	nasm $< -f elf -o $@

# $ < is the first dependancy and $@ is the target file
%.o : %.c ${HEADERS}
	gcc -m32 -fno-pie -fno-stack-protector -c $< -o $@

clean:
	rm -fr *.bin *.dis *.o os-image
	rm -fr kernel/*.o boot/*.bin drivers/*.o cpu/*.o libc/*.o common/*.o