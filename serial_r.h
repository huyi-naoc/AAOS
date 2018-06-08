#ifndef serial_r_h
#define serial_r_h

#include "object_r.h"
#include "virtual_r.h"

struct Serial {
    const struct Object _;
    const void *_vtab;
    char *name;
    char *path;
    char *description;
    
    unsigned int options;
    double read_timeout;
    double write_timeout;
    
    void *read_buffer;
    size_t read_buffer_size;
    size_t read_size;
    
    void *write_buffer;
    size_t write_buffer_size;
    
    pthread_mutex_t mutex;
    int fd;
    
    int state;
};

struct SerialClass {
    const struct Class _;
    struct Method read;
    struct Method write;
    struct Method name_of;
    struct Method path_of;
    struct Method init; 
    struct Method process; 
    struct Method feed_dog;
};

struct SerialVirtualTable {
    struct VirtualTable _;
    struct Method init;
    struct Method process;
    struct Method feed_dog;
};

#endif
