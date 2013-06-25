/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#ifndef _HASH_H_
#define _HASH_H_

/*
  Based on sdbm hash.
*/
static inline unsigned long partial_byte_hash(unsigned long c, unsigned long hash)
{
    return (c + (hash << 6) + (hash << 16) - hash);
}

static inline unsigned long byte_array_hash(const void *bytes, size_t len)
{
    const unsigned char *ptr = (const unsigned char *)bytes;
    unsigned long hash = 0;

    while (len--)
        hash = partial_byte_hash(*ptr++, hash);

    return hash;
}

static inline unsigned long string_hash(const char *str)
{
    unsigned long hash = 0;
    
    while (*str != '\0') 
        hash = partial_byte_hash(*str++, hash);
    
    return hash;
}

#endif /* _HASH_H */
