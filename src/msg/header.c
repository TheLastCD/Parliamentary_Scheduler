#include <msg/header.h>
#include <string.h>


int hdr_parse(const uint8_t *data, msgHeader *hdr){
	if (!data || !hdr) return -1;
	memcpy(hdr, data, FIXED_HDR_SIZE);
	return 0;
}

int encode_header(msgHeader *header, uint8_t *raw) {
    if (!header || !raw) return -1;
    memcpy(raw, header, FIXED_HDR_SIZE);  
    return 0; 
}
