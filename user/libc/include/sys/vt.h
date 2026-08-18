#pragma once

#include <stdint.h>

// ioctl calls to /dev/vt/ctl
#define VT_IOCTL_CREATE  0   // out: new vt id
#define VT_IOCTL_DESTROY 1   // in: delete id, set id free for other processes
#define VT_IOCTL_GET_ACTIVE 2 // out: active id (0 = no id)
#define VT_IOCTL_SET_ACTIVE 3 // in: activate an id

// ioctl calls to /dev/vt/<id>
#define VT_IOCTL_GET_ID   4 // out: get id
#define VT_IOCTL_ACTIVATE   5 // set visible on screen
#define VT_IOCTL_DEACTIVATE 6 // set unvisible for screen
#define VT_IOCTL_FEED     7  // feed to read() vt_feed_args_t

typedef struct
{
    const void *data;
    uint64_t len;
} vt_feed_args_t;