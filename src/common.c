#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <include/common.h>

static FILE* dbg = NULL;

void debug(const char* format, ...)
{	
    #ifdef DEBUG
    va_list vargs;
    va_start(vargs, format);
    vfprintf((dbg) ? dbg : stderr,format, vargs);
    va_end(vargs);
    #else 
        return;
    #endif
}
