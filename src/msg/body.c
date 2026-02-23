#include <msg/body.h>

int bdy_parse(const uint8_t *data, size_t data_len, 
                         msgBody **body_out) {
    if (data_len < FIXED_BDY_SIZE) return -1;
    
    size_t total_len = FIXED_BDY_SIZE + (data_len - FIXED_BDY_SIZE);
    msgBody *body = malloc(sizeof(msgBody) + (total_len - FIXED_BDY_SIZE - 1));
    if (!body) return -1;
    
    memcpy(body, data, total_len);
    body->BodyType = (bdy_type)data[0];
    body->ReturnType = (bdr_ret)data[1];
    
    *body_out = body;
    return 0;
}
