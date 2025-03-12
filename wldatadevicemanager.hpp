#pragma once
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "wldatasource.hpp"

namespace Wayland {
class WlDataDeviceManager {
private:
    wl_data_device_manager* m_device_manager{nullptr};
public:
    explicit WlDataDeviceManager(wl_data_device_manager* manager) {
        m_device_manager = manager;
    }
    ~WlDataDeviceManager() {
        wl_data_device_manager_destroy(m_device_manager);
    }

    wl_data_device_manager* get_innter() const { return m_device_manager; }

    WlDataSource create_data_source() {
        wl_data_source* source = wl_data_device_manager_create_data_source(m_device_manager);
        return WlDataSource{source};
    }
};
}
