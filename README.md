Hodlminer <img src="https://d25lcipzij17d.cloudfront.net/badge.svg?id=gh&type=6&v=2.4.6&x2=0">

This is a multi-threaded CPU miner for HODL algorithm,
fork of Jeff Garzik's reference cpuminer.

License: GPLv2.  See COPYING for details.

Git tree:   https://github.com/ghobson2013/hodlminer-wolf

Additional contributions were made by:
* Optiminer
* Fuzzbawls
* nicehash
* pooler
* TheTDD

Dependencies:
	libcurl			http://curl.haxx.se/libcurl/
	jansson			http://www.digip.org/jansson/
		(jansson is included in-tree)

Basic *nix build instructions:
```
        sudo apt-get install libcurl-dev
	./autogen.sh	# only needed if building from git repo
	./nomacro.pl	# only needed if building on Mac OS X or with Clang
	./configure CFLAGS="-O3 -march=native" --with-curl
	make
```
	Note: see install instructions for static building in ./install-static-instructions.txt

### Windows build steps:
install in order:
  - [GNUstep MSYS System](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/gnustep-msys-system-0.30.0-setup.exe)
  - [GNUstep Core](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/gnustep-core-0.35.0-setup.exe)
  - [GNUstep Devel](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/gnustep-devel-1.4.0-setup.exe)
  - [GNUstep Cairo](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/gnustep-cairo-0.35.0-setup.exe)
  - [ProjectCenter](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/ProjectCenter-0.6.2-35-setup.exe)
  - [Gorm](http://ftpmain.gnustep.org/pub/gnustep/binaries/windows/gorm-1.2.23-35-setup.exe)

**GNUstep Cairo**, **ProjectCenter**, **Gorm** are probably not needed but install them anyways.

WARNING:: DO NOT INSTALL THIS ON A MOUNTED CIFS/NFS share, it has to be on NTFS as it uses REPARSE files.

Intsall [Win-builds 1.5.0](http://win-builds.org/1.5.0/win-builds-1.5.0.exe) and choose and choose **Cygwin or MYSY**, and select **x86_64**
set path to **/c/bin/GNUstep/msys/** (Or the path you have installed GNUstep).
Clone github repo **https://github.com/ghobson2013/hodlminer-wolf**.
Open **C:\bin\GNUstep\msys\1.0\msys.bat** and cd to cloned repo path.
Run the following commands:
  - **PATH="/c/bin/GNUstep/msys/opt/windows_64/bin/:$PATH"** (or the path you have installed GNUstep) to add opt/windows_64 to path.
  - run **./autogen**
  - use one of the scripts **mingw64.sh** (build native extension for CPU), **mingw64aes.sh** for AES support, **mingw64sse2.sh** for SSE2 support, **mingw64avx(+aes).sh** for AVX(_AES) support and **mingw64avx2(+aes).sh** for AVX2(_AES) support
  - the built **hodlminer.exe** binary requires libraries (copy them from or set PATH to **C:\bin\GNUstep\msys\opt\windows_64\bin** when running):
    - libcrypto.dll
    - libcrypto-1.0.0.dll
    - libcurl-4.dll
    - libgcc_s_dw2-1.dll
    - libgcc_s_seh-1.dll
    - libgmp-10.dll
    - libgmpxx-4.dll
    - libjansson-4.dll
    - libjson-c-2.dll
    - libsasl.dll
    - libsigc-2.0-0.dll
    - libssl.dll
    - libssl-1.0.0.dll
    - libstdc++-6.dll
    - libwinpthread-1.dll
    - libz-1.dll

```
Architecture-specific notes:
	ARM:	No runtime CPU detection. The miner can take advantage
		of some instructions specific to ARMv5E and later processors,
		but the decision whether to use them is made at compile time,
		based on compiler-defined macros.
		To use NEON instructions, add "-mfpu=neon" to CFLAGS.
	PowerPC: No runtime CPU detection.
		To use AltiVec instructions, add "-maltivec" to CFLAGS.
	x86:	The miner checks for SSE2 instructions support at runtime,
		and uses them if they are available.
	x86-64:	The miner can take advantage of AVX, AVX2 and XOP instructions,
		but only if both the CPU and the operating system support them.
		    * Linux supports AVX starting from kernel version 2.6.30.
		    * FreeBSD supports AVX starting with 9.1-RELEASE.
		    * Mac OS X added AVX support in the 10.6.8 update.
		    * Windows supports AVX starting from Windows 7 SP1 and
		      Windows Server 2008 R2 SP1.
		The configure script outputs a warning if the assembler
		doesn't support some instruction sets. In that case, the miner
		can still be built, but unavailable optimizations are left off.
		The miner uses the VIA Padlock Hash Engine where available.
```

Usage instructions:  Run "hodlminer --help" to see options.


Connecting through a proxy:  Use the --proxy option.
To use a SOCKS proxy, add a socks4:// or socks5:// prefix to the proxy host.
Protocols socks4a and socks5h, allowing remote name resolving, are also
available since libcurl 7.18.0.
If no protocol is specified, the proxy is assumed to be a HTTP proxy.
When the --proxy option is not used, the program honors the http_proxy
and all_proxy environment variables.
