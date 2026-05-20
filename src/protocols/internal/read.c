#include <protocols/internal/read.h>
#include <stdlib.h>

int create_read(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_READ) return -1;
    return 0;
}

int create_read_response(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_READ) return -1;
    return 0;
}
