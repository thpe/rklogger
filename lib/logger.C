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
#include "logger.H"

DirectLogger&
DirectLogger::log_val (uint8_t format, uint32_t val)
{
    uint32_t* ptr = &val;
    switch (format) {
    case 0x0:
        std::cout << "unknown format";
        break;
    case 0x01:
        std::cout << *ptr;
        break;
    case 0x02:
        std::cout << *reinterpret_cast< int8_t* > (ptr);
        break;
    case 0x03:
        std::cout << *reinterpret_cast< int16_t* > (ptr);
        break;
    case 0x04:
        std::cout << *reinterpret_cast< int32_t* > (ptr);
        break;
    case 0x05:
        std::cout << *reinterpret_cast< uint8_t* > (ptr);
        break;
    case 0x06:
        std::cout << *reinterpret_cast< uint16_t* > (ptr);
        break;
    case 0x07:
        std::cout << val;
        break;
    case 0x08:
        std::cout << *reinterpret_cast< float* > (ptr);
        break;
    case 0x09:
    default:
        std::cout << "cannot interpret format: " << std::hex << format << std::dec;
    }
//    std::cout << std::endl;
    return *this;
}

DirectLogger&
DirectLogger::log_err (void)
{
    std::cout << "U";
    return *this;
}
