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
	readlink \
	statfs \
	unlink \
	unlinkat \

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
