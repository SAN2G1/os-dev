# --- tools / flags (기존 내용 유지) ---
CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

# --- sources 자동 수집 ---
C_SOURCES = $(wildcard src/kernel/*.c) \
			$(wildcard src/drivers/*/*.c)

# build 디렉토리 아래에 src 구조 그대로 오브젝트 생성
C_OBJECTS = $(patsubst src/%.c, build/%.o, $(C_SOURCES))

# loader는 고정
OBJECTS = build/arch/i386/loader.o $(C_OBJECTS)

all: build/kernel.elf

build:
	mkdir -p build

build/kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o build/kernel.elf

build/os.iso: build/kernel.elf
	cp build/kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
				-b boot/grub/stage2_eltorito    \
				-no-emul-boot                   \
				-boot-load-size 4               \
				-A os                           \
				-input-charset utf8             \
				-quiet                          \
				-boot-info-table                \
				-o build/os.iso                 \
				iso

run: build/os.iso
	bochs -f bochsrc.txt -q

# --- rules: src 구조 유지하며 build 아래에 .o 생성 ---
build/%.o: src/%.c | build
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS)  $< -o $@

build/%.o: src/%.s | build
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf build iso/boot/kernel.elf

