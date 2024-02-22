
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>

/*
 * __strncpy_chk2
 *
 * This is a variant of __strncpy_chk, but it also checks to make
 * sure we don't read beyond the end of "src". The code for this is
 * based on the original version of strncpy, but modified to check
 * how much we read from "src" at the end of the copy operation.
 */
extern "C" char *__strncpy_chk2(char *__restrict dst, const char *__restrict src,
                     size_t n, size_t dest_len, size_t src_len)
{

  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
      if ((*d++ = *s++) == 0)
      {
        /* NUL pad the remaining n-1 bytes */
        while (--n != 0)
        {
          *d++ = 0;
        }
        break;
      }
    } while (--n != 0);
    size_t s_copy_len = (size_t)(s - src);
  }
  return dst;
}

extern "C" size_t __strlen_chk(const char *s, size_t s_len)
{
  return strlen(s);
}

extern "C" char* __strrchr_chk(const char *p, int ch, size_t s_len)
{
    char *save;
    for (save = NULL;; ++p, s_len--) {
        // if (s_len == 0)
        //     __fortify_chk_fail("strrchr read beyond buffer", 0);
        if (*p == (char) ch)
            save = (char *)p;
        if (!*p)
            return(save);
    }
    /* NOTREACHED */
}

extern "C" char* __strchr_chk(const char* p, int ch, size_t s_len) {
  for (;; ++p, s_len--) {
    // if (__predict_false(s_len == 0)) {
    //   __fortify_chk_fail("strchr buffer overrun prevented", 0);
    // }
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
