/*
   Copyright (c) 2004-2009 Jay Sorg

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

   thread calls

*/

#if !defined(THREAD_CALLS_H)
#define THREAD_CALLS_H

#include "arch.h"
#include "os_calls.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

typedef struct {
    int needed;
    int called;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} barrier_t;


int APP_CC
tc_barrier_init(barrier_t *barrier,int needed);
int APP_CC
tc_barrier_destroy(barrier_t *barrier);
int APP_CC
tc_barrier_wait(barrier_t *barrier);
int APP_CC
tc_thread_create(THREAD_RV (THREAD_CC * start_routine)(void*), void* arg);
tbus APP_CC
tc_get_threadid(void);
int APP_CC
tc_threadid_equal(tbus tid1, tbus tid2);
tbus APP_CC
tc_mutex_create(void);
void APP_CC
tc_mutex_delete(tbus mutex);
int APP_CC
tc_mutex_lock(tbus mutex);
int APP_CC
tc_mutex_unlock(tbus mutex);
tbus APP_CC
tc_sem_create(int init_count);
void APP_CC
tc_sem_delete(tbus sem);
int APP_CC
tc_sem_dec(tbus sem);
int APP_CC
tc_sem_inc(tbus sem);

#endif
