#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (c) 2026 sulfurLabs
#
# PROJECT: sulfurOS
# FILE: Makefile
#

all:
	@$(MAKE) -C libc
	@$(MAKE) -C bin/hello
	@$(MAKE) -C bin/doomgeneric
	@$(MAKE) -C bin/poweroff
	@$(MAKE) -C bin/reboot

clean:
	@$(MAKE) -C libc clean
	@$(MAKE) -C bin/hello clean
	@$(MAKE) -C bin/doomgeneric clean
	@$(MAKE) -C bin/poweroff clean
	@$(MAKE) -C bin/reboot clean

.PHONY: all clean
