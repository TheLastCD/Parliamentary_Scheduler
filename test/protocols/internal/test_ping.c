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

    // Verify the ping payload is 0x90
    size_t p_len = 0;
    const uint8_t *p = ping_get_payload(&p_len);
    assert(p_len == 1);
    assert(p != NULL && p[0] == 0x90);

    // Build a confirm payload for seq 0x05 and verify it contains seq + 0x01
    uint8_t buf[4] = {0};
    size_t out_len = 0;
    assert(ping_build_confirm(0x05, buf, sizeof(buf), &out_len) == 0);
    assert(out_len == 2);
    assert(buf[0] == 0x05);
    assert(buf[1] == 0x01);

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
