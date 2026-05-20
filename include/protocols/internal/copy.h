#ifndef PROTOCOLS_INTERNAL_COPY_H
#define PROTOCOLS_INTERNAL_COPY_H

#include <msg/body.h>
#include <stdint.h>

int create_copy(const msgBody *bdy);
int create_copy_response(const msgBody *bdy);

#endif // PROTOCOLS_INTERNAL_COPY_H
