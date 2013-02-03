/* Copyright 2018, Google Inc.  All rights reserved.
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

// Check we can unlink files.
static void unlinkat_file(int fd, char *path) {
  // Make sure it exists.
  int filefd = creat(path, 0755);
  assert(filefd != -1);
  assert(close(filefd) == 0);
  assert(access(path, F_OK) == 0);

  // Then delete it.
  assert(sys_unlinkat(fd, path, 0) == 0);

  // Make sure it's gone.
  assert(access(path, F_OK) != 0);
}

// Check we can rmdirs.
static void unlinkat_dir(int fd, char *path) {
  // Create the dir first.
  assert(mkdir(path, 0755) == 0);

  // Make sure it exists.
  assert(access(path, F_OK) == 0);

  // Then delete it.
  assert(sys_unlinkat(fd, path, AT_REMOVEDIR) == 0);

  // Make sure it's gone.
  assert(access(path, F_OK) != 0);
}

int main(int argc, char *argv[]) {
  // Get a unique path to play with.
  char path[] = "foo.XXXXXX";
  int fd = mkstemp(path);
  assert(fd != -1);
  assert(close(fd) == 0);
  assert(unlink(path) == 0);

  // First test AT_FDCWD.
  unlinkat_file(AT_FDCWD, path);
  unlinkat_dir(AT_FDCWD, path);

  // Then test a real directory handle.
  fd = open(".", O_DIRECTORY);
  assert(fd != -1);
  unlinkat_file(fd, path);
  unlinkat_dir(fd, path);
  assert(close(fd) == 0);

  return 0;
}
