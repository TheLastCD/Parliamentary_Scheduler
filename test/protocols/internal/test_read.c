#include <assert.h>
#include <stdio.h>

#include <protocols/internal/read.h>
#include <msg/body.h>

int test_read_command(void) {
    msgBody read_body = {
        .BodyType = bdy_READ,
        .ReturnType = bdy_CONFIRM,
    };

    assert(create_read(&read_body) == 0);
    assert(create_read_response(&read_body) == 0);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing read internal protocol\n");
    assert(test_read_command() == 1);
    printf("--read protocol tests successful\n");
    return 0;
}
#endif
