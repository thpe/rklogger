/*
** Copyright 2012 Thomas Petig
**
** This file is part of rklogger.
**
** rklogger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** rklogger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with rklogger.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "common.H"
#include "serialport.H"

namespace RK {
namespace ComLynx {

class Protocol {
    public:
    Protocol (SerialPort& port_,
              uint8_t host_address_ = 0x01,
              uint8_t host_network_ = 0x00) :
        _port(port_),
        _host_address(host_address_),
        _host_network(host_network_)
    {
    }

    bool ping             (uint8_t network, uint8_t address);
    bool node_information (uint8_t network, uint8_t address);
    bool embedded_can     (uint8_t network, uint8_t address);
    bool embedded_can     (uint8_t network, uint8_t address,
                           uint8_t index,   uint8_t subindex);
    bool embedded_can     (uint8_t network, uint8_t address,
                           uint8_t index,   uint8_t subindex,
                           uint8_t& format, uint32_t& val);
    private:
    typedef struct frame_start_mask {
        unsigned int hdlc_delimiter : 8;
        unsigned int hdlc_address : 8;
        unsigned int hdlc_control : 8;
        unsigned int src_network : 8;
        unsigned int src_address : 8;
        unsigned int dest_network : 8;
        unsigned int dest_address : 8;
        unsigned int size : 8;
        unsigned int type : 8;
    } frame_start_mask;

    typedef struct frame_end_mask {
        unsigned int fcs_lsb        : 8;
        unsigned int fcs_msb        : 8;
        unsigned int hdlc_delimiter : 8;
    } frame_end_mask;

    typedef struct node_info_data_mask {
        char product_number[12];
        char serial_number [12];
        char network;
        char subnet;
        char address;
        char device_type_id;
        char device_type_subid;
    } node_info_data_mask;

    typedef struct embedded_can_data_mask {
        unsigned int doc_no        : 8;
        unsigned int dest_mod_id   : 4;
        unsigned int unused        : 4;
        unsigned int page_no       : 4;
        unsigned int src_mod_id    : 4;
        unsigned int param_idx     : 8;
        unsigned int param_sub_idx : 8;
        unsigned int data_type     : 4;
        unsigned int flags         : 4;
        unsigned int param_val     : 32;
    } __attribute__ ((__packed__))  embedded_can_data_mask;

    void     init_msg      (uint8_t* msg, uint8_t network, uint8_t address, uint8_t size, uint8_t type);
    uint8_t* get_msg_data  (uint8_t *msg);
    bool     get_msg_frame (uint8_t* msg, size_t length, uint8_t*& first_frame);
    void     fcs           (uint8_t* msg, size_t length);
    size_t   stuff         (uint8_t* msg, uint8_t* buf, size_t length);
    size_t   unstuff       (uint8_t* msg, uint8_t* buf, size_t length);
    frame_start_mask* frame_start_mask_ptr (uint8_t* msg);
    frame_end_mask*   frame_end_mask_ptr   (uint8_t* msg);

    SerialPort& _port;
    uint8_t     _host_address;
    uint8_t     _host_network;

    static const char* hardware_type_ids[];
    static const char* tlx_op_modes[];

    static const uint8_t hdlc_delimiter = 0x7E;
    static const uint8_t hdlc_address   = 0xFF;
    static const uint8_t hdlc_control   = 0x03;
    static const uint8_t header_size    = 8;

    static const size_t maxmsgsize      = ((255+10)*2+12)*2;
};

} /* namepace ComLynx */
} /* namespace RK */
