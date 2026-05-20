#include <protocols/internal/write.h>
#include <stdlib.h>

int create_write(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_WRITE) return -1;
    return 0;
}

int create_write_response(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_WRITE) return -1;
    return 0;
}
