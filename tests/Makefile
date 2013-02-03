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
	unlink \

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

compile_tests: $(TESTS:=_test)

check: $(TESTS:=_run)

# The "foo" targets are the names we use with temp files.
# Clean them out in case some tests crashed in the middle.
clean:
	rm -f *~ *.o *_test *_run a.out core
	rm -f foo.?????? sys_foo.??????

.SUFFIXES:
.PHONY: all check clean compile_tests
.SECONDARY: $(TESTS:=_run) $(TESTS:=_test)

# Try to cross-compile the tests for all our supported arches.  We test with
# both gcc and clang.  We don't support execution (yet?), but just compiling
# helps catch common bugs.
.PHONY: cross compile_cross
cross_compile:
	@echo "Running: $(MAKE) $@ CC='$(CC)' CXX='$(CXX)'"; \
	if (echo '#include <stdio.h>' | $(CC) -x c -c -o /dev/null -) 2>/dev/null; then \
		$(MAKE) -s clean; \
		$(MAKE) -k --no-print-directory compile_tests; \
	else \
		echo "Skipping $(CC) test: not installed"; \
	fi; \
	echo

# The names here are a best effort.  Not easy to probe for.
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
		cxx=`echo "$$cc" | sed 's:-gcc:-g++:'`; \
		$(MAKE) --no-print-directory CC="$$cc" CXX="$$cxx" cross_compile; \
		\
		sysroot=`$$cc --print-sysroot 2>/dev/null`; \
		gccdir=`$$cc -print-file-name=libgcc.a 2>/dev/null`; \
		gccdir=`dirname "$$gccdir"`; \
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
			*) cc="$$cc --sysroot $$sysroot -B$$gccdir -L$$gccdir";; \
		esac; \
		cxx=`echo "$$cc" | sed 's:^clang:clang++:'`; \
		$(MAKE) --no-print-directory CC="$$cc" CXX="$$cxx" cross_compile; \
	done
