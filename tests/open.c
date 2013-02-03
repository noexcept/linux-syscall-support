/* Copyright 2018 Google LLC
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
 *     * Neither the name of Google LLC nor the names of its
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

int main(int argc, char *argv[]) {
  int fd, sys_fd;
  struct stat st, sys_st;

  // Make sure opening an invalid path fails.
  sys_fd = sys_open("/does_not_exist.asdfasdfjalskdjflkasd", O_RDONLY, 0);
  assert(sys_fd == -1);
  assert(errno == ENOENT);

  // Open the same path via C lib & kernel and compare the two fds.
  fd = open("/dev/null", O_RDONLY);
  assert(fd != -1);
  sys_fd = sys_open("/dev/null", O_RDONLY, 0);
  assert(sys_fd != -1);

  memset(&st, 0, sizeof(st));
  memset(&sys_st, 0, sizeof(sys_st));
  assert(fstat(fd, &st) == 0);
  assert(fstat(sys_fd, &sys_st) == 0);

  assert_buffers_eq(&st, &sys_st);

  assert(close(fd) == 0);
  assert(sys_close(sys_fd) == 0);

  return 0;
}
