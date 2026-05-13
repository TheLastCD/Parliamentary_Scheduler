#include <msg/header.h>
#include <string.h>


size_t decode_header(msgHeader *hdr, const uint8_t *buf, size_t buf_len) {
    if (buf_len < FIXED_HDR_SIZE) return 0;  // Changed from -1 to 0 since size_t can't be negative
    
    size_t offset = 0;
    hdr->Requester = buf[offset++];
    hdr->PriorityRequested = (hdr_reqpriority)buf[offset++];
    hdr->SeqNum = buf[offset++];
    hdr->Localref = buf[offset++];
    hdr->BodyLen = buf[offset++];
    
    return offset;
}

int encode_header(msgHeader *hdr, uint8_t *buf) {
    if (!buf) return -1;
    
    size_t offset = 0;
    buf[offset++] = hdr->Requester;
    buf[offset++] = (uint8_t)hdr->PriorityRequested;
    buf[offset++] = hdr->SeqNum;
    buf[offset++] = hdr->Localref;
    buf[offset++] = hdr->BodyLen;
    
    return offset;
}


