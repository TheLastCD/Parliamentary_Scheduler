#include <msg/header.h>
#include <string.h>


int decode_header(msgHeader *hdr, const uint8_t *buf, size_t buf_len) {
    if (buf_len < FIXED_HDR_SIZE) return -1;
    
    size_t offset = 0;
    hdr->Requester = buf[offset++];
    hdr->PriorityRequested = (hdr_reqpriority)buf[offset++];
    hdr->SeqNum = buf[offset++];
    hdr->Localref = buf[offset++];
    hdr->BodyLen = buf[offset++];
    
    return offset;
}


