#include <iostream>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

namespace __detail {
inline void __target_s(void *data,
		       struct wl_data_source *wl_data_source,
		       const char *mime_type) {
}
inline void __send(void *data,
			  struct wl_data_source *wl_data_source) {
	exit(0);
}
}

namespace Wayland {
class WlDataSource {
private:
    wl_data_source* m_data_source;

    wl_data_source_listener data_source_listener {
        .target = __detail::__target_s,
        .send = NULL,
        .cancelled = __detail::__send,
    };
public:
    explicit WlDataSource(wl_data_source* data_source) {
        m_data_source = data_source;
    }
    ~WlDataSource() {
        wl_data_source_destroy(m_data_source);
    }

    void offer(const std::string_view type) {
        wl_data_source_offer(m_data_source, type.data());
    }
    void add_listener(void (*send)(void *data,
		     struct wl_data_source *wl_data_source,
		     const char *mime_type,
		     int32_t fd), void* user_data)
    {
        data_source_listener.send = send;
        wl_data_source_add_listener(m_data_source, &data_source_listener, user_data);
	}

	wl_data_source* get_innter() const { return m_data_source; }
};
}
