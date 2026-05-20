#include <assert.h>
#include <stdio.h>

#include <protocols/internal/copy.h>
#include <msg/body.h>

int test_copy_command(void) {
    msgBody copy_body = {
        .BodyType = bdy_COPY,
        .ReturnType = bdy_CONFIRM,
    };

    assert(create_copy(&copy_body) == 0);
    assert(create_copy_response(&copy_body) == 0);
    return 1;
}

#ifndef TEST_SUITE_MAIN
int main(void) {
    printf("Testing copy internal protocol\n");
    assert(test_copy_command() == 1);
    printf("--copy protocol tests successful\n");
    return 0;
}
#endif
