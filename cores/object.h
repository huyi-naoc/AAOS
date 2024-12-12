//
//  object.h
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//  This code is a modification from the book Object-Oriented Programming in C.
//

#ifndef __object_h
#define __object_h

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef move
#undef move
#endif

typedef void (*Method)(void);

extern const void *Object(void);

const struct Class *classOf(const void *_self);
size_t sizeOf(const void *_self);
bool isA(const void *_self, const void *class);
bool isOf(const void *_self, const void *class);
void *cast(const void *class, const void *_self);
Method respondsTo(const void * _self, const char *tag);

void *ctor(void *_self, va_list *app);
void *cctor(void *_self, const void *_from);
void *mctor(void *_self, void *_from);

void *dtor(void *_self);
int puto(const void *_self, FILE *fp);
void forward(const void *_self, void *result, Method selector, const char *name, ...);
void delete(void *_self);
struct Object *new(const void *_self, ...);
struct Object *ocopy(const void *_self, const void *_from);
struct Object *omove(const void *_self, void *_from);

extern const void *Class(void);

struct Object *allocate(const void *_self);
const struct Class *super(const void *_self);
const char *nameOf(const void *_self);

#endif /* object_h */
