#include <msg/body.h>

int decode_body(msgBody *bdy, const uint8_t *buf, size_t buf_len, 
                           size_t expected_body_len) {
    if (buf_len < FIXED_BDY_SIZE) return -1;
    
    size_t offset = 0;
    bdy->BodyType = (bdy_type)buf[offset++];
    bdy->ReturnType = (bdr_ret)buf[offset++];

    size_t msg_data_len = expected_body_len - FIXED_BDY_SIZE;
    if (msg_data_len > (buf_len - FIXED_BDY_SIZE)) return -1;
    
    return offset;
}
