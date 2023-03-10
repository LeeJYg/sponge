#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
: _output(capacity), _capacity(capacity)
, write_index(capacity)
, unassembled_data(0)
, Data_NotStored(capacity, '\0')
, first_unassembled_index(0)
, is_end(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t end_of_capacity = first_unassembled_index + _output.remaining_capacity();
    size_t LastIndexofData = index + data.length();

    if (LastIndexofData > Data_NotStored.size()) {
        Data_NotStored.resize(LastIndexofData);
    }

    if(index >= end_of_capacity){
        return;//out of capacity => discarded
    }
    
    if(LastIndexofData <= end_of_capacity){//can be written in ByteStream
        if(eof){//input data is the last bytes of the entire stream
            is_end = true;
        }
    }
    
    if(LastIndexofData > first_unassembled_index){//unassembled_data case
        for(size_t i = max(first_unassembled_index, index); i < min(end_of_capacity, LastIndexofData); i++){
            if(write_index.find(i) == write_index.end()){
                write_index.insert(i);
                Data_NotStored[i] = data[i - index];
                unassembled_data++;
            }
        }
        
        while (write_index.count(first_unassembled_index) > 0){//assemble data
            _output.write(Data_NotStored.substr(first_unassembled_index, 1));
            write_index.erase(first_unassembled_index);
            unassembled_data--;
            first_unassembled_index++;
        }
    }

    if(is_end == true && unassembled_data == 0){//write end, call end_input.
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_data; }

bool StreamReassembler::empty() const { return unassembled_data == 0; }
