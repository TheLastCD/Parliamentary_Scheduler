// standard schedule request encoding
// vaguely ASN.1 inspired
#include <msg/body.h>
#include <msg/header.h>
#include <stddef.h>
#include <stdint.h>

// encoding macros
#define FIXED_HDR_SIZE 6
#define FIXED_BDY_SIZE 2

// Enums

// Structs

typedef struct {
  msgHeader hdr;
  msgBody bdy;
} Msg;

// Methods

int encode_msg(const Msg *msg, uint8_t *buf, size_t buf_len);
int decode_msg(Msg *msg, const uint8_t *buf, size_t buf_len);

int parse_message(const uint8_t *data, size_t data_len, msgHeader *hdr,
                  msgBody **body_out);
