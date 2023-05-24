#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();

    EthernetFrame ef;

    if (_addr_table.count(next_hop_ip)) {
        ef.header().type = EthernetHeader::TYPE_IPv4;
        ef.header().src = _ethernet_address;
        ef.header().dst = _addr_table[next_hop_ip];
        ef.payload() = dgram.serialize();
    } else if (_ARP_status.count(next_hop_ip) == 0) {
        ARPMessage amsg;
        amsg.sender_ethernet_address = _ethernet_address;
        amsg.sender_ip_address = _ip_address.ipv4_numeric();
        amsg.target_ip_address = next_hop_ip;
        amsg.target_ethernet_address = EthernetAddress{0};
        amsg.opcode = ARPMessage::OPCODE_REQUEST;

        ef.header().type = EthernetHeader::TYPE_ARP;
        ef.header().src = _ethernet_address;
        ef.header().dst = EthernetAddress{ETHERNET_BROADCAST};
        ef.payload() = amsg.serialize();

        _waiting_dgram.push_back(make_pair(next_hop, dgram));
        _ARP_status[next_hop_ip] = -1;
    } else if (_ARP_status.count(next_hop_ip) && (_ARP_status[next_hop_ip] < 0)) {
        _waiting_dgram.push_back(make_pair(next_hop, dgram));
        return;
    }

    _frames_out.push(ef);
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    EthernetHeader ehdr = frame.header();

    if (ehdr.type == EthernetHeader::TYPE_IPv4 && frame.header().dst == _ethernet_address) {
        InternetDatagram dataGgram;
        auto parsing = dataGgram.parse(frame.payload());
        if (parsing == ParseResult::NoError) {
            return dataGgram;
        }
    }

    if (ehdr.type == EthernetHeader::TYPE_ARP) {
        ARPMessage amsg;
        auto parsing = amsg.parse(frame.payload());
        if (parsing == ParseResult::NoError) {
            _addr_table[amsg.sender_ip_address] = amsg.sender_ethernet_address;
            _ARP_status[amsg.sender_ip_address] = 0;
        }
        if (amsg.opcode == ARPMessage::OPCODE_REQUEST && amsg.target_ip_address == _ip_address.ipv4_numeric()) {
            amsg.target_ethernet_address = amsg.sender_ethernet_address;
            amsg.target_ip_address = amsg.sender_ip_address;
            amsg.sender_ip_address = _ip_address.ipv4_numeric();
            amsg.sender_ethernet_address = _ethernet_address;
            amsg.opcode = ARPMessage::OPCODE_REPLY;

            EthernetFrame replyf;
            replyf.payload() = amsg.serialize();
            replyf.header().type = EthernetHeader::TYPE_ARP;
            replyf.header().src = _ethernet_address;
            replyf.header().dst = amsg.target_ethernet_address;

            _frames_out.push(replyf);
        }
    }

    waiting_dgram_check();
    return {};
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    for (auto i : _ARP_status) {
        if (_ARP_status.empty()) {
            break;
        }
        if (_ARP_status[i.first] >= 0) {
            _ARP_status[i.first] += ms_since_last_tick;
            if (_ARP_status[i.first] >= 30000) {
                _ARP_status.erase(i.first);
                _addr_table.erase(i.first);
            }
        } else {
            _ARP_status[i.first] -= ms_since_last_tick;
            if (_ARP_status[i.first] <= -5001) {
                _ARP_status.erase(i.first);
            }
        }
    }
    waiting_dgram_check();
}

void NetworkInterface::waiting_dgram_check() {
    int idx = 0;
    for (auto i : _waiting_dgram) {
        if (_ARP_status.count(i.first.ipv4_numeric()) && (_ARP_status[i.first.ipv4_numeric()] >= 0)) {
            send_datagram(i.second, i.first);
            _waiting_dgram.erase(_waiting_dgram.begin() + idx);
        }
        idx++;
    }
}