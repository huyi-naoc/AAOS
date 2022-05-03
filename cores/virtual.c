//
//  virtual.c
//  AAOS
//
//  Created by huyi on 2018/7/10.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "virtual.h"
#include "virtual_r.h"
#include "wrapper.h"

/*
 * virtual function has lower precedence than delegation (e.g., multi-inherit).
 * be careful
 */

Method
virtualTo(const void *_self, const char *tag)
{
    const struct VirtualTableClass *class = (const struct VirtualTableClass *) classOf((_self));
    
    if (isOf(class, VirtualTableClass()) && class->virtualTo.method) {
        return ((Method (*)(const void *, const char *)) class->virtualTo.method)(_self, tag);
    } else {
        Method result;
        forward(_self, &result, (Method) virtualTo, "virtualTo", _self, tag);
        return result;
    }
}

Method
VirtualTable_virtualTo(const void *_self, const char *tag)
{
    if (tag && *tag) {
        const struct VirtualTable *self = cast(VirtualTable(), _self);
        const struct Method *p = &self->dummy;
        size_t nmeth = (sizeOf(self) - offsetof(struct VirtualTable, dummy)) / sizeof(struct Method);
        
        do {
            if (p->tag && strcmp(p->tag, tag) == 0) {
                return p->method;
            }
            p++;
        } while (--nmeth);
    }
    
    return 0;
}

int
VirtualTable_puto(const void *_self, FILE *fp)
{
    const struct VirtualTable *self = cast(VirtualTable(), _self);
    
    const struct Method *p = &self->dummy;
    size_t nmeth = (sizeOf(self) - offsetof(struct VirtualTable, dummy)) / sizeof(struct Method);
    
    fprintf(fp, "%s totoally have %zd virtual methods: \n", nameOf(classOf(self)), nmeth);
    
    do {
        fprintf(fp, "\t%s\n", p->tag);
        p++;
    } while (--nmeth);
    
    return 0;
}

static void *
VirtualTable_ctor(void *_self, va_list *app)
{
    struct VirtualTable *self = super_ctor(VirtualTable(), _self, app);
    
    self->dummy.method = 0;
    self->dummy.tag = "dummy";
    self->dummy.selector = 0;
    
    return (void *) self;
}

static void *
VirtualTableClass_ctor(void *_self, va_list *app)
{
    struct VirtualTableClass *self = super_ctor(VirtualTableClass(), _self, app);
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
        
        if (selector == (Method) virtualTo) {
            if (tag) {
                self->virtualTo.tag = tag;
                self->virtualTo.selector = selector;
            }
            self->virtualTo.method = method;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_VirtualTableClass;

static void
VirtualTableClass_destroy(void)
{
    free((void *) _VirtualTableClass);
}

static void
VirtualTableClass_initialize(void)
{
    _VirtualTableClass = new(Class(), "VirtualTableClass", Class(), sizeof(struct VirtualTableClass),
                             ctor, "", VirtualTableClass_ctor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualTableClass_destroy);
#endif
    
}

const void *
VirtualTableClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, VirtualTableClass_initialize);
#endif
    return _VirtualTableClass;
}

static const void *_VirtualTable;

static void
VirtualTable_destroy(void)
{
    free((void *) _VirtualTable);
}

static void
VirtualTable_initialize(void)
{
    _VirtualTable = new(VirtualTableClass(), "VirtualTable", Object(), sizeof(struct VirtualTable),
                        ctor, "ctor", VirtualTable_ctor,
                        puto, "puto", VirtualTable_puto,
                        virtualTo, "virtualTo", VirtualTable_virtualTo,
                        (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualTable_destroy);
#endif
    
}

const void *
VirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, VirtualTable_initialize);
#endif
    return _VirtualTable;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_VIRTUAL_PRIORITY_)));

static void
__destructor__(void)
{
    VirtualTable_destroy();
    VirtualTableClass_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_VIRTUAL_PRIORITY_)));

static void
__constructor__(void)
{
    VirtualTableClass_initialize();
    VirtualTable_initialize();
}
#endif
