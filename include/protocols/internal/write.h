#ifndef PROTOCOLS_INTERNAL_WRITE_H
#define PROTOCOLS_INTERNAL_WRITE_H

#include <msg/body.h>
#include <stdint.h>

int create_write(const msgBody *bdy);
int create_write_response(const msgBody *bdy);

#endif // PROTOCOLS_INTERNAL_WRITE_H
