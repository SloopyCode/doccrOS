#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (c) 2026 sulfurLabs
#
# PROJECT: sulfurOS
# FILE: Makefile
#

CC     := x86_64-elf-gcc
AR     := x86_64-elf-ar
RANLIB := x86_64-elf-ranlib
LD     := x86_64-elf-ld
LIBC   := ../../libc

CFLAGS := -ffreestanding -nostdlib -fno-builtin -fno-stack-protector \
          -fno-PIE -fno-pic -m64 -march=x86-64 -mno-sse -mno-sse2 \
          -mno-mmx -mno-red-zone -Wall -Wextra -std=gnu11 \
          -I$(LIBC)/include

SRCS := libcpio.c
OBJS := $(SRCS:%.c=build/%.o)

all: build/libcpio.a

build/%.o: %.c libcpio.h | build
	$(CC) $(CFLAGS) -c $< -o $@

build/libcpio.a: $(OBJS) | build
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean