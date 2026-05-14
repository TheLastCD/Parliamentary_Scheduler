#include <assert.h>
#include <stdio.h>

#include <msg/body.h>
#include <protocols/internal/ping.h>

int test_ping_command(void) {
    msgBody ping_body = {
        .BodyType = bdy_PING,
        .ReturnType = bdy_CONFIRM,
    };

    assert(create_ping(&ping_body) == 0);

    msgBody bad_body = {
        .BodyType = bdy_READ,
        .ReturnType = bdy_CONFIRM,
    };
    assert(create_ping(&bad_body) == -1);
    assert(create_ping(NULL) == -1);

    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing ping command\n");
    assert(test_ping_command() == 1);
    printf("--ping command successful\n");
    return 0;
}
#endif
