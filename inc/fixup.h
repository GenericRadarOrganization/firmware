#ifndef FIXUP_H
#define FIXUP_H

#include "MKL26Z4.h"
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

#define LED  (1U << 5)

#define OS_ADAPTER_ACTIVE_OS 0
#define OS_ADAPTER_SDK 1

#define OS_Lock __enable_irq
#define OS_Unlock __disable_irq

#define OS_Mutex_create() 0
#define OS_Mutex_destroy( n )
#define OS_Mutex_lock( n ) OS_Lock()
#define OS_Mutex_unlock( n ) OS_Unlock()


#define OS_Mem_copy( src, dst, len ) memcpy(dst,src,len)
#define OS_Mem_zero( dst, len ) memset(dst, 0, len)

typedef uint8_t os_mutex_handle;

#endif