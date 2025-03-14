#include "wldatadevice.hpp"
#include "wldisplay.hpp"
#include "wlregistry.hpp"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <variant>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <iostream>
#include "wldatadevicemanager.hpp"
#include "wlseat.hpp"
#include <unistd.h>
#include "utils.hpp"
#include <signal.h>
#include <sys/stat.h>



struct App {
    int tempfile_fd{-1};
    std::string input_buffer;
    bool is_image{false};

    std::unique_ptr<Wayland::WlDataDeviceManager> device_manager;
    std::unique_ptr<Wayland::WlSeat> seat;
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
    if (strcmp(mime_type, "text/plain") && strcmp(mime_type, "text/plain;charset=utf-8") &&
        strcmp(mime_type, "UTF8_STRING") && strcmp(mime_type, "image/png") && strcmp(mime_type, "image/jpeg")) {
        close(fd);
        return;
    }

    App* app = reinterpret_cast<App*>(data);
    std::cout << mime_type << std::endl;

    std::string buffer;
    if (app->tempfile_fd == -1) {
        buffer = app->input_buffer;
    } else {
        std::string str_buf;
        struct stat file_stat{};
        if (fstat(app->tempfile_fd, &file_stat) == -1) {
            std::cerr << "Can't get temp file stats\n";
            return;
        }
        lseek(app->tempfile_fd, 0, SEEK_SET);
        str_buf.reserve(file_stat.st_size);
        char buf[512];

        ssize_t bytes_read;
        while ((bytes_read = read(app->tempfile_fd, buf, 512)) > 0) {
            str_buf.append(buf, bytes_read);
        }
        buffer = std::move(str_buf);
    }



    size_t buf_len = buffer.size();
    size_t total_written{0};
    while (total_written < buf_len) {
        auto to_be_written = buf_len - total_written;
        auto written = write(fd, buffer.c_str() + total_written, to_be_written);
        total_written += written;
    }
    std::cout << buffer << std::endl;

    close(fd);
}



std::variant<int, std::string> get_input_from_stdin(App& app) {
    std::string input_buffer{};
    char temp_buf[128];
    size_t read_bytes;
    while ((read_bytes = read(STDIN_FILENO, temp_buf, 128)) > 0) {
        input_buffer.append(temp_buf, read_bytes);
    }
    Wayland::Utils::trim(input_buffer);

    if (Wayland::Utils::is_jpeg(input_buffer) || Wayland::Utils::is_png(input_buffer)) {
        app.is_image = true;
    }

    if (input_buffer.size() >= 1 * 1024 * 1024) {
        char filename[] = {"/tmp/wl_copy_XXXXXX"};
        int tempfile_fd = mkstemp(filename);
        write(tempfile_fd, input_buffer.c_str(), input_buffer.length());
        unlink(filename);
        return tempfile_fd;
    }
    return input_buffer;
}

template<class... Ts>
struct overload : Ts... { using Ts::operator()...; };
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    App app{};

    auto user_input = get_input_from_stdin(app);


    std::visit(overload{
        [&app](int fd) {
            app.tempfile_fd = fd;
        },
        [&app](std::string &input) {
            app.input_buffer = std::move(input);
        }
    }, user_input);


    Wayland::WlDisplay display{NULL};
    Wayland::WlRegistry registry = display.get_registry();
    registry.add_listener(registry_handler_global, registry_handler_global_remove, &app);
    display.roundtrip();

    Wayland::WlDataDevice data_device{wl_data_device_manager_get_data_device(app.device_manager->get_innter(), app.seat->get_innter_seat())};

    Wayland::WlDataSource data_source = app.device_manager->create_data_source();
    data_source.offer("text/plain");
    data_source.offer("text/plain;charset=utf-8");
    data_source.offer("UTF8_STRING");

    if (app.is_image) {
        data_source.offer("image/jpeg");
        data_source.offer("image/png");
    }


    data_source.add_listener(data_source_send, &app);

    data_device.set_selection(data_source, 0);

    // int i = fork();
    // if (i < 0) {
    //     throw std::runtime_error("Failed to fork");
    // }
    // if (i > 0) {
    //     exit(0);
    // }
    while (display.dispatch() > 0) {
    }

    if (app.tempfile_fd != -1) {
        close(app.tempfile_fd);
    }
    return 0;
}
