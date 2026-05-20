#ifndef PROTOCOLS_INTERNAL_PING_H
#define PROTOCOLS_INTERNAL_PING_H

#include <msg/msg.h>
#include "msg/body.h"
#include <stdint.h>


typedef struct{
	uint8_t echo_data;
	uint8_t	stop_byte;
}send_ping_int;



int create_ping(const msgBody *bdy);
int create_ping_response(const msgBody *bdy);

// Helpers to retrieve the last-created ping payload and response payload
// Returns pointer to internal buffer and sets length in out_len (may be NULL)
const uint8_t *ping_get_payload(size_t *out_len);
const uint8_t *ping_get_response_payload(size_t *out_len);

// Build a ping confirm payload containing the sequence number and status byte
// outbuf must be at least 2 bytes. Returns 0 on success, -1 on failure.
int ping_build_confirm(uint8_t seqnum, uint8_t *outbuf, size_t buf_len, size_t *out_len);

// Create a `Msg` populated as a PING with an automatic 0x90 payload.
// Caller is responsible for calling `free_msg()` on `out` and freeing `out->msg_buff` when done.
// Returns 0 on success, -1 on failure.
int create_ping_message(Msg *out, uint8_t requester, uint8_t priority, uint8_t seq, uint8_t localref, bdr_ret return_type);

#endif // PROTOCOLS_INTERNAL_PING_H
