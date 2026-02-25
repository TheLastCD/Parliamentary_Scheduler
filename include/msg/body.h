// standard schedule request encoding
// vaguely ASN.1 inspired

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// encoding macros
#define FIXED_BDY_SIZE 2

// Enums
typedef enum {
  bdy_NONE, // 0x01 ... for ease
  bdy_READ,
  bdy_WRITE,
  bdy_COPY,
  bdy_PING,
} bdy_type;

typedef enum {
  bdy_IGNORE,
  bdy_CONFIRM,
  bdy_MSG,
} bdr_ret;


// Structs

typedef struct {
  bdy_type BodyType;
  bdr_ret ReturnType;
} msgBody;

// functions

int decode_body(msgBody *bdy, const uint8_t *buf, size_t buf_len, 
                           size_t expected_body_len); 
// uint8_t bdy_type_parse(bdy_type *type);
// uint8_t bdy_ret_parse(bdr_ret type);
// uint8_t bdy_buff_parse(uint8_t *MsgBuffer);
