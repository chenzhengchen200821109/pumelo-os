#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

/* This file contains the definitions for memory management in our OS. */

/* global segment number */
#define SEG_KTEXT       1
#define SEG_KDATA       2
#define SEG_VIDEO_KDATA 3
#define SEG_TSS         4
#define SEG_UTEXT       5
#define SEG_UDATA       6

/* global descriptor numbers */
#define GD_KTEXT       ((SEG_KTEXT) << 3)        // kernel text
#define GD_KDATA       ((SEG_KDATA) << 3)        // kernel data
#define GD_VIDEO_KDATA ((SEG_VIDEO_KDATA) << 3)
#define GD_UTEXT       ((SEG_UTEXT) << 3)        // user text
#define GD_UDATA       ((SEG_UDATA) << 3)        // user data
#define GD_TSS         ((SEG_TSS) << 3)        // task segment selector

#define DPL_KERNEL     (0)
#define DPL_USER       (3)

#define KERNEL_CS      ((GD_KTEXT) | DPL_KERNEL)
#define KERNEL_DS      ((GD_KDATA) | DPL_KERNEL)
#define KERNEL_GS      ((GD_VIDEO_KDATA) | DPL_KERNEL)
#define USER_CS        ((GD_UTEXT) | DPL_USER)
#define USER_DS        ((GD_UDATA) | DPL_USER)

#endif /* _MEMLAYOUT_H */

