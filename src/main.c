#include <stdint.h>
#include <stdio.h>

#include "msg/msg.h"

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
  test.bdy.MsgBuffer = payload;

  uint8_t buff[128];
  int x = encode_msg(&test, buff, sizeof(buff));

  Msg test_ret;
  int l = decode_msg(&test_ret, buff, sizeof(buff));

  return x;
};


int main(int argc, char *argv[]) {
  printf("hello from main\n");
  (void)argc;
  (void)argv;

  printf("Testing encoding and decoding\n");
  int res = test_msg();
  if (res == 1) {
    printf("must've worked?\n");
    printf("\n");
  }

  return 0;
}
