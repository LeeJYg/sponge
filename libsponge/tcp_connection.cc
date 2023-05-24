#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    if (_active != true) {
        return;
    }

    _time_since_last_segment_received = 0;

    if (seg.header().rst) {
        _active = false;
        _receiver.stream_out().set_error();
        _sender.stream_in().set_error();
    } else if (_sender.next_seqno_absolute() == 0 && seg.header().syn) {
        _receiver.segment_received(seg);
        connect();
    } else if (_sender.next_seqno_absolute() == _sender.bytes_in_flight() && _receiver.ackno().has_value()) {
        if (!_receiver.stream_out().input_ended()) {
            _sender.ack_received(seg.header().ackno, seg.header().win);
            _receiver.segment_received(seg);
        }
    } else if (_sender.next_seqno_absolute() == _sender.bytes_in_flight() && !_receiver.ackno().has_value()) {
        if (seg.header().syn && seg.header().ack) {
            _sender.ack_received(seg.header().ackno, seg.header().win);
            _sender.send_empty_segment();
            _receiver.segment_received(seg);
            sending_data();
        } else if (seg.header().syn && !seg.header().ack) {
            _sender.send_empty_segment();
            _receiver.segment_received(seg);
            sending_data();
        }
    } else if (_sender.next_seqno_absolute() > _sender.bytes_in_flight() && !_sender.stream_in().eof()) {
        _sender.ack_received(seg.header().ackno, seg.header().win);
        _receiver.segment_received(seg);
        if (seg.length_in_sequence_space() > 0) {
            _sender.send_empty_segment();
        }
        connect();
    } else if (_sender.stream_in().eof() && _sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2 &&
               !_receiver.stream_out().input_ended() && _sender.bytes_in_flight() == 0) {
        _sender.ack_received(seg.header().ackno, seg.header().win);
        _receiver.segment_received(seg);
        _sender.send_empty_segment();
        sending_data();
    } else if (_sender.stream_in().eof() && _sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2 &&
               !_receiver.stream_out().input_ended() && _sender.bytes_in_flight() > 0) {
        if (seg.header().fin) {
            _sender.ack_received(seg.header().ackno, seg.header().win);
            _receiver.segment_received(seg);
            _sender.send_empty_segment();
            sending_data();
        } else if (seg.header().ack) {
            _sender.ack_received(seg.header().ackno, seg.header().win);
            _receiver.segment_received(seg);
            sending_data();
        }
    } else if (_sender.stream_in().eof() && _sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2 &&
               _receiver.stream_out().input_ended() && _sender.bytes_in_flight() == 0) {
        if (seg.header().fin) {
            _sender.ack_received(seg.header().ackno, seg.header().win);
            _receiver.segment_received(seg);
            _sender.send_empty_segment();
            sending_data();
        }
    } else {
        _sender.ack_received(seg.header().ackno, seg.header().win);
        _receiver.segment_received(seg);
        _sender.fill_window();
        sending_data();
    }
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    if (data.empty()) {
        return 0;
    }
    size_t write_byte_size = _sender.stream_in().write(data);
    _sender.fill_window();
    sending_data();
    return write_byte_size;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    if (_active != true) {
        return;
    }

    _time_since_last_segment_received += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);

    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        set_rst_to_cut_connection();
        return;
    }

    sending_data();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    sending_data();
}

void TCPConnection::connect() {
    _sender.fill_window();
    sending_data();
}

void TCPConnection::sending_data() {
    while (_sender.segments_out().empty() != true) {
        TCPSegment flying_seg = _sender.segments_out().front();
        _sender.segments_out().pop();
        if (_receiver.ackno().has_value()) {
            flying_seg.header().ackno = _receiver.ackno().value();
            flying_seg.header().ack = true;
            flying_seg.header().win = _receiver.window_size();
        }
        _segments_out.push(flying_seg);
    }

    if (_receiver.stream_out().input_ended() == true) {
        if (_sender.stream_in().eof() != true) {
            _linger_after_streams_finish = false;
        } else if (_sender.bytes_in_flight() == 0) {
            if (!_linger_after_streams_finish || time_since_last_segment_received() >= 10 * _cfg.rt_timeout) {
                _active = false;
            }
        }
    }
}

void TCPConnection::set_rst_to_cut_connection() {
    TCPSegment S;
    S.header().rst = true;
    _segments_out.push(S);

    _active = false;

    _sender.stream_in().set_error();
    _receiver.stream_out().set_error();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            set_rst_to_cut_connection();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
