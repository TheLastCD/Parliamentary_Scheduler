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

  if (bdy_len > 0 && msg->bdy.MsgBuffer) {
    memcpy(&buf[offset], msg->bdy.MsgBuffer, bdy_len);
  }

  return 1;
}

int decode_msg(Msg *msg, const uint8_t *buf, size_t buf_len) {

  size_t offset = 0;
  msg->hdr.Requester = buf[offset++];
  msg->hdr.PriorityRequested = buf[offset++];
  msg->hdr.SeqNum = buf[offset++];
  msg->hdr.Localref = buf[offset++];
  msg->hdr.BodyLen = buf[offset++];

  msg->bdy.BodyType = buf[offset++];
  msg->bdy.ReturnType = buf[offset++];

  size_t body_len = msg->hdr.BodyLen;
  if (buf_len < (offset + body_len)) {
    return 0; // longer than it should be
  }

  if (body_len > 0) {
    msg->bdy.MsgBuffer = malloc(body_len);
    if (!msg->bdy.MsgBuffer) {
      return 0; // malloc failed
    }
    memcpy(msg->bdy.MsgBuffer, &buf[offset], body_len);
  } 
  else { 
    return 0; //not data
  }

  return 1;
}

int parse_message(const uint8_t *data, size_t data_len, 
                            msgHeader *hdr, msgBody **body_out) {
    if (data_len < FIXED_HDR_SIZE) return -1;
    
    hdr_parse(data, hdr);
    size_t body_len = hdr->BodyLen;
    if (data_len < FIXED_HDR_SIZE + body_len) return -1;
    
    return bdy_parse(data + FIXED_HDR_SIZE, body_len, body_out);
}
