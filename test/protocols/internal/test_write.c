#include <assert.h>
#include <stdio.h>

#include <protocols/internal/write.h>
#include <msg/body.h>

int test_write_command(void) {
    msgBody write_body = {
        .BodyType = bdy_WRITE,
        .ReturnType = bdy_MSG,
    };

    assert(create_write(&write_body) == 0);
    assert(create_write_response(&write_body) == 0);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing write internal protocol\n");
    assert(test_write_command() == 1);
    printf("--write protocol tests successful\n");
    return 0;
}
#endif
