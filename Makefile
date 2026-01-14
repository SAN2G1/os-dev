# --- tools / flags (기존 내용 유지) ---
CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

# --- sources 자동 수집 ---
# 1. C 소스 파일 찾기
C_SOURCES = $(wildcard src/kernel/*.c) \
            $(wildcard src/drivers/*/*.c)

# 2. 어셈블리 소스 파일 찾기 (이 부분이 추가되었습니다!)
# src/drivers 폴더 아래의 모든 .s 파일을 찾습니다. (예: src/drivers/video/io.s)
ASM_SOURCES = $(wildcard src/drivers/*/*.s)


# --- Objects 파일 경로 생성 ---
# C 소스 -> .o 파일 경로 변환
C_OBJECTS = $(patsubst src/%.c, build/%.o, $(C_SOURCES))

# 어셈블리 소스 -> .o 파일 경로 변환 (이 부분이 추가되었습니다!)
ASM_OBJECTS = $(patsubst src/%.s, build/%.o, $(ASM_SOURCES))


# loader는 고정, 나머지는 자동 수집된 객체들 합치기
# $(ASM_OBJECTS)를 끝에 추가하여 io.o가 링크되도록 합니다.
OBJECTS = build/arch/i386/loader.o $(C_OBJECTS) $(ASM_OBJECTS)

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

