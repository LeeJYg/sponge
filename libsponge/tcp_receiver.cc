#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if(_isn == nullopt && !seg.header().syn){
        return;
    }
    
    if(seg.header().syn){
        _isn = WrappingInt32{seg.header().seqno.raw_value()};
    }

    int64_t abo_seqno = unwrap(seg.header().seqno + static_cast<int>(seg.header().syn), _isn.value(), _checkpoint);

    _reassembler.push_substring(seg.payload().copy(), abo_seqno - 1, seg.header().fin);
    
    _checkpoint += seg.length_in_sequence_space();
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if(_isn == nullopt){
        return nullopt;
    }
    uint64_t written = stream_out().bytes_written () + 1;
    if(stream_out().input_ended()){
        written += 1;
    }
    return wrap(written, _isn.value());
}

size_t TCPReceiver::window_size() const { 
    return _capacity - stream_out().buffer_size();
 }
