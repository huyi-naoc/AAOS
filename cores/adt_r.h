//
//  adt_r.h
//  AAOS
//
//  Created by huyi on 18/7/11.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __adt_r_h
#define __adt_r_h

#include "object_r.h"

struct node {
    void *data;
	void (*cleanup)(struct node *);
    struct node *next;
};

struct dnode {
    void *data;
    struct dnode *prev;
    struct dnode *next;
};

struct Queue {
    const struct Object _;
    void *_vtab;	
};

struct QueueClass {
    const struct Class _;
    struct Method push;
    struct Method try_pop;
    struct Method wait_and_pop;
    struct Method empty;
};

struct ThreadsafeQueue {
    const struct Object _;
    struct node *head;
    struct node *tail;
    void (*cleanup)(void *);
    pthread_mutex_t head_mutex;
    pthread_mutex_t tail_mutex;
    pthread_cond_t data_cond;
};

struct ThreadsafeQueueClass {
    const struct Class _;
    struct Method push;
    struct Method wait_and_pop;
    struct Method try_pop;
    struct Method empty;
};

struct ThreadsafeCircularQueue {
    const struct Object _;
    size_t size; /* Number of ring buffers in the queue. */
    size_t get;
    size_t put;
    size_t length; /* The length of one buffer. */
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    void *data;
};

struct ThreadsafeCircularQueueClass {
    const struct Class _;
    struct Method push;
    struct Method pop;
    struct Method timed_pop;
    struct Method empty;
};

struct l_node {
    void *data;
    struct l_node *next;
    pthread_mutex_t mutex;
};

struct ThreadsafeList {
    const struct Object _;
    struct l_node head;
    void (*cleanup)(void *);
};

struct ThreadsafeListClass {
    const struct Class _;
    struct Method push_front;
    struct Method insert_if;
    struct Method find_fisrt_if;
    struct Method operate_first_if;
    struct Method remove_if;
    struct Method foreach;
};

struct LinkList {
    const struct Object _;
    struct node head;
};

struct LinkListClass {
    const struct Class _;
    struct Method push_front;
    struct Method insert_if;
    struct Method find_first_if;
    struct Method operate_first_if;
    struct Method remove_if;
    struct Method foreach;
};


struct DoubleLinkList {
    const struct Object _;
    void (*cleanup)(void *);
    struct dnode *head;
    struct dnode *tail;
};

struct DoubleLinkListClass {
    struct Method push_front;
    struct Method append;
    struct Method find_fisrt_if;
    struct Method find_last_if;
    struct Method remove_first_if;
    struct Method remove_last_if;
    struct Method forward_foreach;
    struct Method backward_foreach;
};

struct Node;
struct NodeCountedPtr {
    int external_counter;
    struct Node *ptr;
};

struct NodeCounter {
    unsigned int internal_count : 30;
    unsigned int external_count : 2;
};

/*
struct Node {
    void *data;
    struct NodeCounter count;
    struct NodeCountedPtr next;
};

struct LockFreeQueue {
    const struct Object _;
    struct NodeCountedPtr head;
    struct NodeCountedPtr tail;
};

struct LockFreeQueueClass {
    const struct Object _;
    struct Method push;
    struct Method try_pop;
    struct Method wait_and_pop;
};
 */

#endif /* adt_r_h */
