#ifndef PROTOCOLS_INTERNAL_PING_H
#define PROTOCOLS_INTERNAL_PING_H

#include "msg/body.h"
#include <stdint.h>


typedef struct{
	uint8_t echo_data;
	uint8_t	stop_byte;
}send_ping_int;



int create_ping(const msgBody *bdy);
int create_ping_response(const msgBody *bdy);

#endif // PROTOCOLS_INTERNAL_PING_H
