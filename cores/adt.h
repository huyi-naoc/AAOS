//
//  adt.h
//  AAOS
//
//  Created by huyi on 18/7/11.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __adt_h
#define __adt_h

#include "object.h"

#ifdef __cpluspus
extern "C" {
#endif

bool threadsafe_queue_push(void *_self, void *data);
void *threadsafe_queue_try_pop(void *_self);
void *threadsafe_queue_wait_and_pop(void *_self);
bool threadsafe_queue_empty(void *_self);

extern const void *ThreadsafeQueue(void);
extern const void *ThreadsafeQueueClass(void);

typedef void (*cleanup)(void *);
typedef bool (*predict)(void *, va_list *app);
typedef void (*disposition)(void *, va_list *app);

void threadsafe_circular_queue_push(void *_self, void *data);
void threadsafe_circular_queue_pop(void *_self, void *data);
/**
   
 */
int threadsafe_circular_queue_timed_pop(void *_self, void *data, double timeout);

extern const void *ThreadsafeCircularQueue(void);
extern const void *ThreadsafeCircularQueueClass(void);

void threadsafe_list_push_front(void *_self, void *data);
void threadsafe_list_foreach(void *_self, disposition func, ...);
void *threadsafe_list_find_first_if(void *_self, predict pred, ...);
void threadsafe_list_insert_if(void *_self, void *data, predict pred, ...);
void threadsafe_list_remove_if(void *_self, predict pred, ...);
void threadsafe_list_operate_first_if(void *_self, predict pred, disposition func, ...);

extern const void *ThreadsafeList(void);
extern const void *ThreadsafeListClass(void);

#ifdef __cplusplus
}
#endif

#endif /* adt_h */
