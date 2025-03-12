#pragma once
#include "wlregistry.hpp"
#include <stdexcept>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

namespace Wayland {
class WlDisplay {
private:
    wl_display* m_display{nullptr};
public:
    explicit WlDisplay(const char* name) {
        auto* display = wl_display_connect(name);
        if (!display) {
            throw std::runtime_error("Display is null");
        }
        m_display = display;
    }
    ~WlDisplay() {
        wl_display_disconnect(m_display);
    }

    WlRegistry get_registry() {
        wl_registry* registry = wl_display_get_registry(m_display);
        if (!registry) {
            throw std::runtime_error("Could not get registry");
        }
        return WlRegistry{registry};
    }

    void roundtrip() {
        wl_display_roundtrip(m_display);
    }
    int dispatch() {
        return wl_display_dispatch(m_display);
    }
};

}
