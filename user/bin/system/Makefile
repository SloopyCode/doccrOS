#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (c) 2026 sulfurLabs
#
# FILE: Makefile
#
#

CC      := x86_64-elf-gcc
LD      := x86_64-elf-ld
LIBC    := ../../libc
LIBS    := ../../libs

CFLAGS := -ffreestanding -nostdlib -fno-builtin -fno-stack-protector    \
          -fno-PIE -fno-pic -m64 -march=x86-64 -mno-sse -mno-sse2       \
          -mno-mmx -mno-red-zone -Wall -Wextra -std=gnu11               \
          -I$(LIBC)/include                                             \
          -I$(LIBS)/libcpio

LDFLAGS := -nostdlib -static -no-pie -T ../../user.ld

all: clean sulfd.elf

sulfd.elf: sulfd.o rc.o $(LIBC)/build/crt0.o $(LIBC)/build/libc.a $(LIBS)/libcpio/build/libcpio.a
	$(LD) $(LDFLAGS) $(LIBC)/build/crt0.o sulfd.o rc.o $(LIBS)/libcpio/build/libcpio.a $(LIBC)/build/libc.a -o $@

rc.o: rc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC)/build/crt0.o $(LIBC)/build/libc.a:
	$(MAKE) -C $(LIBC)

$(LIBS)/libcpio/build/libcpio.a:
	$(MAKE) -C $(LIBS)/libcpio

clean:
	rm -f *.o sulfd.elf

.PHONY: all clean