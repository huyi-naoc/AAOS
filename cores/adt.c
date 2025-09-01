//
//  adt.c
//  AAOS
//
//  Created by huyi on 18/7/11.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"

#include "wrapper.h"
#include "adt.h"
#include "adt_r.h"

static bool
ThreadsafeQueue_empty(void *_self)
{
    struct ThreadsafeQueue *self = cast(ThreadsafeQueue(), _self);
    struct node *tail;
    
    Pthread_mutex_lock(&self->tail_mutex);
    tail = self->tail;
    Pthread_mutex_unlock(&self->tail_mutex);
    
    return tail == self->head;
}

bool
threadsafe_queue_empty(void *_self)
{
    const struct ThreadsafeQueueClass *class = (const struct ThreadsafeQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeQueueClass()) && class->empty.method) {
        return ((int (*)(void *)) class->empty.method)(_self);
    } else {
        bool result;
        forward(_self, &result, (Method) threadsafe_queue_empty, "empty", _self);
        return result;
    }
}

static bool
ThreadsafeQueue_push(void *_self, void *data)
{
    struct ThreadsafeQueue *self = cast(ThreadsafeQueue(), _self);
    struct node *new_tail;
    
    if ((new_tail = (struct node *) Malloc(sizeof(struct node))) == NULL) {
        return false;
    }
    
    Pthread_mutex_lock(&self->tail_mutex);
    self->tail->data = data;
    new_tail->next = NULL;
    self->tail->next = new_tail;
    self->tail = new_tail;
    Pthread_mutex_unlock(&self->tail_mutex);
    
    Pthread_cond_signal(&self->data_cond);
    
    return true;
}

bool
threadsafe_queue_push(void *_self, void *data)
{
    const struct ThreadsafeQueueClass *class = (const struct ThreadsafeQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeQueueClass()) && class->push.method) {
        return ((int (*)(void *, void *)) class->push.method)(_self, data);
    } else {
        bool result;
        forward(_self, &result, (Method) threadsafe_queue_push, "push", _self, data);
        return result;
    }
}

static void *
ThreadsafeQueue_try_pop(void *_self)
{
    struct ThreadsafeQueue *self = cast(ThreadsafeQueue(), _self);
    struct node *old_head;
    void *data = NULL;
    
    Pthread_mutex_lock(&self->head_mutex);
    if (!threadsafe_queue_empty(self)) {
        old_head = self->head;
        self->head = old_head->next;
        data = old_head->data;
        free(old_head);
    }
    Pthread_mutex_unlock(&self->head_mutex);
    
    return data;
}

void *
threadsafe_queue_try_pop(void *_self)
{
    const struct ThreadsafeQueueClass *class = (const struct ThreadsafeQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeQueueClass()) && class->empty.method) {
        return ((void * (*)(void *)) class->try_pop.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) threadsafe_queue_try_pop, "try_pop", _self);
        return result;
    }
}

static void *
ThreadsafeQueue_wait_and_pop(void *_self)
{
    struct ThreadsafeQueue *self = cast(ThreadsafeQueue(), _self);
    struct node *old_head;
    void *data;
    
    Pthread_mutex_lock(&self->head_mutex);
    while (threadsafe_queue_empty(self)) {
        Pthread_cond_wait(&self->data_cond, &self->head_mutex);
    }
    old_head = self->head;
    self->head = old_head->next;
    data = old_head->data;
    free(old_head);
    Pthread_mutex_unlock(&self->head_mutex);
    
    return data;
}

void *
threadsafe_queue_wait_and_pop(void *_self)
{
    const struct ThreadsafeQueueClass *class = (const struct ThreadsafeQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeQueueClass()) && class->empty.method) {
        return ((void * (*)(void *)) class->wait_and_pop.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) threadsafe_queue_wait_and_pop, "wait_and_pop", _self);
        return result;
    }
}

static void *
ThreadsafeQueue_ctor(void *_self, va_list *app)
{
    struct ThreadsafeQueue *self = super_ctor(ThreadsafeQueue(), _self, app);
    
    self->cleanup = va_arg(*app, void (*)(void *));
    if ((self->head = (struct node *) Malloc(sizeof(struct node))) == NULL) {
        super_delete(ThreadsafeQueue(), _self);
        free(self);
        return NULL;
    }
    self->head->next = NULL;
    self->tail = self->head;
    self->head->data = NULL;
    Pthread_mutex_init(&self->head_mutex, NULL);
    Pthread_mutex_init(&self->tail_mutex, NULL);
    Pthread_cond_init(&self->data_cond, NULL);
    
    return self;
}

static void *
ThreadsafeQueue_dtor(void *_self)
{
    struct ThreadsafeQueue *self = super_dtor(ThreadsafeQueue(), _self);
    
    struct node *iter = self->head, *iter_old;
      
    while (iter != NULL) {
        iter_old = iter;
        iter = iter->next;
        if (self->cleanup) {
            self->cleanup(iter_old->data);
        }
        free(iter_old);
    }
    
    Pthread_mutex_destroy(&self->head_mutex);
    Pthread_mutex_destroy(&self->tail_mutex);
    Pthread_cond_destroy(&self->data_cond);
    
    return self;
}

static void *
ThreadsafeQueueClass_ctor(void *_self, va_list *app)
{
    struct ThreadsafeQueueClass *self = super_ctor(ThreadsafeQueueClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) threadsafe_queue_empty) {
            if (tag) {
                self->empty.tag = tag;
                self->empty.selector = selector;
            }
            self->empty.method = method;
        }
        if (selector == (Method) threadsafe_queue_push) {
            if (tag) {
                self->push.tag = tag;
                self->push.selector = selector;
            }
            self->push.method = method;
        }
        if (selector == (Method) threadsafe_queue_try_pop) {
            if (tag) {
                self->try_pop.tag = tag;
                self->try_pop.selector = selector;
            }
            self->try_pop.method = method;
        }
        if (selector == (Method) threadsafe_queue_wait_and_pop) {
            if (tag) {
                self->wait_and_pop.tag = tag;
                self->wait_and_pop.selector = selector;
            }
            self->wait_and_pop.method = method;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_ThreadsafeQueueClass;

static void
ThreadsafeQueueClass_destroy(void)
{
    free((void *) _ThreadsafeQueueClass);
}

static void
ThreadsafeQueueClass_initialize(void)
{
    _ThreadsafeQueueClass = new(Class(), "ThreadsafeQueueClass", Class(), sizeof(struct ThreadsafeQueueClass),
                                ctor, "ctor", ThreadsafeQueueClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeQueueClass_destroy);
#endif
}

const void *
ThreadsafeQueueClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeQueueClass_initialize);
#endif
    
    return _ThreadsafeQueueClass;
}

static const void *_ThreadsafeQueue;

static void
ThreadsafeQueue_destroy(void)
{
    free((void *) _ThreadsafeQueue);
}

static void
ThreadsafeQueue_initialize(void)
{
    _ThreadsafeQueue = new(ThreadsafeQueueClass(), "ThreadsafeQueue", Object(), sizeof(struct ThreadsafeQueue),
                           ctor, "ctor", ThreadsafeQueue_ctor,
                           dtor, "dtor", ThreadsafeQueue_dtor,
                           threadsafe_queue_empty, "empty", ThreadsafeQueue_empty,
                           threadsafe_queue_push, "push", ThreadsafeQueue_push,
                           threadsafe_queue_try_pop, "try_pop", ThreadsafeQueue_try_pop,
                           threadsafe_queue_wait_and_pop, "try_pop", ThreadsafeQueue_wait_and_pop,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeQueue_destroy);
#endif
}

const void *
ThreadsafeQueue(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeQueue_initialize);
#endif
    
    return _ThreadsafeQueue;
}

/*
 * Threadsafe circular queue or ring buffer.
 * A variant algorithm from Unix Network Programmig (UNP).
 */

static void
ThreadsafeCircularQueue_push(void *_self, void *data)
{

    struct ThreadsafeCircularQueue *self = cast(ThreadsafeCircularQueue(), _self);
    Pthread_mutex_lock(&self->mtx);
    memcpy((char *)self->data + self->put * self->length, data, self->length);
    if (++(self->put) == self->size) {
        self->put = 0;
    }
    Pthread_cond_signal(&self->cond);
    Pthread_mutex_unlock(&self->mtx);
}

void
threadsafe_circular_queue_push(void *_self, void *data)
{
    const struct ThreadsafeQueueClass *class = (const struct ThreadsafeQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeCircularQueueClass()) && class->push.method) {
        return ((void (*)(void *, void *)) class->push.method)(_self, data);
    } else {
       
        forward(_self, 0, (Method) threadsafe_circular_queue_push, "push", _self, data);
    
    }
}

static void
ThreadsafeCircularQueue_pop(void *_self, void *data)
{
    struct ThreadsafeCircularQueue *self = cast(ThreadsafeCircularQueue(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    while (self->get == self->put)  {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    memcpy(data, (const char *)self->data + self->get * self->length, self->length);
    if (++(self->get) == self->size) {
        self->get =  0;
    }
    Pthread_mutex_unlock(&self->mtx);
}

void
threadsafe_circular_queue_pop(void *_self, void *data)
{
    const struct ThreadsafeCircularQueueClass *class = (const struct ThreadsafeCircularQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeCircularQueueClass()) && class->pop.method) {
        return ((void (*)(void *, void *)) class->pop.method)(_self, data);
    } else {
        forward(_self, 0, (Method) threadsafe_circular_queue_pop, "pop", _self, data);
    }
}

static int
ThreadsafeCircularQueue_timed_pop(void *_self, void *data, double timeout)
{
    struct ThreadsafeCircularQueue *self = cast(ThreadsafeCircularQueue(), _self);
    
    struct timespec tp;
    int ret;
    
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout- floor(timeout)) * 1000000000.;
    
    
    Pthread_mutex_lock(&self->mtx);
    while (self->get == self->put)  {
        ret = Pthread_cond_timedwait(&self->cond, &self->mtx, &tp);
        if (ret == ETIMEDOUT) {
            Pthread_mutex_unlock(&self->mtx);
            return -1;
        }
    }
    memcpy(data, (const char *)self->data + self->get * self->length, self->length);
    if (++(self->get) == self->size) {
        self->get =  0;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    return 0;
}

int
threadsafe_circular_queue_timed_pop(void *_self, void *data, double timeout)
{
    const struct ThreadsafeCircularQueueClass *class = (const struct ThreadsafeCircularQueueClass *) classOf(_self);
    
    
    if (isOf(class, ThreadsafeCircularQueueClass()) && class->timed_pop.method) {
        return ((int (*)(void *, void *, double)) class->timed_pop.method)(_self, data, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) threadsafe_circular_queue_timed_pop, "timed_pop", _self, data, timeout);
        return result;
    }
}


static void *
ThreadsafeCircularQueue_ctor(void *_self, va_list *app)
{
    struct ThreadsafeCircularQueue *self = super_ctor(ThreadsafeCircularQueue(), _self, app);
    
    
    self->size = va_arg(*app, size_t);
    self->length = va_arg(*app, size_t);
    self->data = Malloc(self->size * self->length);
    
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    return self;
}

static void *
ThreadsafeCircularQueue_dtor(void *_self)
{
    struct ThreadsafeCircularQueue *self = super_dtor(ThreadsafeCircularQueue(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    free(self->data);
    Pthread_mutex_unlock(&self->mtx);
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);

    return self;
}

static void *
ThreadsafeCircularQueueClass_ctor(void *_self, va_list *app)
{
    struct ThreadsafeCircularQueueClass *self = super_ctor(ThreadsafeCircularQueueClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) threadsafe_circular_queue_pop) {
            if (tag) {
                
                self->pop.tag = tag;
                self->pop.selector = selector;
            }
            self->pop.method = method;
            continue;
        }
        if (selector == (Method) threadsafe_circular_queue_timed_pop) {
            if (tag) {
                
                self->timed_pop.tag = tag;
                self->timed_pop.selector = selector;
            }
            self->timed_pop.method = method;
            continue;
        }
        if (selector == (Method) threadsafe_circular_queue_push) {
            if (tag) {
                
                self->push.tag = tag;
                self->push.selector = selector;
            }
            self->push.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_ThreadsafeCircularQueueClass;

static void
ThreadsafeCircularQueueClass_destroy(void)
{
    free((void *) _ThreadsafeQueueClass);
}

static void
ThreadsafeCircularQueueClass_initialize(void)
{
    _ThreadsafeCircularQueueClass = new(Class(), "ThreadsafeCircularQueueClass", Class(), sizeof(struct ThreadsafeCircularQueueClass),
                                        ctor, "ctor", ThreadsafeCircularQueueClass_ctor,
                                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeCircularQueueClass_destroy);
#endif
}

const void *
ThreadsafeCircularQueueClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeCircularQueueClass_initialize);
#endif
    
    return _ThreadsafeCircularQueueClass;
}

static const void *_ThreadsafeCircularQueue;

static void
ThreadsafeCircularQueue_destroy(void)
{
    free((void *) _ThreadsafeCircularQueue);
}

static void
ThreadsafeCircularQueue_initialize(void)
{
    _ThreadsafeCircularQueue = new(ThreadsafeCircularQueueClass(), "ThreadsafeCircularQueue", Object(), sizeof(struct ThreadsafeCircularQueue),
                                   ctor, "ctor", ThreadsafeCircularQueue_ctor,
                                   dtor, "dtor", ThreadsafeCircularQueue_dtor,
                                   threadsafe_circular_queue_push, "push", ThreadsafeCircularQueue_push,
                                   threadsafe_circular_queue_pop, "pop", ThreadsafeCircularQueue_pop,
                                   threadsafe_circular_queue_timed_pop, "pop", ThreadsafeCircularQueue_timed_pop,
                                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeCircularQueue_destroy);
#endif
}

const void *
ThreadsafeCircularQueue(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeCircularQueue_initialize);
#endif
    
    return _ThreadsafeCircularQueue;
}


/*
 * Threadsafe list
 */

void
threadsafe_list_push_front(void *_self, void *data)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    
    if (isOf(class, ThreadsafeListClass()) && class->push_front.method) {
        ((void (*)(void *, void *)) class->push_front.method)(_self, data);
    } else {
        forward(_self, 0, (Method) threadsafe_list_push_front, "push_front", _self, data);
    }
}

static void
ThreadsafeList_push_front(void *_self, void *data)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *new_node = (struct l_node *) Malloc(sizeof(struct l_node));
    
    new_node->data = data;
    Pthread_mutex_init(&new_node->mutex, NULL);
    
    Pthread_mutex_lock(&self->head.mutex);
    new_node->next = self->head.next;
    self->head.next = new_node;
    Pthread_mutex_unlock(&self->head.mutex);
}

void
threadsafe_list_foreach(void *_self, disposition func, ...)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    va_list ap;
    
    va_start(ap, func);
    if (isOf(class, ThreadsafeListClass()) && class->foreach.method) {
        ((void (*)(void *, disposition, va_list *)) class->foreach.method)(_self, func, &ap);
    } else {
        forward(_self, 0, (Method) threadsafe_list_foreach, "foreach", _self, func, &ap);
    }
    va_end(ap);
}

static void
ThreadsafeList_foreach(void *_self, disposition func, va_list *app)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *current = &self->head, *next;
    pthread_mutex_t *mutex = &self->head.mutex, *mutex_next;
    
    Pthread_mutex_lock(mutex);
    while ((next = current->next)) {
        mutex_next = &next->mutex;
        Pthread_mutex_lock(mutex_next);
        Pthread_mutex_unlock(mutex);
        
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        func(next->data, &ap);
        va_end(ap);
#else
        func(next->data, *app);
#endif
        current = next;
        mutex = mutex_next;
    }
    Pthread_mutex_unlock(mutex);
}

void *
threadsafe_list_find_first_if(void *_self, predict pred, ...)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    va_list ap;
    void *result;
    
    va_start(ap, pred);
    if (isOf(class, ThreadsafeListClass()) && class->find_fisrt_if.method) {
        result = ((void * (*)(void *, predict, va_list *)) class->find_fisrt_if.method)(_self, pred, &ap);
    } else {
        forward(_self, &result, (Method) threadsafe_list_find_first_if, "find_first_if", _self, pred, &ap);
    }
    va_end(ap);
    
    return result;
}

static void *
ThreadsafeList_find_first_if(void *_self, predict pred, va_list *app)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *current = &self->head, *next;
    pthread_mutex_t *mutex = &self->head.mutex, *mutex_next;
    void *result;
    
    Pthread_mutex_lock(mutex);
    while ((next = current->next)) {
        mutex_next = &next->mutex;
        Pthread_mutex_lock(mutex_next);
        Pthread_mutex_unlock(mutex);
        
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        if (pred(next->data, &ap)) {
            result = next->data;
            Pthread_mutex_unlock(mutex_next);
            va_end(ap);
            return result;
        }
        va_end(ap);
#else
        if (pred(next->data, *app)) {
            result = next->data;
            Pthread_mutex_unlock(mutex_next);
            return result;
        }
#endif
        current = next;
        mutex = mutex_next;
    }
    Pthread_mutex_unlock(mutex);
    
    return NULL;
}

void
threadsafe_list_operate_first_if(void *_self, predict pred, disposition func, ...)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    va_list ap;
    
    
    va_start(ap, func);
    if (isOf(class, ThreadsafeListClass()) && class->operate_first_if.method) {
        ((void (*)(void *, predict, disposition, va_list *)) class->operate_first_if.method)(_self, pred, func, &ap);
    } else {
        forward(_self, (void *) 0, (Method) threadsafe_list_operate_first_if, "operate_first_if", _self, pred, func, &ap);
    }
    va_end(ap);
}

static void
ThreadsafeList_operate_first_if(void *_self, predict pred, disposition func, va_list *app)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *current = &self->head, *next;
    pthread_mutex_t *mutex = &self->head.mutex, *mutex_next;
    void *result;
    
    Pthread_mutex_lock(mutex);
    while ((next = current->next)) {
        mutex_next = &next->mutex;
        Pthread_mutex_lock(mutex_next);
        Pthread_mutex_unlock(mutex);
        
#ifdef va_copy
        va_list ap, ap2;
        va_copy(ap, *app);
        if (pred(next->data, &ap)) {
            result = next->data;
            va_copy(ap2, *app);
            func(result, &ap2);
            va_end(ap2);
            Pthread_mutex_unlock(mutex_next);
            va_end(ap);
            return;
        }
        va_end(ap);
#else
        if (pred(next->data, *app)) {
            result = next->data;
            func(result, *app);
            Pthread_mutex_unlock(mutex_next);
            return;
        }
#endif
        current = next;
        mutex = mutex_next;
    }
    Pthread_mutex_unlock(mutex);
}

void
threadsafe_list_insert_if(void *_self, void *data, predict pred, ...)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    va_list ap;
    
    va_start(ap, pred);
    if (isOf(class, ThreadsafeListClass()) && class->insert_if.method) {
        ((void (*)(void *, void *, predict, va_list *)) class->insert_if.method)(_self, data, pred, &ap);
    } else {
        forward(_self, 0, (Method) threadsafe_list_insert_if, "insert_if", _self, data, pred, &ap);
    }
    va_end(ap);
    
}

static void
ThreadsafeList_insert_if(void *_self, void *data, predict pred, va_list *app)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *current = &self->head, *next;
    pthread_mutex_t *mutex = &self->head.mutex, *mutex_next;
    
    
    Pthread_mutex_lock(mutex);
    while ((next = current->next)) {
        mutex_next = &next->mutex;
        Pthread_mutex_lock(mutex_next);
        Pthread_mutex_unlock(mutex);
        
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        if (pred(next->data, &ap)) {
            struct l_node *new_node = (struct l_node *) Malloc(sizeof(struct l_node));
            Pthread_mutex_init(&new_node->mutex, NULL);
            new_node->data = data;
            new_node->next = next->next;
            next->next = new_node;
            Pthread_mutex_unlock(mutex_next);
            va_end(ap);
        }
        va_end(ap);
#else
        if (pred(next->data, *app)) {
            struct l_node *new_node = (struct l_node *) Malloc(sizeof(struct l_node));
            Pthread_mutex_init(&new_node->mutex, NULL);
            new_node->data = data;
            new_node->next = next->next;
            next->next = new_node;
            Pthread_mutex_unlock(mutex_next);
        }
#endif
        current = next;
        mutex = mutex_next;
    }
    Pthread_mutex_unlock(mutex);
}

void
threadsafe_list_remove_if(void *_self, predict pred, ...)
{
    const struct ThreadsafeListClass *class = (const struct ThreadsafeListClass *) classOf(_self);
    va_list ap;
    
    va_start(ap, pred);
    if (isOf(class, ThreadsafeListClass()) && class->remove_if.method) {
        ((void (*)(void *, predict, va_list *)) class->remove_if.method)(_self, pred, &ap);
    } else {
        forward(_self, 0, (Method) threadsafe_list_remove_if, "remove_if", _self, pred, &ap);
    }
    va_end(ap);
}

static void
ThreadsafeList_remove_if(void *_self, predict pred, va_list *app)
{
    struct ThreadsafeList *self = cast(ThreadsafeList(), _self);
    struct l_node *current = &self->head, *next;
    pthread_mutex_t *mutex = &self->head.mutex, *mutex_next;
    
    Pthread_mutex_lock(mutex);
    while ((next = current->next)) {
        mutex_next = &next->mutex;
        Pthread_mutex_lock(mutex_next);
        Pthread_mutex_unlock(mutex);
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        if (pred(next->data, &ap)) {
            struct l_node *old_next = current->next;
            current->next = next->next;
            Pthread_mutex_unlock(mutex_next);
            Pthread_mutex_destroy(&old_next->mutex);
            if (self->cleanup) {
                self->cleanup(old_next->data);
            }
            free(old_next);
        } else {
            Pthread_mutex_unlock(mutex);
            current = next;
            mutex = mutex_next;
        }
        va_end(ap);
#else
        if (pred(next->data, *app)) {
            struct l_node *old_next = current->next;
            current->next = next->next;
            Pthread_mutex_unlock(mutex_next);
            Pthread_mutex_destroy(&old_next->mutex);
            if (self->cleanup) {
                self->cleanup(old_next->data);
            }
            free(old_next);
        } else {
            Pthread_mutex_unlock(mutex);
            current = next;
            mutex = mutex_next;
        }
#endif
    }
    Pthread_mutex_unlock(mutex);
    
}

static void *
ThreadsafeList_ctor(void *_self, va_list *app)
{
    struct ThreadsafeList *self = super_ctor(ThreadsafeList(), _self, app);
    
    Pthread_mutex_init(&self->head.mutex, NULL);
    self->cleanup = (cleanup) va_arg(*app, cleanup);
    
    return (void *) self;
}

static bool
always_true(void *data, va_list *app)
{
    return true;
}

static void *
ThreadsafeList_dtor(void *_self)
{
    struct ThreadsafeList *self = super_dtor(ThreadsafeList(), _self);
    
    threadsafe_list_remove_if(self, always_true);
    Pthread_mutex_destroy(&self->head.mutex);
    
    return (void *) self;
}


static void *
ThreadsafeListClass_ctor(void *_self, va_list *app)
{
    struct ThreadsafeListClass *self = super_ctor(ThreadsafeListClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) threadsafe_list_push_front) {
            if (tag) {
                self->push_front.tag = tag;
                self->push_front.selector = selector;
            }
            self->push_front.method = method;
        }
        
        if (selector == (Method) threadsafe_list_find_first_if) {
            if (tag) {
                self->find_fisrt_if.tag = tag;
                self->find_fisrt_if.selector = selector;
            }
            self->find_fisrt_if.method = method;
        }

         if (selector == (Method) threadsafe_list_operate_first_if) {
            if (tag) {
                self->operate_first_if.tag = tag;
                self->operate_first_if.selector = selector;
            }
            self->operate_first_if.method = method;
        }
        
        if (selector == (Method) threadsafe_list_remove_if) {
            if (tag) {
                self->remove_if.tag = tag;
                self->remove_if.selector = selector;
            }
            self->remove_if.method = method;
        }
        
        if (selector == (Method) threadsafe_list_foreach) {
            if (tag) {
                self->foreach.tag = tag;
                self->foreach.selector = selector;
            }
            self->foreach.method = method;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_ThreadsafeListClass;

static void
ThreadsafeListClass_destroy(void)
{
    free((void *) _ThreadsafeListClass);
}

static void
ThreadsafeListClass_initialize(void)
{
    _ThreadsafeListClass = new(Class(), "ThreadsafeListClass", Class(), sizeof(struct ThreadsafeListClass),
                               ctor, "ctor", ThreadsafeListClass_ctor,
                               (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeListClass_destroy);
#endif
}

const void *
ThreadsafeListClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeListClass_initialize);
#endif
    
    return _ThreadsafeListClass;
}

static const void *_ThreadsafeList;

static void
ThreadsafeList_destroy(void)
{
    free((void *) _ThreadsafeList);
}

static void
ThreadsafeList_initialize(void)
{
    _ThreadsafeList = new(ThreadsafeListClass(), "ThreadsafeList", Object(), sizeof(struct ThreadsafeList),
                          ctor, "ctor", ThreadsafeList_ctor,
                          dtor, "dtor", ThreadsafeList_dtor,
                          threadsafe_list_push_front, "push_front", ThreadsafeList_push_front,
                          threadsafe_list_foreach, "foreach", ThreadsafeList_foreach,
                          threadsafe_list_remove_if, "remove_if", ThreadsafeList_remove_if,
                          threadsafe_list_insert_if, "insert_if", ThreadsafeList_insert_if,
                          threadsafe_list_find_first_if, "find_first_if", ThreadsafeList_find_first_if,
                          threadsafe_list_operate_first_if, "operate_first_if", ThreadsafeList_operate_first_if,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThreadsafeList_destroy);
#endif
}

const void *
ThreadsafeList(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThreadsafeList_initialize);
#endif
    
    return _ThreadsafeList;
}


#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_ADT_PRIORITY_)));

static void
__destructor__(void)
{
    ThreadsafeQueue_destroy();
    ThreadsafeQueueClass_destroy();
    ThreadsafeCircularQueue_destroy();
    ThreadsafeCircularQueueClass_destroy();
    ThreadsafeList_destroy();
    ThreadsafeListClass_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_ADT_PRIORITY_)));

static void
__constructor__(void)
{
    ThreadsafeQueueClass_initialize();
    ThreadsafeQueue_initialize();
    ThreadsafeCircularQueueClass_initialize();
    ThreadsafeQueue_initialize();
    ThreadsafeListClass_initialize();
    ThreadsafeList_initialize();
}
#endif
