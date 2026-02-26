# 1. SETUP
## 크로스 컴파일러 설정
#크로스-컴파일
 : 개발 환경은 우분투 24.04 이지만 대상이 되는 타깃(i686-elf)을 기준으로 컴파일을 하기 위함이다.
 
  1. 필수 종속성
```bash
sudo apt update
sudo apt install build-essential bison flex git wget curl \
                 libgmp3-dev libmpfr-dev libmpc-dev libisl-dev \
                 texinfo nasm  
```
[필수 종속성](https://wiki.osdev.org/GCC_Cross-Compiler)은 이곳에서 보고 크로스 파일 설정을 하였다. 
`git, wget, curl, nasm`은 이후에 필요할 것 같아. 추가로 설치하여 준다. 

2. 환경 변수와 작업 디렉터리 설정
```
export PREFIX="$HOME/opt/cross"   # 설치 위치
export TARGET=i686-elf            # 타깃 트리플릿
export PATH="$PREFIX/bin:$PATH"   # 새 툴체인 우선 사용
mkdir -p ~/src && cd ~/src

```
3. 소스 내려받기
```
wget https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.xz
tar -xf binutils-2.42.tar.xz
tar -xf gcc-14.1.0.tar.xz

```

4. binutils 컴파일 및 설치
```
mkdir -p build-binutils && cd build-binutils
../binutils-2.42/configure \
    --target="$TARGET"  --prefix="$PREFIX" \
    --with-sysroot      --disable-nls      --disable-werror
make -j$(nproc)
make install
cd ..

```

5. gcc 빌드
```
mkdir -p build-gcc && cd build-gcc
../gcc-14.1.0/configure \
    --target="$TARGET"          --prefix="$PREFIX" \
    --disable-nls               --without-headers \
    --enable-languages=c,c++    --disable-multilib \
    --disable-libssp            --disable-threads \
    --disable-shared            --disable-hosted-libstdcxx
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
```

6. 확인
```
$TARGET-gcc --version          # i686-elf-gcc 14.1.0 ...
$TARGET-ld  -v                 # GNU ld (GNU Binutils) 2.42 ...
```
`CC=i686-elf-gcc`, `LD=i686-elf-ld`

## 가상머신 설정. 
교제에서는 Bochs를 사용한다고 한다. Bochs는 디버깅 기능 덕분에 OS개발에 적합한 x86플랫폼 용 에뮬레이터라고 한다. 
나는 일단 Bochs와 QEMU 둘다 사용해 보려고 한다. QEMU가 Bochs보다 훨씬 빠른 부트를 지원한다고 해서, 테스트시에 유용하게 사용될 것  같아서 이다. 

```
sudo apt update
sudo apt install \
    genisoimage            \
    bochs bochs-sdl        \
    qemu-system-x86 qemu-utils \
    grub-pc-bin mtools xorriso \
    gdb-multiarch

```

| 패키지                              | 용도                                      |
| -------------------------------- | --------------------------------------- |
| **genisoimage**                  | 부팅 ISO 이미지 생성 (`grub-mkrescue` 내부에서 사용) |
| **bochs, bochs-sdl**             | 정확도 높은 x86 에뮬레이터 + SDL 그래픽 프런트엔드        |
| **qemu-system-x86, qemu-utils**  | 빠른 가상 실행·자동 테스트, GDB 원격 디버깅             |
| **grub-pc-bin, mtools, xorriso** | GRUB 부트로더 및 FAT/ISO 하이브리드 이미지 작성 의존성    |
| **gdb-multiarch**                | QEMU `-s -S`와 연결해 커널/부트코드 소스-레벨 디버깅     |

