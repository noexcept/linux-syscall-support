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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#undef NDEBUG

#include <assert.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/wait.h>

#include "linux_syscall_support.h"

/* Make sure the kernel doesn't overwrite more mem than we expect */
#define _PAD_POISON 0xba
#define _PAD_SIZE 0x1000
#define _PAD_TO_OBJ(buf) ((void *)((uintptr_t)(buf) + _PAD_SIZE))
#define _PAD_FROM_OBJ(obj) ((void *)((uintptr_t)(obj) - _PAD_SIZE))

void *_pad_buf_get(size_t obj_len) {
  size_t len = obj_len + _PAD_SIZE * 2;
  void *ret = malloc(len);
  return _PAD_TO_OBJ(ret);
}
#define PAD_BUF_GET(obj) _pad_buf_get(sizeof(*obj))

void _pad_buf_poison(void *buf, size_t obj_len) {
  size_t len = obj_len + _PAD_SIZE * 2;
  memset(buf, _PAD_POISON, len);
}
#define PAD_BUF_POISON(obj) _pad_buf_poison(_PAD_FROM_OBJ(obj), sizeof(*obj))

void _pad_buf_check(void *_buf, size_t obj_len) {
  size_t len = obj_len + _PAD_SIZE * 2;
  size_t i;
  unsigned char *buf = (unsigned char *)_buf;

  for (i = 0; i < _PAD_SIZE; ++i)
    if (buf[i] != _PAD_POISON)
      goto err;
  for (i = _PAD_SIZE + obj_len; i < len; ++i)
    if (buf[i] != _PAD_POISON)
      goto err;
  return;

 err:
  fprintf(stderr, "buf[%#zx] = %#x (should have been %#x)\n",
    i, buf[i], _PAD_POISON);
  fprintf(stderr, "sizeof(obj) = %#zx; perhaps it should be larger?\n",
    obj_len);
  abort();
}
#define PAD_BUF_CHECK(obj) _pad_buf_check(_PAD_FROM_OBJ(obj), sizeof(*obj))

#define PAD_BUF_FREE(obj) free(_PAD_FROM_OBJ(obj))

void dump_buf(const void *buf, size_t obj_len) {
  const uint8_t *u8 = (const uint8_t *)buf;
  size_t i;

  for (i = 0; i < obj_len; ++i)
    printf("%02x ", u8[i]);
  printf("\n");
}

void assert_buffers_eq_len(const void *buf1, const void *buf2, size_t len) {
  const uint8_t *u8_1 = (const uint8_t *)buf1;
  const uint8_t *u8_2 = (const uint8_t *)buf2;
  size_t i;

  for (i = 0; i < len; ++i)
    if (u8_1[i] != u8_2[i])
      printf("offset %zu: %02x != %02x\n", i, u8_1[i], u8_2[i]);
}
#define assert_buffers_eq(obj1, obj2) assert_buffers_eq_len(obj1, obj2, sizeof(*obj1))
