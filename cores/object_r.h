//
//  object_r.h
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//  This code is a modification from the book Object-Oriented Programming in C.
//

#ifndef __object_r_h
#define __object_r_h

#include "object.h"

struct Method {
    const char *tag;
    Method selector;
    Method method;
};

struct Object {
    unsigned long magic;
    const void *class;
};

struct Class {
    struct Object _;
    const char *name;
    const void *super;
    size_t size;
    
    struct Method ctor;
    struct Method dtor;
    struct Method puto;
    struct Method forward;
    struct Method delete;
    struct Method cctor;
    struct Method mctor;
    struct Method new;
    struct Method copy;
    struct Method move;
};

void *super_ctor(const void *_class, void *_self, va_list *app);
void *super_cctor(const void *_class, const void *_from);
void *super_mctor(const void *_class, void *_from);
void *super_dtor(const void *_class, void *_self);
int super_puto(const void *_class, const void *_self, FILE *fp);
void super_forward(const void *_class, const void *_self, void *result, Method selector, const char *name, va_list *app);
void super_delete(const void *_class, void *_self);
struct Object *super_new(const void *_class, const void *_self, va_list *app);
struct Object *super_copy(const void *_class, const void *_self);
struct Object *super_move(const void *_class, void *_self);

#define MAGIC   0x0effaced

#endif /* object_r_h */
