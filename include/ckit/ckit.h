/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef CKIT_H
#define CKIT_H

#include <stddef.h>

#define get_enclosing(ptr, type, member) ({                     \
            (type *) ((char *)ptr - offsetof(type, member)); })

#endif /* CKIT_H */
