#include <assert.h>
#include <stdio.h>

extern int test_msg(void);
extern int test_tape_messages(void);
extern int test_tape_shift(void);
extern int test_tape_empty(void);
extern int test_tape_null(void);
extern int test_tape_last_entry(void);
extern int test_ping_command(void);
extern int test_server_init(void);
extern int test_server_start_stop(void);
extern int test_client_routing(void);
extern int test_client_process(void);
extern int test_callbacks(void);

int main(void) {
    printf("Running combined message, tape, ping, server, client, and callback suite\n");

    assert(test_msg() == 1);
    assert(test_tape_messages() == 1);
    assert(test_tape_shift() == 1);
    assert(test_tape_empty() == 1);
    assert(test_tape_null() == 1);
    assert(test_tape_last_entry() == 1);
    assert(test_ping_command() == 1);
    assert(test_server_init() == 1);
    assert(test_server_start_stop() == 1);
    assert(test_client_routing() == 1);
    assert(test_client_process() == 1);
    assert(test_callbacks() == 1);

    printf("--combined suite successful\n");
    return 0;
}
