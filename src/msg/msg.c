#include "msg/msg.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int encode_msg(const Msg *msg, uint8_t *buf, size_t buf_len) {

  size_t bdy_len = msg->hdr.BodyLen;
  size_t total_size = FIXED_HDR_SIZE + FIXED_BDY_SIZE + bdy_len;

  if (buf_len < total_size) {
    return 0;
  }

  size_t offset = 0;

  buf[offset++] = msg->hdr.Requester;
  buf[offset++] = msg->hdr.PriorityRequested;
  buf[offset++] = msg->hdr.SeqNum;
  buf[offset++] = msg->hdr.Localref;
  buf[offset++] = (uint8_t)bdy_len;

  buf[offset++] = msg->bdy.BodyType;
  buf[offset++] = msg->bdy.ReturnType;

  if (bdy_len > 0 && msg->msg_buff) {
    memcpy(&buf[offset], msg->msg_buff, bdy_len);
  }

  return 1;
}

int decode_msg(Msg *msg, const uint8_t *buf, size_t buf_len) {

  size_t offset = decode_header(&msg->hdr, buf, buf_len);
  if (offset == 0) return 0;  // Changed from < 0 to == 0
  
  size_t body_offset = decode_body(&msg->bdy, buf + offset, buf_len - offset, msg->hdr.BodyLen);
  if (body_offset == 0) return 0;  // Changed from < 0 to == 0
  
  offset += body_offset;

  size_t body_len = msg->hdr.BodyLen;
  if (buf_len < offset + body_len) {
    return 0; // buffer too small
  }

  if (body_len > 0) {
    msg->msg_buff = malloc(body_len);
    if (!msg->msg_buff) {
      return 0; // malloc failed
    }
    memcpy(msg->msg_buff, &buf[offset], body_len);
  } else {
    msg->msg_buff = NULL;
  }

  return 1;
}

void free_msg(Msg *msg) {
  if (msg && msg->msg_buff) {
    free(msg->msg_buff);
    msg->msg_buff = NULL;
  }
}
