// standard schedule request encoding
// vaguely ASN.1 inspired

#ifndef MSG_HEADER_H
#define MSG_HEADER_H

#include <stdint.h>
#include <stddef.h>


// encoding macros
#define FIXED_HDR_SIZE 5


typedef enum{
  hdr_RED,
  hdr_LOW, 
  hdr_MED,
  hdr_HIGH,
  hdr_INTR,
}hdr_reqpriority;



// Structs
typedef struct {
    uint8_t Requester;
    hdr_reqpriority PriorityRequested;
    uint8_t SeqNum;
    uint8_t Localref;
    uint8_t BodyLen;
} msgHeader;


size_t decode_header(msgHeader *hdr, const uint8_t *buf, size_t buf_len);
int encode_header(msgHeader *header, uint8_t *raw);

#endif // MSG_HEADER_H


