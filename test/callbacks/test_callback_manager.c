#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <callbacks/callback_manager.h>
#include <msg/msg.h>

// Test context structure
typedef struct {
    int callback_count;
    int success_count;
    uint8_t last_requester;
} test_context_t;

// Simple callback - just count invocations
int simple_callback(const Msg *msg, void *context) {
    test_context_t *ctx = (test_context_t *)context;
    ctx->callback_count++;
    ctx->last_requester = msg->hdr.Requester;
    
    printf("[Simple Callback] Invoked (count=%d, requester=0x%02X)\n", 
           ctx->callback_count, msg->hdr.Requester);
    
    return 0;
}

// Filter function - only trigger for specific requesters
bool requester_filter(const Msg *msg, void *context) {
    // Only trigger for requester 0x11
    bool should_trigger = msg->hdr.Requester == 0x11;
    printf("[Filter] Checking requester 0x%02X - %s\n", 
           msg->hdr.Requester, should_trigger ? "PASS" : "FAIL");
    return should_trigger;
}

// Callback that uses filter
int filtered_callback(const Msg *msg, void *context) {
    test_context_t *ctx = (test_context_t *)context;
    ctx->success_count++;
    
    printf("[Filtered Callback] Triggered for requester 0x%02X (success_count=%d)\n",
           msg->hdr.Requester, ctx->success_count);
    
    return 0;
}

// Helper to create a test message
static Msg create_test_msg(uint8_t requester, bdy_type body_type, bdr_ret return_type) {
    Msg msg = {0};
    msg.hdr.Requester = requester;
    msg.hdr.PriorityRequested = hdr_HIGH;
    msg.hdr.SeqNum = 0x01;
    msg.hdr.Localref = 0x11;
    msg.hdr.BodyLen = 0;
    msg.bdy.BodyType = body_type;
    msg.bdy.ReturnType = return_type;
    msg.msg_buff = NULL;
    return msg;
}

// Test 1: Basic callback registration and triggering
static int test_basic_callback(void) {
    printf("\n=== Test 1: Basic Callback ===\n");
    
    assert(cb_manager_init() == 0);
    
    test_context_t ctx = {0};
    
    callback_config_t config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = simple_callback,
        .filter = NULL,
        .context = &ctx
    };
    
    int callback_id = cb_register(&config);
    assert(callback_id >= 0);
    printf("Registered callback with ID: %d\n", callback_id);
    
    // Create and trigger a matching message
    Msg msg = create_test_msg(0x42, bdy_PING, bdy_CONFIRM);
    
    int triggered = cb_trigger(&msg);
    assert(triggered == 1);
    assert(ctx.callback_count == 1);
    assert(ctx.last_requester == 0x42);
    
    assert(cb_unregister(callback_id) == 0);
    assert(cb_manager_cleanup() == 0);
    
    printf("Test 1 PASSED\n");
    return 1;
}

// Test 2: Multiple callbacks
static int test_multiple_callbacks(void) {
    printf("\n=== Test 2: Multiple Callbacks ===\n");
    
    assert(cb_manager_init() == 0);
    
    test_context_t ctx1 = {0};
    test_context_t ctx2 = {0};
    
    // Register first callback
    callback_config_t config1 = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = simple_callback,
        .filter = NULL,
        .context = &ctx1
    };
    
    int id1 = cb_register(&config1);
    assert(id1 >= 0);
    
    // Register second callback
    callback_config_t config2 = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = simple_callback,
        .filter = NULL,
        .context = &ctx2
    };
    
    int id2 = cb_register(&config2);
    assert(id2 >= 0);
    
    assert(cb_get_count() == 2);
    
    // Trigger - both should execute
    Msg msg = create_test_msg(0x33, bdy_PING, bdy_CONFIRM);
    
    int triggered = cb_trigger(&msg);
    assert(triggered == 2);
    assert(ctx1.callback_count == 1);
    assert(ctx2.callback_count == 1);
    
    assert(cb_unregister(id1) == 0);
    assert(cb_unregister(id2) == 0);
    assert(cb_manager_cleanup() == 0);
    
    printf("Test 2 PASSED\n");
    return 1;
}

// Test 3: Callback with filter
static int test_callback_with_filter(void) {
    printf("\n=== Test 3: Callback with Filter ===\n");
    
    assert(cb_manager_init() == 0);
    
    test_context_t ctx = {0};
    
    callback_config_t config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = filtered_callback,
        .filter = requester_filter,
        .context = &ctx
    };
    
    int callback_id = cb_register(&config);
    assert(callback_id >= 0);
    
    // Trigger with matching requester - should execute
    Msg msg1 = create_test_msg(0x11, bdy_PING, bdy_CONFIRM);
    int triggered1 = cb_trigger(&msg1);
    assert(triggered1 == 1);
    assert(ctx.success_count == 1);
    
    // Trigger with non-matching requester - should not execute
    Msg msg2 = create_test_msg(0x99, bdy_PING, bdy_CONFIRM);
    int triggered2 = cb_trigger(&msg2);
    assert(triggered2 == 0);
    assert(ctx.success_count == 1);  // Still 1
    
    assert(cb_unregister(callback_id) == 0);
    assert(cb_manager_cleanup() == 0);
    
    printf("Test 3 PASSED\n");
    return 1;
}

// Test 4: Non-matching message types
static int test_non_matching_types(void) {
    printf("\n=== Test 4: Non-Matching Message Types ===\n");
    
    assert(cb_manager_init() == 0);
    
    test_context_t ctx = {0};
    
    // Register callback for PING
    callback_config_t config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = simple_callback,
        .filter = NULL,
        .context = &ctx
    };
    
    int callback_id = cb_register(&config);
    assert(callback_id >= 0);
    
    // Trigger with READ message (not PING) - should NOT execute
    Msg msg = create_test_msg(0x42, bdy_READ, bdy_CONFIRM);
    
    int triggered = cb_trigger(&msg);
    assert(triggered == 0);
    assert(ctx.callback_count == 0);
    
    assert(cb_unregister(callback_id) == 0);
    assert(cb_manager_cleanup() == 0);
    
    printf("Test 4 PASSED\n");
    return 1;
}

// Test 5: Query functions
static int test_query_functions(void) {
    printf("\n=== Test 5: Query Functions ===\n");
    
    assert(cb_manager_init() == 0);
    
    // Initially no callbacks
    assert(cb_get_count() == 0);
    assert(cb_has_callbacks_for_type(bdy_PING) == false);
    
    test_context_t ctx = {0};
    
    // Register for PING
    callback_config_t config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = simple_callback,
        .filter = NULL,
        .context = &ctx
    };
    
    int id = cb_register(&config);
    assert(id >= 0);
    
    // After registration
    assert(cb_get_count() == 1);
    assert(cb_has_callbacks_for_type(bdy_PING) == true);
    assert(cb_has_callbacks_for_type(bdy_READ) == false);
    
    // After clearing
    assert(cb_clear_all() == 0);
    assert(cb_get_count() == 0);
    assert(cb_has_callbacks_for_type(bdy_PING) == false);
    
    assert(cb_manager_cleanup() == 0);
    
    printf("Test 5 PASSED\n");
    return 1;
}

// Main test runner - exported for test suite
int test_callbacks(void) {
    printf("\n===== Callback Framework Tests =====\n");
    
    int all_passed = 1;
    
    all_passed &= test_basic_callback();
    all_passed &= test_multiple_callbacks();
    all_passed &= test_callback_with_filter();
    all_passed &= test_non_matching_types();
    all_passed &= test_query_functions();
    
    if (all_passed) {
        printf("\n*** Callback Tests PASSED ***\n");
        return 1;
    } else {
        printf("\n*** Callback Tests FAILED ***\n");
        return 0;
    }
}

#ifdef CALLBACK_STANDALONE
// Main function for standalone execution
int main(void) {
    return test_callbacks() ? 0 : 1;
}
#endif

