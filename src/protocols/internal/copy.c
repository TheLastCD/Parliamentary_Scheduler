#include <protocols/internal/copy.h>
#include <stdlib.h>

int create_copy(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_COPY) return -1;
    return 0;
}

int create_copy_response(const msgBody *bdy) {
    if (!bdy) return -1;
    if (bdy->BodyType != bdy_COPY) return -1;
    return 0;
}
