/**
 * @file callback_example.c
 * @brief Example: Using callbacks with PING messages
 * 
 * This example demonstrates a practical use case of the callback framework:
 * - Sending PING messages to a server
 * - Triggering callbacks when PING responses are received
 * - Tracking statistics about ping success rates
 */

#include <stdio.h>
#include <string.h>
#include <callbacks/callback_manager.h>
#include <msg/msg.h>
#include <protocols/internal/ping.h>

/**
 * Application context struct
 */
typedef struct {
    int total_pings_sent;
    int successful_responses;
    int failed_responses;
    uint8_t last_responder_id;
} ping_monitor_t;

/**
 * Callback function: Log successful ping response
 * 
 * This callback is invoked when a PING message receives a CONFIRM response
 */
int on_ping_confirmed(const Msg *msg, void *context) {
    ping_monitor_t *monitor = (ping_monitor_t *)context;
    
    monitor->successful_responses++;
    monitor->last_responder_id = msg->hdr.Requester;
    
    printf("[PING Monitor] Success! Responder: 0x%02X (Seq: %u)\n",
           msg->hdr.Requester, msg->hdr.SeqNum);
    printf("               Success Rate: %d/%d (%.0f%%)\n",
           monitor->successful_responses,
           monitor->total_pings_sent,
           (100.0 * monitor->successful_responses) / monitor->total_pings_sent);
    
    return 0;
}

/**
 * Callback function: Log failed ping response
 */
int on_ping_failed(const Msg *msg, void *context) {
    ping_monitor_t *monitor = (ping_monitor_t *)context;
    
    monitor->failed_responses++;
    
    printf("[PING Monitor] Failed! Responder: 0x%02X (Response: %u)\n",
           msg->hdr.Requester, msg->bdy.ReturnType);
    printf("               Success Rate: %d/%d (%.0f%%)\n",
           monitor->successful_responses,
           monitor->total_pings_sent,
           (100.0 * monitor->successful_responses) / monitor->total_pings_sent);
    
    return 0;
}

/**
 * Filter function: Only monitor responses from server (0x42)
 */
bool is_server_response(const Msg *msg, void *context) {
    return msg->hdr.Requester == 0x42;
}

/**
 * Example main function
 */
int example_ping_monitoring(void) {
    printf("\n========== Callback Example: PING Monitoring ==========\n\n");
    
    // Initialize callback manager
    if (cb_manager_init() != 0) {
        printf("Failed to initialize callback manager\n");
        return 1;
    }
    
    // Create monitor instance
    ping_monitor_t monitor = {
        .total_pings_sent = 0,
        .successful_responses = 0,
        .failed_responses = 0,
        .last_responder_id = 0
    };
    
    // Register callback for successful PING responses
    callback_config_t success_config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = on_ping_confirmed,
        .filter = is_server_response,  // Only track server responses
        .context = &monitor
    };
    int success_callback_id = cb_register(&success_config);
    if (success_callback_id < 0) {
        printf("Failed to register success callback\n");
        cb_manager_cleanup();
        return 1;
    }
    printf("Registered success callback (ID: %d)\n", success_callback_id);
    
    // Register callback for failed PING responses
    callback_config_t fail_config = {
        .message_type = bdy_PING,
        .response_type = bdy_MSG,        // Different response type = failure
        .on_response = on_ping_failed,
        .filter = is_server_response,
        .context = &monitor
    };
    int fail_callback_id = cb_register(&fail_config);
    if (fail_callback_id < 0) {
        printf("Failed to register failure callback\n");
        cb_unregister(success_callback_id);
        cb_manager_cleanup();
        return 1;
    }
    printf("Registered failure callback (ID: %d)\n\n", fail_callback_id);
    
    // Simulate receiving various PING responses
    printf("--- Simulating PING responses ---\n\n");
    
    // Create test scenarios
    Msg test_msgs[] = {
        // Simulated response 1: Server responds with CONFIRM
        {
            .hdr = {.Requester = 0x42, .SeqNum = 0x01, .Localref = 0x11, .BodyLen = 0},
            .bdy = {.BodyType = bdy_PING, .ReturnType = bdy_CONFIRM},
            .msg_buff = NULL
        },
        // Simulated response 2: Server responds with CONFIRM
        {
            .hdr = {.Requester = 0x42, .SeqNum = 0x02, .Localref = 0x11, .BodyLen = 0},
            .bdy = {.BodyType = bdy_PING, .ReturnType = bdy_CONFIRM},
            .msg_buff = NULL
        },
        // Simulated response 3: Server responds with MSG (failure)
        {
            .hdr = {.Requester = 0x42, .SeqNum = 0x03, .Localref = 0x11, .BodyLen = 0},
            .bdy = {.BodyType = bdy_PING, .ReturnType = bdy_MSG},
            .msg_buff = NULL
        },
        // Simulated response 4: Client (0x11) responds (should be ignored by filter)
        {
            .hdr = {.Requester = 0x11, .SeqNum = 0x04, .Localref = 0x11, .BodyLen = 0},
            .bdy = {.BodyType = bdy_PING, .ReturnType = bdy_CONFIRM},
            .msg_buff = NULL
        },
        // Simulated response 5: Server responds with CONFIRM
        {
            .hdr = {.Requester = 0x42, .SeqNum = 0x05, .Localref = 0x11, .BodyLen = 0},
            .bdy = {.BodyType = bdy_PING, .ReturnType = bdy_CONFIRM},
            .msg_buff = NULL
        },
    };
    
    // Process each message
    for (int i = 0; i < 5; i++) {
        monitor.total_pings_sent++;
        printf("[Message %d] Processing...\n", i + 1);
        
        int triggered = cb_trigger(&test_msgs[i]);
        if (triggered > 0) {
            printf("           %d callback(s) executed\n", triggered);
        } else {
            printf("           No callbacks triggered (filtered or no match)\n");
        }
        printf("\n");
    }
    
    // Final statistics
    printf("--- Final Statistics ---\n");
    printf("Total PINGs sent:         %d\n", monitor.total_pings_sent);
    printf("Successful responses:     %d\n", monitor.successful_responses);
    printf("Failed responses:         %d\n", monitor.failed_responses);
    if (monitor.total_pings_sent > 0) {
        printf("Success rate:             %.0f%%\n",
               (100.0 * monitor.successful_responses) / monitor.total_pings_sent);
    }
    printf("Last responder ID:        0x%02X\n\n", monitor.last_responder_id);
    
    // Cleanup
    cb_unregister(success_callback_id);
    cb_unregister(fail_callback_id);
    cb_manager_cleanup();
    
    printf("=========================================\n\n");
    return 0;
}

// Uncomment to run as standalone program
// int main(void) {
//     return example_ping_monitoring();
// }
