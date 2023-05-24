#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _retransmission_timeout(retx_timeout)
    , _stream(capacity) {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    if (_fin == true) {
        return;
    }
    TCPSegment tcp_seg;
    if (_syn != true) {
        _syn = true;
        tcp_seg.header().syn = true;
        segment_sending(tcp_seg);
        return;
    }

    uint16_t wnd_size;
    if (rcv_wnd > 0) {
        wnd_size = rcv_wnd;
    } else {
        wnd_size = 1;
    }

    if (_stream.eof() && _rcvd_seqno + wnd_size > _next_seqno) {
        tcp_seg.header().fin = true;
        _fin = true;
        segment_sending(tcp_seg);
        return;
    }

    while (!_stream.buffer_empty() && _rcvd_seqno + wnd_size > _next_seqno) {
        size_t sending_size =
            min(TCPConfig::MAX_PAYLOAD_SIZE, static_cast<size_t>(wnd_size - (_next_seqno - _rcvd_seqno)));
        tcp_seg.payload() = _stream.read(min(sending_size, _stream.buffer_size()));
        if (_stream.eof() && tcp_seg.length_in_sequence_space() < wnd_size) {
            _fin = true;
            tcp_seg.header().fin = true;
        }

        segment_sending(tcp_seg);
    }
}

void TCPSender::segment_sending(TCPSegment &t) {
    t.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(t);
    _segments_ready.push(t);

    _next_seqno += t.length_in_sequence_space();
    _bytes_in_flight += t.length_in_sequence_space();

    if (!_retransmission_timer_activated) {
        _retransmission_timer_activated = true;
        _retransmission_timer = 0;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t abs_ackno = unwrap(ackno, _isn, _next_seqno);
    if (abs_ackno > _next_seqno) {
        return;
    }
    if (abs_ackno >= _rcvd_seqno) {
        _rcvd_seqno = abs_ackno;
        rcv_wnd = window_size;
    }
    bool isPopped = false;
    while (!_segments_ready.empty()) {
        TCPSegment t = _segments_ready.front();
        uint64_t range = unwrap(t.header().seqno, _isn, _next_seqno) + t.length_in_sequence_space();
        if (abs_ackno < range) {
            return;
        }
        _segments_ready.pop();
        isPopped = true;
        _retransmission_timeout = _initial_retransmission_timeout;
        _retransmission_timer = 0;
        _consecutive_retransmissions = 0;
        _bytes_in_flight -= t.length_in_sequence_space();
    }

    if (isPopped) {
        fill_window();
    }

    if (_segments_ready.empty()) {
        _retransmission_timer_activated = false;
    } else {
        _retransmission_timer_activated = true;
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (_retransmission_timer_activated != true) {
        return;
    }
    _retransmission_timer += ms_since_last_tick;
    if (_retransmission_timer >= _retransmission_timeout && !_segments_ready.empty()) {
        _retransmission_timer = 0;
        TCPSegment t = _segments_ready.front();
        _segments_out.push(t);
        if (rcv_wnd > 0 || t.header().syn) {
            _consecutive_retransmissions += 1;
            _retransmission_timeout *= 2;
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment t;
    t.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(t);
    return;
}
