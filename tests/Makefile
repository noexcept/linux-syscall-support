# Copyright 2018, Google Inc.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

top_srcdir ?= ..

CFLAGS ?= -g -pipe
CFLAGS += -Wall -Wstrict-prototypes
CPPFLAGS += -I$(top_srcdir)
LDFLAGS += -static

TESTS = \
	clone \
	dup \
	dup2 \
	dup3 \
	fork \
	getpgrp \
	lseek \
	mmap \
	open \
	open-cpp \
	pipe \
	pipe2 \
	read \
	readlink \
	statfs \
	unlink \
	unlinkat \
	write \

all: check

%_test: %.c test_skel.h $(top_srcdir)/linux_syscall_support.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

%_test: %.cc test_skel.h $(top_srcdir)/linux_syscall_support.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

%_run: %_test
	@t=$(@:_run=_test); \
	echo "./$$t"; \
	if ! env -i ./$$t; then \
		env -i strace -f -v ./$$t; \
		echo "TRY: gdb -q -ex r -ex bt ./$$t"; \
		exit 1; \
	fi

check: $(TESTS:=_run)

cross:
	@for cc in \
		"x86_64-pc-linux-gnu-gcc" \
		"i686-pc-linux-gnu-gcc" \
		"x86_64-pc-linux-gnu-gcc -mx32" \
		"armv7a-unknown-linux-gnueabi-gcc -marm -mhard-float" \
		"armv7a-unknown-linux-gnueabi-gcc -mthumb -mhard-float" \
		"powerpc-unknown-linux-gnu-gcc" \
		"aarch64-unknown-linux-gnu-gcc" \
		"mips64-unknown-linux-gnu-gcc -mabi=64" \
		"mips64-unknown-linux-gnu-gcc -mabi=32" \
		"mips64-unknown-linux-gnu-gcc -mabi=n32" \
		"s390-ibm-linux-gnu-gcc" \
		"s390x-ibm-linux-gnu-gcc" \
	; do \
		echo "Testing $$cc"; \
		$(MAKE) -s clean; \
		$(MAKE) -k --no-print-directory $(TESTS:=_test) CC="$$cc"; \
		echo; \
		case $$cc in \
		mips64*-mabi=32) continue;; \
		s390-*) continue;; \
		esac; \
		set -- $$cc; \
		tuple=$${1%-gcc}; \
		shift; \
		cc="clang -target $$tuple $$*"; \
		case $$tuple in \
		x86_64*) ;; \
		*) cc="$$cc --sysroot /usr/$$tuple";; \
		esac; \
		echo "Testing $$cc"; \
		$(MAKE) -s clean; \
		$(MAKE) -k --no-print-directory $(TESTS:=_test) CC="$$cc"; \
		echo; \
	done

clean:
	rm -f *~ *.o *_test *_run a.out core
	rm -f foo.?????? sys_foo.??????

.SUFFIXES:
.PHONY: all check clean
.SECONDARY: $(TESTS:=_run) $(TESTS:=_test)
