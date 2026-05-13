#include <msg/body.h>

size_t decode_body(msgBody *bdy, const uint8_t *buf, size_t buf_len, 
                           size_t expected_body_len) {
    if (buf_len < FIXED_BDY_SIZE + expected_body_len) return 0;
    
    size_t offset = 0;
    bdy->BodyType = (bdy_type)buf[offset++];
    bdy->ReturnType = (bdr_ret)buf[offset++];
    
    return offset;
}
