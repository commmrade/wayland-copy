#pragma once
#include "wldatadevicemanager.hpp"
#include <iostream>
#include <wayland-client-protocol.h>
#include <wayland-client.h>



namespace Wayland {
class WlDataDevice {
private:
    wl_data_device* m_data_device{nullptr};

public:
    explicit WlDataDevice(wl_data_device* data_device) {
        m_data_device = data_device;
    }
    ~WlDataDevice() {
        wl_data_device_destroy(m_data_device);
    }

    void set_selection(Wayland::WlDataSource& data_source, int serial = 0) {
        wl_data_device_set_selection(m_data_device, data_source.get_innter(), serial);
    }
};
}
