#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>

#define YM_INIT   pthread_mutexattr_t pthread_mutexattr_default; \
                  pthread_mutexattr_init(&pthread_mutexattr_default); \
                  pthread_mutex_init(&_mutex, &pthread_mutexattr_default);

#define YM_CLOSE  pthread_mutex_destroy(&_mutex);

#define YM_LOCK   pthread_mutex_lock(&_mutex);
#define YM_UNLOCK pthread_mutex_unlock(&_mutex);

#endif // __MUTEX_H__



