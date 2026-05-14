#include <protocols/internal/ping.h>
#include <stdlib.h>
#include <string.h>

int create_ping(const msgBody *bdy) {
    if (!bdy) return -1;
    
    // For ping, we expect bdy->BodyType to be bdy_PING
    if (bdy->BodyType != bdy_PING) return -1;
    
    // Ping messages typically don't need complex payload processing
    // Just return success for now
    return 0;
}


int create_ping_response(const msgBody *bdy) {
    if (!bdy) return -1;
    
    // For ping response, we expect bdy->BodyType to be bdy_PING
    if (bdy->BodyType != bdy_PING) return -1;
    
    // Ping response is similar to ping, but indicates acknowledgment
    return 0;
}
