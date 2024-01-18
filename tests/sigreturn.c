/* Copyright 2023 Google LLC
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

/* Test that signal handler can return */

volatile int alarm_triggered = 0;

void alarm_sigaction(int sig, siginfo_t* info, void* ucontext) {
  alarm_triggered = 1;
}

int main(int argc, char *argv[]) {
  // Ensure all signals are unblocked before test.
  sigset_t empty_mask;
  sigemptyset(&empty_mask);
  int err = sigprocmask(SIG_SETMASK, &empty_mask, NULL);
  assert(err == 0);

  struct kernel_sigaction act = {
    .sa_sigaction_ = alarm_sigaction,
  };
  // Block SIGUSR1 and check that it is unblocked when alarm_sigaction returns
  // {rt_}sigreturn should restore signal mask.
  sys_sigemptyset(&act.sa_mask);
  sys_sigaddset(&act.sa_mask, SIGUSR1);
  err = sys_sigaction(SIGALRM, &act, NULL);
  assert(err == 0);

  // Set up a real-time alarm
  const struct timeval zero_timeval = { .tv_sec = 0, .tv_usec = 0 };
  const struct timeval one_micro_sec_timeval = { .tv_sec = 0, .tv_usec = 1 };
  struct itimerval itv = {
    .it_interval = zero_timeval,
    .it_value = one_micro_sec_timeval,
  };

  // The alarm should trigger very quickly.
  err = setitimer(ITIMER_REAL, &itv, NULL);
  assert(err == 0);

  for (int i = 0; i < 1000000 && !alarm_triggered; ++i)
    ;
  assert(alarm_triggered);

  // Verify that SIGUSR1 is unblocked after signal is handled.
  sigset_t mask_after_sigreturn;
  err = sigprocmask(SIG_SETMASK, NULL, &mask_after_sigreturn);
  assert(err == 0);
  assert(!sigismember(&mask_after_sigreturn, SIGUSR1));

  return 0;
}

