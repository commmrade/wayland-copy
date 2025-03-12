#pragma once
#include <functional>
#include <wayland-client-protocol.h>
#include <wayland-client.h>



namespace Wayland {
class WlRegistry {
private:
    wl_registry_listener registry_listener {
        .global = NULL,
        .global_remove = NULL,
    };

    wl_registry* m_registry{nullptr};
public:
    explicit WlRegistry(wl_registry* registry) {
        m_registry = registry;
    }
    ~WlRegistry() {
        wl_registry_destroy(m_registry);
    }

    void add_listener(void (*global)(void *data,
	    struct wl_registry *wl_registry,
	    uint32_t name,
	    const char *interface,
	    uint32_t version),
        void (*global_remove)(void *data,
	    struct wl_registry *wl_registry,
		uint32_t name), void* user_data)
    {
        registry_listener.global = global;
        registry_listener.global_remove = global_remove;
        wl_registry_add_listener(m_registry, &registry_listener, user_data);
    }

};
}
