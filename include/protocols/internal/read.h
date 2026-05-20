#ifndef PROTOCOLS_INTERNAL_READ_H
#define PROTOCOLS_INTERNAL_READ_H

#include <msg/body.h>
#include <stdint.h>

int create_read(const msgBody *bdy);
int create_read_response(const msgBody *bdy);

#endif // PROTOCOLS_INTERNAL_READ_H
