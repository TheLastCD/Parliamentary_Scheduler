// standard schedule request encoding
// vaguely ASN.1 inspired
#ifndef MSG_MSG_H
#define MSG_MSG_H

#include <msg/body.h>
#include <msg/header.h>
#include <msg/payload.h>
#include <stddef.h>
#include <stdint.h>


// Structs

typedef struct {
  msgHeader hdr;
  msgBody bdy;
  MsgBuffer msg_buff;
} Msg;

// Methods

int encode_msg(const Msg *msg, uint8_t *buf, size_t buf_len);

int decode_msg(Msg *msg, const uint8_t *buf, size_t buf_len);

void free_msg(Msg *msg);

#endif // MSG_MSG_H
