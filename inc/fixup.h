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
#ifndef NVIC_STIR
#define NVIC_STIR			(*(volatile uint32_t *)0xE000EF00)
#endif

#ifndef NVIC_ENABLE_IRQ
#define NVIC_ENABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) = (1 << ((n) & 31)))
#endif

#ifndef NVIC_DISABLE_IRQ
#define NVIC_DISABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E180 + ((n) >> 5)) = (1 << ((n) & 31)))
#endif

#ifndef NVIC_SET_PENDING
#define NVIC_SET_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) = (1 << ((n) & 31)))
#endif

#ifndef NVIC_CLEAR_PENDING
#define NVIC_CLEAR_PENDING(n)	(*((volatile uint32_t *)0xE000E280 + ((n) >> 5)) = (1 << ((n) & 31)))
#endif

#ifndef NVIC_IS_ENABLED
#define NVIC_IS_ENABLED(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) & (1 << ((n) & 31)))
#endif

#ifndef NVIC_IS_PENDING
#define NVIC_IS_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) & (1 << ((n) & 31)))
#endif

#ifndef NVIC_IS_ACTIVE
#define NVIC_IS_ACTIVE(n)	(*((volatile uint32_t *)0xE000E300 + ((n) >> 5)) & (1 << ((n) & 31)))
#endif

#endif