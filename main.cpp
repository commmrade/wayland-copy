#include "wldatadevice.hpp"
#include "wldisplay.hpp"
#include "wlregistry.hpp"
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <iostream>
#include "wldatadevicemanager.hpp"
#include "wlseat.hpp"
#include <unistd.h>
#include "utils.hpp"
#include <signal.h>

struct App {
    std::unique_ptr<Wayland::WlDataDeviceManager> device_manager;
    std::unique_ptr<Wayland::WlSeat> seat;
    std::string input_buffer;
};


void registry_handler_global(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version) {
    App* app = reinterpret_cast<App*>(data);
    if (!strcmp(interface, wl_data_device_manager_interface.name)) {
        wl_data_device_manager* manager = reinterpret_cast<wl_data_device_manager*>(wl_registry_bind(wl_registry, name, &wl_data_device_manager_interface, version));
        app->device_manager = std::make_unique<Wayland::WlDataDeviceManager>(manager);
    } else if (!strcmp(interface, wl_seat_interface.name)) {
        wl_seat* seat = reinterpret_cast<wl_seat*>(wl_registry_bind(wl_registry, name, &wl_seat_interface, version));
        app->seat = std::make_unique<Wayland::WlSeat>(seat);
    }
}
void registry_handler_global_remove(void *data, struct wl_registry *wl_registry, uint32_t name) {
}

void data_source_send(void* data, struct wl_data_source* data_source, const char* mime_type, int32_t fd) {
    App* app = reinterpret_cast<App*>(data);
    size_t buf_len = app->input_buffer.size();
    size_t total_written{0};

    while (total_written < buf_len) {
        auto to_be_written = buf_len - total_written;
        auto written = write(fd, app->input_buffer.c_str() + total_written, to_be_written);
        total_written += written;
    }
    close(fd);
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    App app{};

    // Reading from stdin
    std::string input_buffer{};
    char temp_buf[128];
    size_t read_bytes;
    while ((read_bytes = read(STDIN_FILENO, temp_buf, 128)) > 0) {
        input_buffer.append(temp_buf, read_bytes);
    }
    trim(input_buffer);
    app.input_buffer = std::move(input_buffer);

    Wayland::WlDisplay display{NULL};
    Wayland::WlRegistry registry = display.get_registry();
    registry.add_listener(registry_handler_global, registry_handler_global_remove, &app);
    display.roundtrip();

    Wayland::WlDataDevice data_device{wl_data_device_manager_get_data_device(app.device_manager->get_innter(), app.seat->get_innter_seat())};

    Wayland::WlDataSource data_source = app.device_manager->create_data_source();
    data_source.offer("text/plain");
    data_source.offer("text/plain;charset=utf-8");
    data_source.offer("UTF8_STRING");
    data_source.add_listener(data_source_send, &app);

    data_device.set_selection(data_source, 0);

    int i = fork();
    if (i < 0) {
        throw std::runtime_error("Failed to fork");
    }
    if (i > 0) {
        _exit(0);
    }
    if (i == 0) std::cout << "hi\n";
    while (display.dispatch() > 0) {
    }

    return 0;
}
