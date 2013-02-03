/* Copyright (c) 2017, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test_skel.h"

int fd, sys_fd;
char foo[] = "foo.XXXXXX";
char sys_foo[] = "sys_foo.XXXXXX";

void test_lseek(uint64_t len) {
  struct kernel_stat *kst = PAD_BUF_GET(kst);
  struct stat st;

  assert(ftruncate(fd, len) == 0);
  assert(sys_ftruncate(sys_fd, len) == 0);

  assert(lseek(fd, len, SEEK_SET) == len);
  assert(sys_lseek(sys_fd, len, SEEK_SET) == len);

  PAD_BUF_POISON(kst);
  assert(fstat(fd, &st) == 0);
  assert(sys_fstat(sys_fd, kst) == 0);
  PAD_BUF_CHECK(kst);

  assert(st.st_size == len);
  assert(kst->st_size == len);

  PAD_BUF_POISON(kst);
  assert(stat(foo, &st) == 0);
  assert(sys_stat(foo, kst) == 0);
  PAD_BUF_CHECK(kst);

  assert(st.st_size == len);
  assert(kst->st_size == len);

  PAD_BUF_FREE(kst);
}

int main(int argc, char *argv[])
{
  fd = mkstemp(foo);
  assert(fd != -1);
  sys_fd = mkstemp(sys_foo);
  assert(sys_fd != -1);

  test_lseek(0);
  test_lseek(100);
  test_lseek(0xffffffff);
  test_lseek(0xff87654321);

  assert(close(fd) == 0);
  assert(sys_close(sys_fd) == 0);

  assert(unlink(foo) == 0);
  assert(sys_unlink(sys_foo) == 0);

  assert(access(foo, F_OK) == -1);
  assert(access(sys_foo, F_OK) == -1);

  return 0;
}
