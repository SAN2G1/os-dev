# =============================================================================
#  Simple OS Development Makefile
# =============================================================================

# --- Tools & Flags ---
CC      = i686-elf-gcc
LD      = i686-elf-ld
# -Iinclude: 헤더 파일을 include 폴더에서 찾도록 설정
CFLAGS  = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
          -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -T link.ld -melf_i386
AS      = nasm
ASFLAGS = -f elf

# =============================================================================
#  Sources & Objects Automation
# =============================================================================

# 1. C 소스 파일 자동 수집
# 커널(kernel)과 드라이버(drivers) 폴더 하위의 모든 .c 파일
C_SOURCES = $(wildcard src/kernel/*.c) \
            $(wildcard src/drivers/*/*.c) \
            $(wildcard src/klib/*.c) \
            $(wildcard src/*.c)

# 2. 어셈블리 소스 파일 자동 수집
# 아키텍처(arch) 폴더 하위의 모든 .s 파일 (loader.s, io.s 등)
# [중요] 기존 src/*.s는 파일을 못 찾으므로 경로를 명시해야 합니다.
ASM_SOURCES = $(wildcard src/arch/i386/*.s)


# 3. 오브젝트 파일 경로 변환 (src/xxx.c -> build/xxx.o)
C_OBJECTS   = $(patsubst src/%.c, build/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst src/%.s, build/%.o, $(ASM_SOURCES))

# =============================================================================
#  Linking Strategy (Link Order)
# =============================================================================

# [중요] 부트 로더(loader.o)는 반드시 메모리 맨 앞에 위치해야 합니다.
LOADER_OBJ = build/arch/i386/loader.o

# [중요] ASM_OBJECTS 리스트에 loader.o가 이미 포함되어 있으므로, 
# 중복 링크를 막기 위해 filter-out으로 제거하고 나머지만 남깁니다. (예: io.o)
OTHER_ASM_OBJS = $(filter-out $(LOADER_OBJ), $(ASM_OBJECTS))

# 최종 링크 목록: [로더] + [나머지 어셈블리] + [C 오브젝트]
OBJECTS = $(LOADER_OBJ) $(OTHER_ASM_OBJS) $(C_OBJECTS)


# =============================================================================
#  Build Rules
# =============================================================================

all: build/kernel.elf

build:
	mkdir -p build

# 커널 링크 (Linking)
build/kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o build/kernel.elf

# ISO 이미지 생성
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

# Bochs 실행
run: build/os.iso
	bochs -f bochsrc.txt -q

# --- 컴파일 규칙 (Generic Rules) ---

# C 파일 컴파일 (.c -> .o)
# $(dir $@)는 타겟 파일의 디렉토리 경로를 의미 (폴더가 없으면 생성)
build/%.o: src/%.c | build
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS)  $< -o $@

# 어셈블리 파일 컴파일 (.s -> .o)
build/%.o: src/%.s | build
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# 청소
clean:
	rm -rf build iso/boot/kernel.elf
