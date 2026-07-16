<div align="center">
	<h1> doccrOS </h1>

a simple x86_64 Operating System in C made from scratch

	<div>
		<img src="screenshots/screenshot1.png" width="23%" />
		<img src="screenshots/screenshot2.png" width="23%" />
		<img src="screenshots/screenshot3.png" width="23%" />
		<img src="screenshots/screenshot4.png" width="23%" />
	</div>
	<br>
	<a href="https://discord.gg/88mpV6NEr7">
		<img src="https://img.shields.io/badge/Join%20the%20Discord-Black?style=for-the-badge&logo=discord&logoColor=white&color=black" alt="Discord Badge" />
	</a>
</div>

## Build Dependencies
For building and compiling doccrOS, ensure you have the following installed -
- x86_64 GCC cross-compiler - How you install this depends on your OS. Obviously this is the compiler for the OS code. doccrOS only supports x86 64-bit machines for now but in the future it may support more architectures.
- [NASM](https://www.nasm.us/) - Assembler
- [QEMU](https://www.qemu.org/) - Our preferred emulator
- [Xorriso](https://www.gnu.org/software/xorriso/) - To create ISO files
- [Git](https://git-scm.com/) - fetches dependencies 
- [wget](https://www.gnu.org/software/wget/) - fetches dependencies
- [CPIO](https://www.gnu.org/software/cpio/) ("brew install cpio" on macos) - For the RAM disk

## For Nix Users
If you use [Nix](https://nixos.org/) then you can use the provided flake that includes all the required build dependencies so you can set up a development environment for doccrOS out of the box with just a short and simple command.

## Building and Compiling
Now you can finally build & run doccrOS,
- `make fetchDeps` - Fetches all libraries and such that doccrOS depends on.
- `make` - Builds doccrOS
- `make run` - Emulates doccrOS using QEMU
- `make clean` - Cleans up all build outputs

<br/>

##
 - README.md by Voxi0 & emexSW
##
