#pragma once
#include <memory>
#include "wldatadevicemanager.hpp"
#include "wlseat.hpp"

struct App {
    int tempfile_fd{-1};
    std::string input_buffer;
    bool is_image{false};

    std::unique_ptr<Wayland::WlDataDeviceManager> device_manager;
    std::unique_ptr<Wayland::WlSeat> seat;

    // Options
    bool foreground{false};
    bool paste_once{false};
};
