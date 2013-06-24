/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef __CKIT_H__
#define __CKIT_H__

#include <stddef.h>

#define get_enclosing(ptr, type, member) ({                     \
            (type *) ((char *)ptr - offsetof(type, member)); })

#endif /* __CKIT_H__ */
