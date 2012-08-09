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
#include <string>

extern "C" {
#include <termios.h>
}
#include "common.H"

namespace RK {
class Config {
    public:
    Config (std::string portname_, uint8_t address_ = 0x01, uint8_t network_ = 0x00) :
        _portname(portname_),
        _network(network_),
        _address(address_)
    {
    }

    const std::string& portname (void) const
    {
       return _portname;
    }

    uint8_t address (void) const {return _address;}

    uint32_t baud (void) const {return _baud;}
    private:
    std::string _portname;

    uint8_t _network;
    uint8_t _address;

    static const uint8_t _frame_delimiter = 0x7E;
    static const uint8_t _frame_address   = 0xFF;
    static const uint8_t _frame_control   = 0x03;

    static const uint32_t _baud           = B19200;
};
}
