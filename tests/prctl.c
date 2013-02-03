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

// Available since Linux 2.3.20.
static void test_pr_dumpable(void)
{
  int exp, ret;

  // Sanity check against the C library.
  exp = prctl(PR_GET_DUMPABLE, 0, 0, 0, 0);
  assert(exp == 0 || exp == 1);
  ret = sys_prctl(PR_GET_DUMPABLE, 0, 0, 0, 0);
  assert(exp == ret);
}

// Available since Linux 2.6.25.
static void test_pr_capbset(void)
{
#ifdef PR_CAPBSET_READ
  int exp, ret;

  // Sanity check against the C library.
  exp = prctl(PR_CAPBSET_READ, CAP_SETUID, 0, 0, 0);
  assert(exp == 0 || exp == 1);
  ret = sys_prctl(PR_CAPBSET_READ, CAP_SETUID, 0, 0, 0);
  assert(exp == ret);

  // See we get an error w/invalid cap.
  errno = 0;
  ret = sys_prctl(PR_CAPBSET_READ, -1000, 0, 0, 0);
  assert(ret == -1);
  assert(errno == EINVAL);
#endif
}

// Available since Linux 2.1.57/2.3.15.
static void test_pr_pdeathsig(void)
{
#ifdef PR_CAPBSET_READ
  int exp, ret;

  // Sanity check against the C library.
  assert(prctl(PR_GET_PDEATHSIG, (uintptr_t)&exp, 0, 0, 0) == 0);
  assert(sys_prctl(PR_GET_PDEATHSIG, (uintptr_t)&ret, 0, 0, 0) == 0);
  // Default parent death signal is 0 (it's cleared).
  assert(exp == 0);
  assert(ret == 0);

  // Try setting the signal to something fun.
  assert(sys_prctl(PR_SET_PDEATHSIG, 1, 0, 0, 0) == 0);
  assert(sys_prctl(PR_GET_PDEATHSIG, (uintptr_t)&ret, 0, 0, 0) == 0);
  assert(ret == 1);
  // Then reset it back.
  assert(sys_prctl(PR_SET_PDEATHSIG, 0, 0, 0, 0) == 0);
  assert(sys_prctl(PR_GET_PDEATHSIG, (uintptr_t)&ret, 0, 0, 0) == 0);
  assert(ret == 0);
#endif
}

int main(int argc, char *argv[])
{
  // Since prctl is a large multiplexed syscall, just try a few simple
  // operations.  Hopefully the rest should "just work".
  test_pr_dumpable();
  test_pr_capbset();
  test_pr_pdeathsig();

  return 0;
}
