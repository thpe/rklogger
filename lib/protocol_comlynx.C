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

#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdio>

#include "protocol_comlynx.H"
#include "serialport.H"
#include "fcs.H"
#include "logger.H"

#include <arpa/inet.h>

namespace RK {
namespace ComLynx {

const char* Protocol::hardware_type_ids[] = {
    "",
    "AC module, 1600W",
    "AC module, 3200W",
    "AC module, 4800W",
    "DC module, Medium Voltage",
    "DC module, High Voltage",
    "10kW inverter",
    "12,5kW inverter",
    "15kW inverter",
};

const char* Protocol::tlx_op_modes[] = {
    "Shutdown",
    "Connecting: booting",
    "Connecting: monitoring grid",
    "On Grid",
    "Fail safe",
    "Off Grid",
    "Unkown"
};

int tlx_op_mode_id (uint8_t val) {
    if (val < 10) {
        return 0;
    } else if (val < 50) {
        return 1;
    } else if (val < 60) {
        return 2;
    } else if (val < 70) {
        return 3;
    } else if (val < 80) {
        return 4;
    } else if (val < 90) {
        return 5;
    }
    return 6;
}
#if 0
class Message {
    public:
    Message (uint8_t* msg_, size_t length_) :
        _msg(NULL),
        _length(length_)
    {
        _msg = new uint8_t[length_];
        memcpy(msg_, _msg, length_);
    }


    private:
    uint8_t* _msg;
    uint8_t* _length;
}
#endif

bool
Protocol::ping (uint8_t network, uint8_t address)
{
    uint8_t msg[maxmsgsize];
    uint8_t buf[maxmsgsize*2];

    init_msg (msg, network, address, 0, 0x15);

    size_t length = 12;
    fcs  (msg, length-4);
    assert (checkfcs16 (msg+1, length-4));

    length = stuff(msg, buf, length);

//    _port.flush();
    _port.write_data(buf, length);
    int ret = _port.read_data(buf, maxmsgsize*2);
    ret = unstuff (buf, msg, ret);
    std::cout << ret << "ret\n";


    if (ret > 12) {
//        uint8_t* ptr = NULL;
//        get_msg_frame (msg, ret, ptr);
        return true;
    }


    return false;
}

bool
Protocol::node_information (uint8_t network, uint8_t address)
{

    uint8_t msg[maxmsgsize];
    uint8_t buf[maxmsgsize];

    init_msg (msg, network, address, 0x1D, 0x13);
    memset (get_msg_data(msg), 0xFF, 0x1D);
    int length = 12+ 0x1D;
    fcs (msg, length - 4);
    assert (checkfcs16 (msg+1, length-4));
    length = stuff(msg, buf, length);

    _port.write_data(buf, length);
    int ret = _port.read_data(buf, maxmsgsize*2);
    ret = unstuff (buf, msg, ret);

    if (ret > 0) {
        uint8_t* ptr = NULL;
        if (get_msg_frame (msg, ret, ptr)) {
            node_info_data_mask* ni = reinterpret_cast<node_info_data_mask*> (get_msg_data (ptr));

            printf("product number: %s\n", ni->product_number);
            printf("serial  number: %s\n", ni->serial_number);
            printf("address: network=%d, subnet=%d, address=%d\n", ni->network, ni->subnet, ni->address);
        }
    }
    return false;

}

bool
Protocol::embedded_can (uint8_t network, uint8_t address)
{
    uint8_t format;
    uint32_t val;

    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t t = time(0);
    strftime(s, MAXLEN, "%Y/%m/%d %T", localtime(&t));
    std::cout << "<TR><TH align=\"right\">" << s << "</TH>";

    if (embedded_can(network, address, 0x1E, 0x14, 0x08, format, val)) {
        printf("<TH align=\"right\"> %s</TH>", hardware_type_ids[val]);
    }
    if (embedded_can(network, address, 0x02, 0x46, 0x08, format, val)) {
        printf("<TH align=\"right\"> %dW</TH>", val);
    }
    if (embedded_can(network, address, 0x02, 0x4A, 0x08, format, val)) {
        printf("<TH align=\"right\"> %.1fkWh</TH>", (double)val/1000);
    }
    if (embedded_can(network, address, 0x01, 0x02, 0x08, format, val)) {
        printf("<TH align=\"right\"> %.1fkWh</TH>", (double)val/1000);
    }
    if (embedded_can(network, address, 0x0A, 0x02, 0x08, format, val)) {
        int r = (tlx_op_mode_id(val) == 4) or
                (tlx_op_mode_id(val) == 1) or
                (tlx_op_mode_id(val) == 2) ? 0xFF : 0;
        int g = (tlx_op_mode_id(val) == 3) or
                (tlx_op_mode_id(val) == 1) or
                (tlx_op_mode_id(val) == 2) ? 0xFF : 0;
        int b = (tlx_op_mode_id(val) == 0) ? 0xFF : 0;
        printf("<TH bgcolor=\"#%.2X%.2X%.2X\">%s</TH></TR>\n", r,g,b, tlx_op_modes[tlx_op_mode_id(val)]);
    }
    return false;
}
bool
Protocol::embedded_can (uint8_t  network, uint8_t   address,
                        uint8_t  index,   uint8_t   subindex, uint8_t modindex)
{
    uint8_t format;
    uint32_t val;
    if (embedded_can (network, address, index, subindex, modindex, format, val)) {
        DirectLogger log;
        log.start_host(network, address).log_val(format, val).end_host();
        return true;
    }
    DirectLogger log;
    log.start_host(network, address).log_err().end_host();

    return false;
}

bool
Protocol::embedded_can (uint8_t  network, uint8_t   address,
                        uint8_t  index,   uint8_t   subindex, uint8_t modindex,
                        uint8_t& format,  uint32_t& val)
{
    uint8_t msg[maxmsgsize];
    uint8_t buf[maxmsgsize];

    init_msg (msg, network, address, 0x0A, 0x01);
    memset   (get_msg_data(msg), 0x00, 0x0A);

    embedded_can_data_mask* ecd = reinterpret_cast< embedded_can_data_mask* > (get_msg_data(msg));

    ecd->doc_no        = 0xC8;
    ecd->unused        = 0x00;
    ecd->dest_mod_id   = modindex;
    ecd->src_mod_id    = 0xD;
    ecd->param_idx     = index;
    ecd->param_sub_idx = subindex;
    ecd->flags         = 0x8;


    int length = 12+ 0x0A;
    fcs (msg, length - 4);
    assert (checkfcs16 (msg+1, length-4));
    length = stuff(msg, buf, length);

    _port.write_data(buf, length);
    int ret = _port.read_data(buf, maxmsgsize*2);
    ret = unstuff (buf, msg, ret);

    if (ret > 0) {
        uint8_t* ptr = NULL;
        if (get_msg_frame (msg, ret, ptr)) {
            embedded_can_data_mask* rep = reinterpret_cast< embedded_can_data_mask* > (get_msg_data(ptr));

            format = rep->data_type;
            val    = rep->param_val;
            return true;
        }
    }
    return false;
}

uint8_t*
Protocol::get_msg_data (uint8_t* msg)
{
    return msg + 9;
}

bool
Protocol::get_msg_frame (uint8_t* msg, size_t length, uint8_t*& first_frame)
{
    size_t ptr = 0;
    bool ret = false;
    if (length < 10) {
        std::cerr << "[EE] ComLynx::Protocol::get_msg_frame: msg to short "
                  << length << std::endl;
        return false;
    }

    frame_start_mask* fsm = NULL;
    do {
        while ((msg[ptr] != 0x7E) and (ptr < length - 10)) {++ptr;}
        fsm =  frame_start_mask_ptr (msg+ptr);
        if ((fsm->hdlc_delimiter != 0x7E) or
            (fsm->hdlc_address   != 0xFF) or
            (fsm->hdlc_control   != 0x03) or
            ((fsm->src_address   == _host_address) and
             (fsm->src_network   == _host_network)) or
            (fsm->dest_address   != _host_address) or
            (fsm->dest_network   != _host_network)) {
            ++ptr;
            continue;
        }
        if (fsm->type & 0x40) {
            printf ("[EE] received transmission error from %2X %2X\n",
                    fsm->src_network,
                    fsm->src_address);
            return false;
        } else if (fsm->type & 0x20) {
            std::cerr << "[EE] received application error from " << std::hex
                      << fsm->src_network << " " <<  fsm->src_address
                      << std::endl << std::dec;
            return false;
        }
        first_frame = msg+ptr;
        ret = true;
        break;

    } while (ptr < length);

    if (fsm == NULL or ptr >= length) {
        return false;
    }
    frame_end_mask* fem = frame_end_mask_ptr(first_frame);
    if (fem->hdlc_delimiter != hdlc_delimiter) {
        std::cerr << "[EE] no hdlc frame delimiter from" << std::hex
                  << fsm->src_network      << " " << fsm->src_address
                  << std::endl             << std::dec;
        return false;
    }
    if (not checkfcs16(first_frame+1, fsm->size+8)) {
        std::cerr << "[EE] fcs error from " << std::hex
                  << fsm->src_network      << " " << fsm->src_address
                  << std::endl             << std::dec;
        return false;
    }


    return ret;
}



void
Protocol::init_msg (uint8_t* msg, uint8_t network, uint8_t address, uint8_t size, uint8_t type)
{
    frame_start_mask* fsm = frame_start_mask_ptr (msg);
    fsm->hdlc_delimiter   = hdlc_delimiter;
    fsm->hdlc_address     = hdlc_address;
    fsm->hdlc_control     = hdlc_control;
    fsm->src_network      = _host_network;
    fsm->src_address      = _host_address;
    fsm->dest_network     = network;
    fsm->dest_address     = address;
    fsm->size             = size;
    fsm->type             = type;
}

void
Protocol::fcs (uint8_t* msg, size_t length)
{
    uint16_t fcs = calcfcs16 (msg+1, length);
    frame_end_mask* fem = frame_end_mask_ptr(msg);
    fem->fcs_lsb = (uint8_t) (fcs&0xFF);
    fem->fcs_msb = (uint8_t) (fcs>>8);
    fem->hdlc_delimiter = hdlc_delimiter;
}

size_t
Protocol::stuff (uint8_t* msg, uint8_t* buf, size_t length)
{
    buf[0] = msg[0];
    size_t j = 1;
    for (size_t i = 1; i < length-1; ++i) {
        switch (msg[i]) {
        case 0x7E:
            buf[j] = 0x7D;
            ++j;
            buf[j] = 0x5E;
            ++j;
            break;
        case 0x7D:
            buf[j] = 0x7D;
            ++j;
            buf[j] = 0x5D;
            ++j;
            break;
        default:
            buf[j] = msg[i];
            ++j;
        }
    }
    buf[j] = msg[length-1];
    ++j;
    return j;
}

size_t
Protocol::unstuff (uint8_t* msg, uint8_t* buf, size_t length)
{
    if (length == 0) {
        return 0;
    }
    buf[0] = msg[0];
    if (length == 1) {
        return 1;
    }
    size_t j = 1;
    for (size_t i = 1; i < length-1; ++i) {
        switch (msg[i]) {
        case 0x7D:
            ++i;
            switch (msg[i]) {
            case 0x5D:
                buf[j] = 0x7D;
                ++j;
                break;
            case 0x5E:
                buf[j] = 0x7E;
                j++;
                break;
            default:
                printf("[EE]: unstuff: invaldid esc seqence: 7D %2X", msg[i]);
            }
            break;
        default:
            buf[j] = msg[i];
            ++j;
        }
    }
    buf[j] = msg[length-1];
    ++j;
    return j;
}

Protocol::frame_start_mask*
Protocol::frame_start_mask_ptr(uint8_t* msg)
{
    return reinterpret_cast< frame_start_mask* > (msg);
}
Protocol::frame_end_mask*
Protocol::frame_end_mask_ptr(uint8_t* msg)
{
    frame_start_mask* fsm = frame_start_mask_ptr (msg);

    int offset = header_size + fsm->size + 1;

    return reinterpret_cast< frame_end_mask* > (msg + offset);
}


} /* namepace ComLynx */
} /* namespace RK */
