#include "debug.h"
#include "MKL26Z4.h"
#include "usb_serial.h"
#include <stdio.h>
#include <stdarg.h>

char buff[256];

void debug_printf(const char *fmt, ...)
{
    va_list args;
    uint32_t size;

	va_start(args, fmt);
    size = vsnprintf(buff,256,fmt,args);
    va_end(args);
    usb_serial_write(buff,size);
}