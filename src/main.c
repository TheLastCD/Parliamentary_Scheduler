#include <stdint.h>
#include <stdio.h>
#include <scheduler/tape.h>
#include <msg/msg.h>

int main(int argc, char *argv[]) {
  printf("Parliamentary Scheduler starting...\n");
  
  // Demonstrate message encoding/decoding
  uint8_t payload[] = {0x01, 0x02};
  Msg test_msg;
  
  test_msg.hdr.Requester = 0x1;
  test_msg.hdr.PriorityRequested = hdr_HIGH;
  test_msg.hdr.SeqNum = 0x1;
  test_msg.hdr.Localref = 0x11;
  test_msg.hdr.BodyLen = (uint8_t)sizeof(payload);
  test_msg.bdy.BodyType = bdy_READ;
  test_msg.bdy.ReturnType = bdy_CONFIRM;
  test_msg.msg_buff = payload;

  uint8_t buffer[64];
  int encode_result = encode_msg(&test_msg, buffer, sizeof(buffer));
  printf("Message encoding: %s\n", encode_result ? "SUCCESS" : "FAILED");
  
  Msg decoded_msg;
  int decode_result = decode_msg(&decoded_msg, buffer, sizeof(buffer));
  printf("Message decoding: %s\n", decode_result ? "SUCCESS" : "FAILED");
  
  if (decode_result) {
    printf("Decoded - Requester: %d, Priority: %d, BodyLen: %d\n", 
           decoded_msg.hdr.Requester, decoded_msg.hdr.PriorityRequested, decoded_msg.hdr.BodyLen);
    free_msg(&decoded_msg);
  }
  
  // Demonstrate tape machine
  tape_machine tape = {0};
  printf("Tape machine initialized\n");
  
  int head_action = act_on_head(&tape);
  printf("Head action result: %d (no entries expected)\n", head_action);
  
  printf("Parliamentary Scheduler demo complete.\n");
  
  (void)argc;
  (void)argv;

  return 0;
}
