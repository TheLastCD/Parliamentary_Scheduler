// standard schedule request encoding
// vaguely ASN.1 inspired
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
