#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <client/client.h>
#include <server/server.h>
#include <msg/msg.h>
#include <protocols/internal/ping.h>

// Test configuration
#define TEST_PORT 54321
#define TEST_SERVER_ID 0x42
#define TEST_CLIENT_ID 0x11

// Shared state for test coordination
typedef struct {
    pthread_barrier_t barrier;
    volatile int client_received;
    volatile int server_received;
    uint8_t received_packet[256];
    size_t received_size;
} test_state_t;

// Helper function to create a test packet
// Returns packet size on success, 0 on failure
// Packet format: 5-byte header + 2-byte body header + payload
static size_t make_test_packet(
    uint8_t requester,
    uint8_t priority,
    uint8_t seq,
    uint8_t localref,
    uint8_t body_type,
    uint8_t return_type,
    const uint8_t *payload,
    size_t payload_len,
    uint8_t *buffer,
    size_t buf_len)
{
    Msg msg = {0};
    msg.hdr.Requester = requester;
    msg.hdr.PriorityRequested = priority;
    msg.hdr.SeqNum = seq;
    msg.hdr.Localref = localref;
    msg.hdr.BodyLen = (uint8_t)payload_len;
    msg.bdy.BodyType = body_type;
    msg.bdy.ReturnType = return_type;

    if (payload_len > 0) {
        uint8_t *msg_buff = malloc(payload_len);
        if (!msg_buff) return 0;
        memcpy(msg_buff, payload, payload_len);
        msg.msg_buff = msg_buff;
    }

    int result = encode_msg(&msg, buffer, buf_len);
    
    if (msg.msg_buff) {
        free(msg.msg_buff);
    }

    // Packet size: 5 bytes header + 2 bytes body + payload_len
    return result > 0 ? (5 + 2 + payload_len) : 0;
}

// Server thread function
static void *server_thread(void *arg) {
    test_state_t *state = (test_state_t *)arg;
    
    // Initialize and start server
    srvContext srv_ctx;
    srvSock srv_config = {
        .ServerMetaTag = {
            .ServerRole = srv_SEC,
            .ServerCommAdv = 0x42,
        },
        .ServerPort = TEST_PORT,
    };
    
    assert(srv_InitServer(&srv_ctx, &srv_config) == 0);
    srv_SetGlobalContext(&srv_ctx);
    assert(srv_StartListen() == 0);
    
    printf("[Server] Started listening on port %d\n", TEST_PORT);
    
    // Signal ready
    pthread_barrier_wait(&state->barrier);
    
    // Accept connection
    int client_socket = srv_AcceptConnection(srv_ctx.listen_fd);
    if (client_socket < 0) {
        printf("[Server] Accept failed\n");
        return NULL;
    }
    printf("[Server] Accepted client connection\n");
    
    // Receive packet
    uint8_t buffer[256] = {0};
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    
    if (bytes_read > 0) {
        printf("[Server] Received %zd bytes\n", bytes_read);
        
        // Decode and verify
        Msg received_msg = {0};
        if (decode_msg(&received_msg, buffer, bytes_read) > 0) {
            printf("[Server] Successfully decoded message\n");
            printf("[Server] Requester: 0x%02X, Seq: %u, BodyType: %u\n",
                   received_msg.hdr.Requester,
                   received_msg.hdr.SeqNum,
                   received_msg.bdy.BodyType);
            
            state->server_received = 1;
            memcpy(state->received_packet, buffer, bytes_read);
            state->received_size = bytes_read;

            // If this was a PING, respond with a PING CONFIRM and payload [seq, 0x01]
            if (received_msg.bdy.BodyType == bdy_PING) {
                // build response message
                Msg resp = {0};
                resp.hdr.Requester = TEST_SERVER_ID; // server id as requester
                resp.hdr.PriorityRequested = hdr_HIGH;
                resp.hdr.SeqNum = received_msg.hdr.SeqNum; // echo seq
                resp.hdr.Localref = received_msg.hdr.Localref;
                uint8_t tmp[8] = {0};
                size_t resp_len = 0;
                if (ping_build_confirm(received_msg.hdr.SeqNum, tmp, sizeof(tmp), &resp_len) == 0) {
                    resp.hdr.BodyLen = (uint8_t)resp_len;
                    resp.bdy.BodyType = bdy_PING;
                    resp.bdy.ReturnType = bdy_CONFIRM;
                    resp.msg_buff = malloc(resp_len);
                    if (resp.msg_buff) memcpy(resp.msg_buff, tmp, resp_len);

                    uint8_t outbuf[32] = {0};
                    int enc = encode_msg(&resp, outbuf, sizeof(outbuf));
                    if (enc > 0) {
                        send(client_socket, outbuf, 5 + 2 + resp_len, 0);
                    }

                    if (resp.msg_buff) free(resp.msg_buff);
                }
            }
        } else {
            printf("[Server] Failed to decode message\n");
        }
        
        free_msg(&received_msg);
    } else {
        printf("[Server] Receive failed\n");
    }
    
    close(client_socket);
    srv_StopListen();
    
    return NULL;
}

// Client thread function
static void *client_thread(void *arg) {
    test_state_t *state = (test_state_t *)arg;
    
    // Initialize client
    cltMeta client_meta = {
        .cltRole = clt_MEMBER,
        .serverMeta = {
            .ServerRole = srv_SEC,
            .ServerCommAdv = 0x42,
        },
    };
    
    assert(start_client(&client_meta, TEST_CLIENT_ID) == 0);
    printf("[Client] Client initialized with ID 0x%02X\n", TEST_CLIENT_ID);
    
    // Wait for server to be ready
    pthread_barrier_wait(&state->barrier);
    
    // Small delay to ensure server is listening
    usleep(100000);
    
    // Connect to server
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("[Client] Socket creation failed");
        return NULL;
    }
    
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(TEST_PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };
    
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Client] Connection failed");
        close(socket_fd);
        return NULL;
    }
    printf("[Client] Connected to server at 127.0.0.1:%d\n", TEST_PORT);
    
    // Create and send test packet
    uint8_t test_payload[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t packet[256];
    
    size_t packet_size = make_test_packet(
        TEST_CLIENT_ID,           // requester
        hdr_HIGH,                 // priority
        0x05,                     // sequence number
        TEST_CLIENT_ID,           // localref
        bdy_READ,                 // body type
        bdy_CONFIRM,              // return type
        test_payload,
        sizeof(test_payload),
        packet,
        sizeof(packet)
    );
    
    if (packet_size > 0) {
        ssize_t bytes_sent = send(socket_fd, packet, packet_size, 0);
        if (bytes_sent > 0) {
            printf("[Client] Sent %zd bytes\n", bytes_sent);

            // Wait for server response (ping confirm)
            uint8_t respbuf[256] = {0};
            ssize_t resp_len = recv(socket_fd, respbuf, sizeof(respbuf), 0);
            if (resp_len > 0) {
                Msg decoded = {0};
                if (decode_msg(&decoded, respbuf, resp_len) > 0) {
                    if (decoded.bdy.BodyType == bdy_PING && decoded.bdy.ReturnType == bdy_CONFIRM && decoded.hdr.BodyLen >= 2) {
                        // check payload: first byte == seq, second byte == 0x01
                        if (decoded.msg_buff && decoded.msg_buff[0] == 0x05 && decoded.msg_buff[1] == 0x01) {
                            printf("[Client] Received expected ping response (seq=0x05, status=0x01)\n");
                            state->client_received = 1;
                        }
                    }
                    free_msg(&decoded);
                }
            }
        } else {
            printf("[Client] Send failed\n");
        }
    } else {
        printf("[Client] Failed to create packet\n");
    }
    
    close(socket_fd);
    kill_client();
    
    return NULL;
}

// Integration test: server and client communication
int test_client_server_integration(void) {
    printf("\n=== Integration Test: Client-Server Communication ===\n");
    
    test_state_t state = {0};
    pthread_barrier_init(&state.barrier, NULL, 2);
    
    // Create threads
    pthread_t srv_tid, clt_tid;
    
    assert(pthread_create(&srv_tid, NULL, server_thread, &state) == 0);
    assert(pthread_create(&clt_tid, NULL, client_thread, &state) == 0);
    
    // Wait for both threads to complete
    pthread_join(srv_tid, NULL);
    pthread_join(clt_tid, NULL);
    
    pthread_barrier_destroy(&state.barrier);
    
    // Verify both sides succeeded
    printf("\n=== Test Results ===\n");
    printf("Client sent successfully: %s\n", state.client_received ? "YES" : "NO");
    printf("Server received successfully: %s\n", state.server_received ? "YES" : "NO");
    
    // Verify packet content
    if (state.server_received && state.received_size > 0) {
        Msg decoded_msg = {0};
        if (decode_msg(&decoded_msg, state.received_packet, state.received_size) > 0) {
            printf("Packet verification:\n");
            printf("  Requester: 0x%02X (expected 0x%02X) - %s\n",
                   decoded_msg.hdr.Requester, TEST_CLIENT_ID,
                   decoded_msg.hdr.Requester == TEST_CLIENT_ID ? "OK" : "FAIL");
            printf("  SeqNum: 0x%02X (expected 0x05) - %s\n",
                   decoded_msg.hdr.SeqNum,
                   decoded_msg.hdr.SeqNum == 0x05 ? "OK" : "FAIL");
            printf("  BodyType: %u (expected %u) - %s\n",
                   decoded_msg.bdy.BodyType, bdy_READ,
                   decoded_msg.bdy.BodyType == bdy_READ ? "OK" : "FAIL");
            printf("  ReturnType: %u (expected %u) - %s\n",
                   decoded_msg.bdy.ReturnType, bdy_CONFIRM,
                   decoded_msg.bdy.ReturnType == bdy_CONFIRM ? "OK" : "FAIL");
            
            // Verify all fields
            assert(state.client_received == 1);
            assert(state.server_received == 1);
            assert(decoded_msg.hdr.Requester == TEST_CLIENT_ID);
            assert(decoded_msg.hdr.SeqNum == 0x05);
            assert(decoded_msg.bdy.BodyType == bdy_READ);
            assert(decoded_msg.bdy.ReturnType == bdy_CONFIRM);
            
            free_msg(&decoded_msg);
            
            printf("\nAll assertions passed!\n");
            return 1;
        } else {
            printf("Failed to decode received message\n");
        }
    }
    
    printf("Verification failed - packet not received or not decodable\n");
    return 0;
}

// Main test runner
int main(void) {
    printf("Parliamentary Scheduler - Integration Tests\n");
    
    int result = test_client_server_integration();
    
    if (result) {
        printf("\n*** Integration Test PASSED ***\n");
        return 0;
    } else {
        printf("\n*** Integration Test FAILED ***\n");
        return 1;
    }
}
