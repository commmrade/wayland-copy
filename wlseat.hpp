#pragma once
#include <wayland-client-protocol.h>
#include <wayland-client.h>


namespace Wayland {
class WlSeat {
private:
    wl_seat* m_seat{nullptr};
public:
    explicit WlSeat(wl_seat* seat) {
        m_seat = seat;
    }
    ~WlSeat() {
        wl_seat_destroy(m_seat);
    }

    wl_seat* get_innter_seat() const { return m_seat; }
};
}
