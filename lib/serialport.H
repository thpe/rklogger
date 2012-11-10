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

extern "C" {
#include <termios.h>
}

#include "common.H"
#include "config.H"

namespace RK {
class SerialPort {
    public:
    SerialPort  (Config& config_);
    ~SerialPort (void);

    /* interface */
    void flush      (void);
    void write_data (uint8_t* data, size_t length);
    int  read_data  (uint8_t *data, size_t length);
    bool open_port  (void);

    private:
    void close_port     (void);
    void configure_port (void);


    Config         _config;
    int            _port;
    struct termios _port_config;
    struct termios _curr_config;
};
}
