#include "msg/msg.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <stdio.h>

int test_msg() {
  uint8_t payload[] = {0x01, 0x02};
  Msg test;

  test.hdr.Requester = 0x1;
  test.hdr.PriorityRequested = hdr_HIGH;
  test.hdr.SeqNum = 0x1;
  test.hdr.Localref = 0x11;
  test.hdr.BodyLen = (uint8_t)sizeof(payload);
  test.bdy.BodyType = bdy_READ;
  test.bdy.ReturnType = bdy_MSG;
  // test.bdy.MsgBuffer = payload;

  uint8_t buff[128];
  int x = encode_msg(&test, buff, sizeof(buff));

  assert(buff[0] == 0x1);
  assert(buff[1] == 0x3);
  assert(buff[2] == 0x1);
  assert(buff[3] == 0x11);
  assert(buff[4] == 0x2);
  assert(buff[5] == 0x1);
  assert(buff[6] == 0x2);
  assert(buff[7] == 0x1);
  // assert(buff[8] == 0x2);

  Msg test_ret;
  int l = decode_msg(&test_ret, buff, sizeof(buff));

  assert(x == 1);
  assert(l == 1);
  assert(test_ret.hdr.Requester == 0x1);
  assert(test_ret.hdr.PriorityRequested == hdr_HIGH);
  assert(test_ret.hdr.BodyLen == (uint8_t)sizeof(payload));
  // assert(test_ret.bdy.MsgBuffer == payload);

  return 1;
};

int main(void) {
  printf("Testing encoding and decoding\n");
  int res = test_msg();
  if (res == 1) {
    printf("--encoding & decoding successful\n");
  }
}
