//
//  object.c
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//  This code is a modification from the book Object-Oriented Programming in C.
//  Add move and copy constructor.
//

#include "object.h"

#include "object.h"
#include "object_r.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static struct Object *
Object_new(const void *_self, va_list *app)
{
    const struct Class *self = cast(Class(), _self);
    
    return ctor(allocate(self), app);
}

static struct Object *
Object_copy(const void *_self, const void* _from)
{
    const struct Class *self = cast(Class(), _self);
    
    return cctor(allocate(self), _from);
}

static struct Object *
Object_move(const void *_self, void* _from)
{
    const struct Class *self = cast(Class(), _self);
    
    return mctor(allocate(self), _from);
}

static void *
Object_ctor(void *_self, va_list *app)
{
    struct Object *self = cast(Object(), _self);
    
    return self;
}

static struct Object *
Object_cctor(const void *_self, const void *_from)
{
    struct Object *self = cast(Object(), _self);
    
    return self;
}

static struct Object *
Object_mctor(void *_self, void *_from)
{
    struct Object *self = cast(Object(), _self);
    
    return self;
}

static void *
Object_dtor(void *_self)
{
    struct Object *self = cast(Object(), _self);
    
    return self;
}

static int
Object_puto(const void *_self, FILE *fp)
{
    const struct Class *class;
    const struct Object *self = cast(Object(), _self);
    
    class = classOf(self);
    return fprintf(fp, "%s at %p\n", class->name, self);
}

static void
Object_delete(void *_self)
{
    struct Object *self = cast(Object(), _self);
    
    free(dtor(self));
}

static void
Object_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct Object *self = cast(Object(), _self);
    
    fprintf(stderr, "%s at %p does not answer %s\n", nameOf(classOf(self)), self, name);
    
    assert(0);
}

const struct Class *
classOf(const void *_self)
{
    const struct Object *self = cast(Object(), _self);
    
    return self->class;
}

size_t
sizeOf(const void *_self)
{
    const struct Class *class = classOf(_self);
    
    return class->size;
}

bool
isA(const void *_self, const void *class)
{
    if (_self) {
        const struct Object *self = cast(Object(), _self);
        
        cast(Class(), class);
        
        return classOf(self) == class;
    }
    return false;
}

bool
isOf(const void *_self, const void *class)
{
    if (_self) {
        const struct Class *myClass;
        const struct Object *self = cast(Object(), _self);
        
        cast(Class(), class);
        
        myClass = classOf(self);
        if (class != Object()) {
            while (myClass != class) {
                if (myClass != Object()) {
                    myClass = super(myClass);
                } else {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

const void *
isObject(const void *_self)
{
    assert(_self);
    assert(((struct Object *)_self)->magic == MAGIC);
    return _self;
}


void *
cast(const void *class, const void *_self)
{
    const struct Object *self = isObject(_self);
    const struct Class *myClass = isObject(self->class);
    
    if (class != Object()) {
        isObject(class);
        while (myClass != class) {
            assert(myClass != Object());
            myClass = myClass->super;
        }
    }
    
    return (void *) self;
}

/*
 *  For multi-inheritance purpose, don't touch it!
 */

Method
respondsTo(const void *_self, const char *tag)
{
    if (tag && *tag) {
        const struct Class *class = classOf(_self);
        const struct Method *p = &class->ctor;
        size_t nmeth = (sizeOf(class) - offsetof(struct Class, ctor)) / sizeof(struct Method);
        
        do {
            if (p->tag && strcmp(p->tag, tag) == 0) {
                return p->method ? p->selector : 0;
            }
            p++;
        } while (--nmeth);
        
    }
    
    return 0;
}

static void *
Class_dtor(void *_self)
{
    assert(0);
    
    return 0;
}

static void
Class_delete(void *_self)
{
    struct Class *self = cast(Class(), _self);
    
    fprintf(stderr, "%s: cannot delete class\n", self->name);
}

struct Object *
allocate(const void *_self)
{
    struct Object *object;
    const struct Class *self = cast(Class(), _self);
    
    assert(self->size);
    object = calloc(1, self->size);
    assert(object);
    object->magic = MAGIC;
    object->class = self;
    
    return object;
}

const struct Class *
super(const void *_self)
{
    const struct Class *self = cast(Class(), _self);
    
    return self->super;
}

const char *
nameOf(const void *_self)
{
    const struct Class *self = cast(Class(), _self);
    
    return self->name;
}

static const struct Class _Object;
static const struct Class _Class;

void *
ctor(void *_self, va_list *app)
{
    void *result;
    const struct Class *class = classOf(_self);
    
    if (class->ctor.method) {
        result = ((void * (*)(void *, va_list *)) class->ctor.method)(_self, app);
    } else {
        forward(_self, &result, (Method) ctor, "ctor", _self, app);
    }
    
    return result;
}

void *
super_ctor(const void *_class, void *_self, va_list *app)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->ctor.method);
    
    return  ((void *(*)(void *, va_list *)) superclass->ctor.method)(_self, app);
}

void *
dtor(void *_self)
{
    void *result;
    const struct Class *class = classOf(_self);
    
    if (class->dtor.method) {
        result = ((void * (*)(void *))class->dtor.method)(_self);
    } else {
        forward(_self, &result, (Method) dtor, "dtor", _self);
    }
    
    return result;
}

void *
super_dtor(const void *_class, void *_self)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->dtor.method);
    
    return  ((void *(*)(void *)) superclass->dtor.method)(_self);
}


int
puto(const void *_self, FILE *fp)
{
    int result;
    const struct Class *class = classOf(_self);
    
    if (class->puto.method) {
        result = ((int (*)(const void *, FILE *)) class->puto.method)(_self, fp);
    } else {
        forward(_self, &result, (Method) puto, "puto", _self, fp);
    }
    
    return result;
}

int
super_puto(const void *_class, const void *_self, FILE *fp)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->puto.method);
    
    return ((int (*)(const void *, FILE *)) superclass->puto.method)(_self, fp);
}

/*
 *  For abstract class/virtual function, and multi-inheritance.
 */
void
forward(const void *_self, void *result, Method selector, const char *name, ...)
{
    va_list ap;
    const struct Class *class = classOf(_self);
    
    va_start(ap, name);
    if (class->forward.method) {
        ((void (*)(const void *, void *, Method, const char *, va_list *)) class->forward.method)(_self, result, selector, name, &ap);
    } else {
        assert(0);
    }
    va_end(ap);
}

void
super_forward(const void *_class, const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->forward.method);
    
    ((void (*)(const void *, void *, Method, const char *, va_list *)) superclass->forward.method)(_self, result, selector, name, app);
}

void
delete(void *_self)
{
    if (_self == NULL) {
        return;
    }
    
    const struct  Class *class = classOf(_self);
    
    if (class->delete.method) {
        ((void (*)(void *)) class->delete.method)(_self);
    } else {
        forward(_self, 0, (Method) delete, "delete", _self);
    }
}

void
super_delete(const void *_class, void *_self)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->delete.method);
    
    ((void (*)(void *)) superclass->delete.method)(_self);
}

struct Object *
new(const void *_self, ...)
{
    struct Object *result;
    va_list ap;
    const struct Class *class = cast(Class(), _self);
    
    va_start(ap, _self);
    if (class->new.method) {
        result = ((struct Object *(*)(const void *, va_list *))class->new.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) new, "new", _self, &ap);
    }
    va_end(ap);
    
    return result;
}

struct Object *
super_new(const void *_class, const void *_self, va_list *app)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->new.method);
    
    return ((struct Object * (*)(const void *, va_list *)) superclass->new.method)(_self, app);
}

struct Object *
ocopy(const void *_class, const void *_from)
{
    struct Object *result;
    const struct Class *class = cast(Class(), _class);
    
    if (class->copy.method) {
        result = ((struct Object *(*)(const void *, const void *))class->copy.method)(_class, _from);
    } else {
        forward(_class, &result, (Method) ocopy, "copy", _class, _from);
    }
    
    return result;
}

struct Object *
super_ocopy(const void *_class, const void *_self)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->copy.method);
    
    return ((struct Object * (*)(const void *)) superclass->copy.method)(_self);
}

struct Object *
omove(const void *_self, void *_from)
{
    struct Object *result;
    const struct Class *class = cast(Class(), _from);
    
    if (class->move.method) {
        result = ((struct Object *(*)(void *))class->move.method)(_from);
    } else {
        forward(_from, &result, (Method) omove, "move", _from);
    }
    
    return result;
}

struct Object *
super_omove(const void *_class, void *_self)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->copy.method);
    
    return ((struct Object * (*)(const void *)) superclass->move.method)(_self);
}

/*
 * Copy constructor, for a pointer(probably a class type) member,
 * copy the memory region it points to to a new memory region on the heap.
 */
void *
cctor(void *_self, const void *_from)
{
    void *result;
    const struct Class *class = classOf(_self);
    
    if (class->cctor.method) {
        result = ((void * (*)(void *, const void *)) class->cctor.method)(_self, _from);
    } else {
        forward(_self, &result, (Method) cctor, "cctor", _self, _from);
    }
    
    return result;
}

void *
super_cctor(const void *_class, const void *_from)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->cctor.method);
    
    return ((struct Object * (*)(const void *)) superclass->cctor.method)(_from);
}

/*
 * Move constructor, avoid deep copy.
 */
void *
mctor(void *_self, void *_from)
{
    
    struct Object *result;
    const struct Class *class = classOf(_from);
    
    if (class->mctor.method) {
        result = ((struct Object *(*)(void *))class->mctor.method)(_from);
    } else {
        forward(_from, &result, (Method) mctor, "mctor", _from);
    }
    
    return result;
}

void *
super_mctor(const void *_class, void *_from)
{
    const struct Class *superclass = super(_class);
    
    assert(superclass->mctor.method);
    
    return ((struct Object * (*)(void *)) superclass->mctor.method)(_from);
}

static void *
Class_ctor(void *_self, va_list *app)
{
    struct Class *self = _self;
    const size_t offset = offsetof(struct Class, ctor);
    Method selector;
    va_list ap;
    
    self->name = va_arg(*app, char *);
    self->super = cast(Class(), va_arg(*app, void *));
    self->size = va_arg(*app, size_t);
    
    memcpy((char *) self + offset, (char *) self->super + offset, sizeOf(self->super) - offset);
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) ctor) {
            if (tag) {
                self->ctor.tag = tag;
                self->ctor.selector = selector;
            }
            self->ctor.method = method;
            continue;
        }
        if (selector == (Method) dtor) {
            if (tag) {
                self->dtor.tag = tag;
                self->dtor.selector = selector;
            }
            self->dtor.method = method;
            continue;
        }
        if (selector == (Method) cctor) {
            if (tag) {
                self->cctor.tag = tag;
                self->cctor.selector = selector;
            }
            self->cctor.method = method;
            continue;
        }
        if (selector == (Method) mctor) {
            if (tag) {
                self->mctor.tag = tag;
                self->mctor.selector = selector;
            }
            self->mctor.method = method;
            continue;
        }
        if (selector == (Method) puto) {
            if (tag) {
                self->puto.tag = tag;
                self->puto.selector = selector;
            }
            self->puto.method = method;
            continue;
        }
        if (selector == (Method) forward) {
            if (tag) {
                self->forward.tag = tag;
                self->forward.selector = selector;
            }
            self->forward.method = method;
            continue;
        }
        if (selector == (Method) new) {
            if (tag) {
                self->new.tag = tag;
                self->new.selector = selector;
            }
            self->new.method = method;
            continue;
        }
        if (selector == (Method) delete) {
            if (tag) {
                self->delete.tag = tag;
                self->delete.selector = selector;
            }
            self->delete.method = method;
            continue;
        }
        if (selector == (Method) ocopy) {
            if (tag) {
                self->copy.tag = tag;
                self->copy.selector = selector;
            }
            self->copy.method = method;
            continue;
        }
        if (selector == (Method) omove) {
            if (tag) {
                self->move.tag = tag;
                self->move.selector = selector;
            }
            self->move.method = method;
            continue;
        }
        if (selector == (Method) delete) {
            if (tag) {
                self->delete.tag = tag;
                self->delete.selector = selector;
            }
            self->delete.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}


inline const void *
Object(void)
{
    return &_Object;
}

inline const void *
Class(void)
{
    return &_Class;
}

static const struct Class _Object = {
    {MAGIC, &_Class},
    "Object", &_Object, sizeof(struct Object),
    {"",		(Method) 0,			(Method) Object_ctor},
    {"",		(Method) 0,			(Method) Object_dtor},
    {"puto",	(Method) puto,		(Method) Object_puto},
    {"forward",	(Method) forward,	(Method) Object_forward},
    {"delete",	(Method) delete,	(Method) Object_delete},
    {"",        (Method) 0,         (Method) Object_cctor},
    {"",        (Method) 0,         (Method) Object_mctor},
    {"",		(Method) 0,			(Method) Object_new},
    {"",        (Method) 0,         (Method) Object_copy},
    {"",        (Method) 0,         (Method) Object_move},
};

static const struct Class _Class = {
    {MAGIC, &_Class},
    "Class", &_Object, sizeof(struct Class),
    {"",		(Method) 0,			(Method) Class_ctor},
    {"",		(Method) 0,			(Method) Class_dtor},
    {"puto",	(Method) puto,		(Method) Object_puto},
    {"forward",	(Method) forward,	(Method) Object_forward},
    {"delete",	(Method) delete,	(Method) Class_delete},
    {"",        (Method) 0,         (Method) Object_cctor},
    {"",        (Method) 0,         (Method) Object_mctor},
    {"",		(Method) 0,			(Method) Object_new},
    {"",        (Method) 0,         (Method) Object_copy},
    {"",        (Method) 0,         (Method) Object_move},
};
