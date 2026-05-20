#include <callbacks/callback_manager.h>
#include <msg/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_CALLBACKS 64

/* Generic callback manager types and function prototypes. */
typedef struct callback_manager_t callback_manager_t;
typedef int (*generic_callback_fn)(const void *event, void *context);
typedef bool (*generic_callback_filter_fn)(const void *event, void *context);

typedef struct {
    generic_callback_fn callback;
    generic_callback_filter_fn filter;
    void *context;
} generic_callback_config_t;

extern callback_manager_t *cb_manager_create(size_t max_callbacks);
extern int cb_manager_destroy(callback_manager_t *manager);
extern int cb_manager_add_callback(callback_manager_t *manager,
                                  const generic_callback_config_t *config);
extern int cb_manager_remove_callback(callback_manager_t *manager,
                                     int callback_id);
extern int cb_manager_trigger(callback_manager_t *manager,
                              const void *event);
extern size_t cb_manager_get_count(const callback_manager_t *manager);
extern int cb_manager_clear_all(callback_manager_t *manager);

/**
 * @struct callback_entry_t
 * @brief Internal structure mapping project callbacks to generic callbacks
 */
typedef struct {
    bool active;
    bdy_type message_type;
    bdr_ret response_type;
    callback_fn on_response;
    callback_filter_fn filter;
    void *context;
    int backend_id;
} callback_entry_t;

static callback_entry_t registry[MAX_CALLBACKS];
static callback_manager_t *generic_manager = NULL;
static bool initialized = false;

static int find_free_slot(void) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (!registry[i].active) {
            return i;
        }
    }
    return -1;
}

static bool message_filter(const void *event, void *context) {
    if (!event || !context) {
        return false;
    }

    const Msg *msg = (const Msg *)event;
    callback_entry_t *entry = (callback_entry_t *)context;

    if (entry->message_type != msg->bdy.BodyType) {
        return false;
    }

    if (entry->response_type != msg->bdy.ReturnType) {
        return false;
    }

    if (entry->filter) {
        return entry->filter(msg, entry->context);
    }

    return true;
}

static int message_callback(const void *event, void *context) {
    if (!event || !context) {
        return -1;
    }

    const Msg *msg = (const Msg *)event;
    callback_entry_t *entry = (callback_entry_t *)context;
    return entry->on_response(msg, entry->context);
}

int cb_manager_init(void) {
    if (initialized) {
        printf("[Callback Manager] Already initialized\n");
        return 0;
    }

    generic_manager = cb_manager_create(MAX_CALLBACKS);
    if (!generic_manager) {
        return -1;
    }

    memset(registry, 0, sizeof(registry));
    initialized = true;

    printf("[Callback Manager] Initialized with max %d callbacks\n", MAX_CALLBACKS);
    return 0;
}

int cb_manager_cleanup(void) {
    if (!initialized) {
        return -1;
    }

    cb_clear_all();
    if (cb_manager_destroy(generic_manager) != 0) {
        return -1;
    }

    generic_manager = NULL;
    initialized = false;
    memset(registry, 0, sizeof(registry));

    printf("[Callback Manager] Cleaned up\n");
    return 0;
}

int cb_register(const callback_config_t *config) {
    if (!initialized) {
        printf("[Callback Manager] Not initialized\n");
        return -1;
    }

    if (!config || !config->on_response) {
        printf("[Callback Manager] Invalid configuration\n");
        return -1;
    }

    if (cb_get_count() >= MAX_CALLBACKS) {
        printf("[Callback Manager] Callback registry full (%d)\n", MAX_CALLBACKS);
        return -1;
    }

    int slot = find_free_slot();
    if (slot < 0) {
        printf("[Callback Manager] No free slots available\n");
        return -1;
    }

    callback_entry_t *entry = &registry[slot];
    entry->active = true;
    entry->message_type = config->message_type;
    entry->response_type = config->response_type;
    entry->on_response = config->on_response;
    entry->filter = config->filter;
    entry->context = config->context;

    generic_callback_config_t generic_config = {
        .callback = message_callback,
        .filter = message_filter,
        .context = entry
    };

    int backend_id = cb_manager_add_callback(generic_manager, &generic_config);
    if (backend_id < 0) {
        entry->active = false;
        return -1;
    }

    entry->backend_id = backend_id;

    printf("[Callback Manager] Registered callback %d (MsgType:%u, RespType:%u)\n",
           slot, config->message_type, config->response_type);
    return slot;
}

int cb_unregister(int callback_id) {
    if (!initialized) {
        printf("[Callback Manager] Not initialized\n");
        return -1;
    }

    if (callback_id < 0 || callback_id >= MAX_CALLBACKS) {
        printf("[Callback Manager] Invalid callback ID: %d\n", callback_id);
        return -1;
    }

    callback_entry_t *entry = &registry[callback_id];
    if (!entry->active) {
        printf("[Callback Manager] Callback %d not active\n", callback_id);
        return -1;
    }

    if (cb_manager_remove_callback(generic_manager, entry->backend_id) != 0) {
        return -1;
    }

    entry->active = false;
    entry->backend_id = -1;

    printf("[Callback Manager] Unregistered callback %d\n", callback_id);
    return 0;
}

int cb_trigger(const Msg *msg) {
    if (!initialized) {
        printf("[Callback Manager] Not initialized\n");
        return -1;
    }

    if (!msg) {
        printf("[Callback Manager] Invalid message\n");
        return -1;
    }

    return cb_manager_trigger(generic_manager, msg);
}

int cb_get_count(void) {
    if (!initialized) {
        return 0;
    }
    return (int)cb_manager_get_count(generic_manager);
}

int cb_clear_all(void) {
    if (!initialized) {
        printf("[Callback Manager] Not initialized\n");
        return -1;
    }

    int result = cb_manager_clear_all(generic_manager);
    memset(registry, 0, sizeof(registry));
    return result;
}

bool cb_has_callbacks_for_type(bdy_type message_type) {
    if (!initialized) {
        return false;
    }

    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (registry[i].active && registry[i].message_type == message_type) {
            return true;
        }
    }

    return false;
}
