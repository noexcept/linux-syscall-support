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

// The stack for the thread.
char stack[32 * 1024];

// The exit status of the child.
const int exit_status = 13;

int callback(void *arg) {
  _exit(exit_status);
}

int main(int argc, char *argv[]) {
  pid_t pid;

  // When the child exits, it'll wake the futex at this address.
  int ctid;
  pid = sys_set_tid_address(&ctid);
  assert(pid == getpid());

  // Test creating a thread which is why we have to manage the stack.
  // We set the stack to the middle of the buffer so we don't have to
  // worry about stack-grows-up vs stack-grows-down architectures.
  pid = sys_clone(callback, stack + (sizeof(stack) / 2),
                  CLONE_FS | CLONE_VM | CLONE_CHILD_CLEARTID,
                  &argc, NULL, NULL, &ctid);
  assert(pid > 0);

  // Wait for the child to signal it's exited.
  sys_futex(&ctid, FUTEX_WAKE, 1, NULL, NULL, 0);

  // Check its exit status.
  int status;
  assert(waitpid(pid, &status, __WCLONE) == pid);
  assert(WIFEXITED(status));
  assert(WEXITSTATUS(status) == exit_status);

  return 0;
}
