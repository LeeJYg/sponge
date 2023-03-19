#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { 
    _buffer = "";
    _capacity = capacity; 
    _flag_InputEnd = false;
    _byte_Written = 0;
    _byte_Popped = 0;
    _error = false;
    }

size_t ByteStream::write(const string &data) {
    size_t NumberofByteAccepted = data.length();
    if(NumberofByteAccepted > remaining_capacity()){
        NumberofByteAccepted = remaining_capacity();
    }
    _byte_Written += NumberofByteAccepted;
    string ActualWrittenData = data.substr(0, NumberofByteAccepted);
    _buffer += ActualWrittenData;
    return NumberofByteAccepted;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t PeekLength = len;
    if(len > buffer_size()){
        PeekLength = buffer_size();
    }
    return _buffer.substr(0, PeekLength);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t PopLength;
    if(buffer_size() < len){
        PopLength = buffer_size();
    }
    else{
        PopLength = len;
    }
    _byte_Popped += PopLength;
    _buffer = _buffer.substr(PopLength, _buffer.length());

    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    size_t ActuralRead = (len > buffer_size()) ? buffer_size() : len;
    string ret = peek_output(ActuralRead);
    pop_output(ActuralRead);
    return ret;
}

void ByteStream::end_input() { _flag_InputEnd = true;  return; }

bool ByteStream::input_ended() const { return _flag_InputEnd; }

size_t ByteStream::buffer_size() const { return _buffer.length(); }

bool ByteStream::buffer_empty() const { return buffer_size() == 0; }

bool ByteStream::eof() const { return (buffer_empty() == true) && (input_ended() == true); }

size_t ByteStream::bytes_written() const { return _byte_Written; }

size_t ByteStream::bytes_read() const { return _byte_Popped; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
