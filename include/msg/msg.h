// standard schedule request encoding
// vaguely ASN.1 inspired

#include <stdint.h>
#include <stddef.h>


// encoding macros
#define FIXED_HDR_SIZE 6
#define FIXED_BDY_SIZE 2

// Enums
typedef enum{
  bdy_NONE, //0x01 ... for ease
  bdy_READ,
  bdy_WRITE,
  bdy_COPY,
  bdy_PING,
}bdy_type;


typedef enum{
  bdy_IGNORE, 
  bdy_CONFIRM,
  bdy_MSG,
}bdr_ret;


typedef enum{
  hdr_RED,
  hdr_LOW, 
  hdr_MED,
  hdr_HIGH,
  hdr_INTR,
}hdr_reqpriority;


// Structs

typedef struct {
  bdy_type BodyType;
  bdr_ret ReturnType;
  uint8_t *MsgBuffer; 
} msgBody;

typedef struct {
  uint8_t Requester;
  hdr_reqpriority PriorityRequested; //requesters opinion
  uint8_t SeqNum;
  uint8_t Localref;
  uint8_t BodyLen;
} msgHeader;

typedef struct {
  msgHeader hdr;
  msgBody bdy;
} Msg;

// Methods

int encode_msg(const Msg *msg, uint8_t *buf, size_t buf_len);
int decode_msg(Msg *msg, const uint8_t *buf, size_t buf_len);
