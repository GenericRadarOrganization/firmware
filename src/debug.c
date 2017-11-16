#include "debug.h"
#include "MKL26Z4.h"
#include "usb_serial.h"
#include "config.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef ENABLE_USB_DEBUG
char buff[DEBUG_PRINT_BUFF_LEN];

void debug_printf(const char *fmt, ...)
{
    va_list args;
    uint32_t size;

	va_start(args, fmt);
    size = vsnprintf(buff,DEBUG_PRINT_BUFF_LEN,fmt,args);
    va_end(args);
    usb_serial_write(buff,size);
}
#else

void debug_printf(const char *fmt, ...)
{
    // Do nothing
}

#endif