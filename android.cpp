
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <assert.h>

#define __predict_true(exp)     __builtin_expect((exp), 1)
#define __predict_false(exp)    __builtin_expect((exp), 0)
/*
 * __strncpy_chk2
 *
 * This is a variant of __strncpy_chk, but it also checks to make
 * sure we don't read beyond the end of "src". The code for this is
 * based on the original version of strncpy, but modified to check
 * how much we read from "src" at the end of the copy operation.
 */
extern "C" char* __strncpy_chk2(char* __restrict dst, const char* __restrict src,
              size_t n, size_t dest_len, size_t src_len)
{
  if (__predict_false(n > dest_len)) {
    assert(false && "strncpy: prevented write past end of buffer");
  }
  if (n != 0) {
    char* d = dst;
    const char* s = src;
    do {
      if ((*d++ = *s++) == 0) {
        /* NUL pad the remaining n-1 bytes */
        while (--n != 0) {
          *d++ = 0;
        }
        break;
      }
    } while (--n != 0);
    size_t s_copy_len = static_cast<size_t>(s - src);
    if (__predict_false(s_copy_len > src_len)) {
      assert(false && "strncpy: prevented read past end of buffer");
    }
  }
  return dst;
}

extern "C" size_t __strlen_chk(const char *s, size_t s_len)
{
    size_t ret = strlen(s);
    if (__builtin_expect(ret >= s_len, 0)) {
        assert(false && "*** strlen read overflow detected ***\n");
        abort();
    }
    return ret;
}

extern "C" char* __strrchr_chk(const char *p, int ch, size_t s_len)
{
    char *save;
    for (save = NULL;; ++p, s_len--) {
        if (s_len == 0)
            assert(false && "strrchr read beyond buffer");
        if (*p == (char) ch)
            save = (char *)p;
        if (!*p)
            return(save);
    }
    /* NOTREACHED */
}

extern "C" char* __strchr_chk(const char* p, int ch, size_t s_len) {
  for (;; ++p, s_len--) {
    if (__predict_false(s_len == 0)) {
      assert(false && "strchr buffer overrun prevented");
    }
    if (*p == static_cast<char>(ch)) {
      return const_cast<char*>(p);
    }
    if (*p == '\0') {
      return NULL;
    }
  }
  /* NOTREACHED */
}

extern "C" int __FD_ISSET_chk(int fd, fd_set* set) {
  return FD_ISSET(fd, set);
}

extern "C" void __FD_SET_chk(int fd, fd_set* set) {
  FD_SET(fd, set);
}
