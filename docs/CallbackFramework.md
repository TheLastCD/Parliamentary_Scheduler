# Callback Framework Documentation

## Overview

The Callback Framework provides a customizable system for handling message responses in the Parliamentary Scheduler. When messages are sent (e.g., PING requests), you can register callbacks that automatically trigger when responses are received.

## Quick Start

### 1. Initialize the Callback Manager

Before using any callbacks, initialize the manager:

```c
#include <callbacks/callback_manager.h>

int main(void) {
    // Initialize callback system
    if (cb_manager_init() != 0) {
        printf("Failed to initialize callback manager\n");
        return 1;
    }
    
    // ... rest of your code ...
    
    // Cleanup when done
    cb_manager_cleanup();
    return 0;
}
```

### 2. Define a Callback Function

Create a callback function with the signature:

```c
int my_callback(const Msg *msg, void *context) {
    printf("Callback triggered!\n");
    printf("  Message Type: %u\n", msg->bdy.BodyType);
    printf("  Response Type: %u\n", msg->bdy.ReturnType);
    printf("  Requester: 0x%02X\n", msg->hdr.Requester);
    
    // Do something with the message
    // ...
    
    return 0;  // Return 0 on success, negative on failure
}
```

### 3. Register the Callback

Register your callback for specific message and response types:

```c
callback_config_t config = {
    .message_type = bdy_PING,           // Match PING messages
    .response_type = bdy_CONFIRM,       // That have CONFIRM response
    .on_response = my_callback,         // Call this function
    .filter = NULL,                     // No filter (optional)
    .context = &my_user_data            // Optional context data
};

int callback_id = cb_register(&config);
if (callback_id < 0) {
    printf("Failed to register callback\n");
    return 1;
}
```

### 4. Trigger Callbacks

When a message is received, call `cb_trigger()` to execute all matching callbacks:

```c
// When processing a received message
Msg received_msg = {0};
if (decode_msg(&received_msg, buffer, buffer_size) > 0) {
    // Trigger all matching callbacks
    int count = cb_trigger(&received_msg);
    printf("Triggered %d callbacks\n", count);
    
    free_msg(&received_msg);
}
```

## Advanced Usage

### Using Filters

Filters allow fine-grained control over when callbacks execute:

```c
// Filter function - only trigger for specific requesters
bool ping_from_server_filter(const Msg *msg, void *context) {
    // Only trigger if the ping came from server (0x42)
    return msg->hdr.Requester == 0x42;
}

callback_config_t config = {
    .message_type = bdy_PING,
    .response_type = bdy_CONFIRM,
    .on_response = my_callback,
    .filter = ping_from_server_filter,  // Use filter
    .context = NULL
};

int callback_id = cb_register(&config);
```

### Using Context Data

Pass arbitrary data to callbacks:

```c
// Your context structure
typedef struct {
    int counter;
    char name[32];
} my_context_t;

// Callback that uses context
int my_context_callback(const Msg *msg, void *context) {
    my_context_t *ctx = (my_context_t *)context;
    ctx->counter++;
    printf("Callback triggered %d times for %s\n", ctx->counter, ctx->name);
    return 0;
}

// Register with context
my_context_t user_data = {.counter = 0, .name = "PingTest"};

callback_config_t config = {
    .message_type = bdy_PING,
    .response_type = bdy_CONFIRM,
    .on_response = my_context_callback,
    .filter = NULL,
    .context = &user_data  // Pass context
};

int callback_id = cb_register(&config);
```

### Multiple Callbacks for Same Type

You can register multiple callbacks for the same message type:

```c
// Register first callback
callback_config_t config1 = {
    .message_type = bdy_PING,
    .response_type = bdy_CONFIRM,
    .on_response = logging_callback,
    .filter = NULL,
    .context = NULL
};
int id1 = cb_register(&config1);

// Register second callback for same type
callback_config_t config2 = {
    .message_type = bdy_PING,
    .response_type = bdy_CONFIRM,
    .on_response = metrics_callback,
    .filter = NULL,
    .context = &metrics_data
};
int id2 = cb_register(&config2);

// Both will be triggered when a matching message arrives
```

### Unregistering Callbacks

Remove callbacks when no longer needed:

```c
// Unregister a specific callback
if (cb_unregister(callback_id) != 0) {
    printf("Failed to unregister callback\n");
}

// Clear all callbacks at once
if (cb_clear_all() != 0) {
    printf("Failed to clear all callbacks\n");
}
```

## Example: Ping Response Monitoring

Complete example of setting up a ping response callback:

```c
#include <callbacks/callback_manager.h>
#include <msg/msg.h>
#include <stdio.h>

typedef struct {
    int ping_count;
    int ping_success_count;
} ping_stats_t;

// Callback to monitor ping responses
int on_ping_response(const Msg *msg, void *context) {
    ping_stats_t *stats = (ping_stats_t *)context;
    
    stats->ping_count++;
    
    if (msg->bdy.ReturnType == bdy_CONFIRM) {
        stats->ping_success_count++;
        printf("Ping confirmed! (%d/%d successful)\n", 
               stats->ping_success_count, 
               stats->ping_count);
    } else {
        printf("Ping failed or ignored\n");
    }
    
    return 0;
}

int main(void) {
    // Initialize
    cb_manager_init();
    
    // Setup statistics
    ping_stats_t stats = {0};
    
    // Register callback
    callback_config_t config = {
        .message_type = bdy_PING,
        .response_type = bdy_CONFIRM,
        .on_response = on_ping_response,
        .filter = NULL,
        .context = &stats
    };
    
    int callback_id = cb_register(&config);
    
    // ... send ping and process responses ...
    // When a ping response arrives:
    // cb_trigger(&received_msg);
    
    // Cleanup
    cb_unregister(callback_id);
    cb_manager_cleanup();
    
    return 0;
}
```

## API Reference

### Initialization

| Function | Purpose |
|----------|---------|
| `cb_manager_init()` | Initialize the callback manager (must be called first) |
| `cb_manager_cleanup()` | Cleanup and free all callbacks |

### Registration

| Function | Purpose |
|----------|---------|
| `cb_register()` | Register a new callback, returns callback ID |
| `cb_unregister()` | Unregister a callback by ID |
| `cb_clear_all()` | Unregister all callbacks |

### Execution

| Function | Purpose |
|----------|---------|
| `cb_trigger()` | Execute all matching callbacks for a message |

### Query

| Function | Purpose |
|----------|---------|
| `cb_get_count()` | Get number of registered callbacks |
| `cb_has_callbacks_for_type()` | Check if callbacks exist for a message type |

## Integration Points

To integrate callbacks into the message processing pipeline:

### In Client Message Processing

```c
// In src/client/client.c, after processing a message:
int process_packet(const Msg *msg) {
    if (!msg) return -1;
    
    printf("Client processing packet...\n");
    
    // Trigger any registered callbacks
    int callbacks_fired = cb_trigger(msg);
    
    // Rest of existing processing...
}
```

### In Server Message Processing

```c
// In src/server/server.c, after handling a message:
int handle_message(const Msg *msg) {
    if (!msg) return -1;
    
    printf("Server handling message...\n");
    
    // Trigger any registered callbacks
    int callbacks_fired = cb_trigger(msg);
    
    // Rest of existing processing...
}
```

## Limitations

- Maximum 64 callbacks can be registered simultaneously
- Callbacks are not persistent (lost on application restart)
- Callbacks execute synchronously (blocking)
- No built-in support for callback priorities

## Best Practices

1. **Always initialize and cleanup** - Call `cb_manager_init()` at startup and `cb_manager_cleanup()` at shutdown

2. **Use meaningful context** - Pass relevant data through the `context` parameter instead of using globals

3. **Check return values** - Always check the return value from `cb_register()` and `cb_trigger()`

4. **Keep callbacks fast** - Callbacks should complete quickly to avoid blocking message processing

5. **Use filters for optimization** - If only certain messages need handling, use filter functions to avoid unnecessary callback execution

6. **Unregister unused callbacks** - If callbacks are temporary, unregister them when done to free up slots

7. **Log callback activity** - Add logging to callbacks for debugging message flow issues
