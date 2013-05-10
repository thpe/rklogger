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

#include "serialport.H"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
}

#include "common.H"
#include "fcs.H"
#include "config.H"

namespace RK {
SerialPort::SerialPort (Config& config_) :
    _config(config_),
    _port (-1),
    _port_config (),
    _curr_config ()
{
}
SerialPort::~SerialPort (void)
{
    close_port();
}

/* interface */
void SerialPort::flush (void)
{
    tcflush(_port,TCIOFLUSH);
}

#if 0
void find (void)
{
    uint8_t ibuffer[2000];
    for (size_t i = 1; i <= 0xE; ++i) {
        ping[5] = (i << 4) | 0xF;
        write_data(ping, sizeof(ping));
        sleep(1);
        read_data(ibuffer, 2000);
    }
}
#endif

void SerialPort::write_data (uint8_t* data, size_t length)
{
    for (size_t i = 1; i < length-1; ++i) {
        if (data[i] == 0x7E) {
            printf("data not stuffed\n");
        }
    }

    int ret = 0;
    ret = write (_port, data, length);
    if (ret < 0) {
        perror("RK::SerialPort::write_data");
    }
#if DEBUG
    for (size_t i = 0; i < length; ++i) {
        printf ("%2X ", data[i]);
    }
    printf ("%d bytes written\n", ret);
#endif
}

int SerialPort::read_data (uint8_t *data, size_t length)
{
    int cnt =0;// read  (_port, data, length);

    int ret = 1;
    while (ret > 0) {
        struct timeval timeout;
        timeout.tv_sec  = 0;
        timeout.tv_usec = 60000;
        fd_set  rfds;
        FD_ZERO (&rfds);
        FD_SET  (_port, &rfds);
        ret = select (_port+1, &rfds, NULL, NULL, &timeout);
        if (ret == -1)
            perror("select()");
        else if (ret > 0) {
        /* FD_ISSET(0, &rfds) will be true.
        ** */
            cnt += read (_port, data+cnt, length);
        }
    }

#if DEBUG
    for (int i = 0; i < cnt; ++i) {
        printf("%2X ", data[i]);
    }
    printf (" read %d bytes", cnt);
    printf("\n");
#endif
    return cnt;
}

bool SerialPort::open_port (void)
{
    if (_port != -1) {
        return true;
    }
    _port = open (_config.portname().c_str(), O_RDWR | O_NOCTTY| O_NDELAY);
    if (_port == -1) {
        return false; //TODO: throw...
    }
    ioctl(_port, TIOCEXCL, NULL);
    if (tcgetattr (_port, &_port_config) == -1) {
        std::cout << "[EE]: unable to get config for: " << _config.portname()
            << std::endl;
    }
    configure_port();
    tcflush(_port,TCIOFLUSH);
    return true;
}
void SerialPort::close_port (void)
{
    if (_port != -1) {
        if (tcsetattr (_port, TCSANOW, &_port_config) == -1) {
            std::cout << "[EE]: unable to write backup config" << std::endl;
        }
        ioctl(_port, TIOCNXCL, NULL);
        close (_port);
    }
}

void SerialPort::configure_port (void)
{
    tcgetattr (_port, &_curr_config);
    cfmakeraw(&_curr_config);
    //        memset (&config, 0,sizeof(config));  // struct nullen
    //
    ////        config.c_iflag = INPCK ;//| ICRNL;                // flags setzen
    ////        config.c_oflag = ;
    //        config.c_cflag = CS8 | CLOCAL | CREAD ;// | CRTSCTS;
    ////        config.c_lflag = ;
    _curr_config.c_cc[VTIME] = 10;
    _curr_config.c_cc[VMIN] = 1;
    //
    cfsetospeed (&_curr_config, _config.baud());     // baudrate setzen
    cfsetispeed (&_curr_config, _config.baud());

    if (tcsetattr (_port, TCSANOW, &_curr_config) == -1) {
        std::cout << "[EE]: unable to write config" << std::endl;
    }
    //fcntl(_port, F_SETFL,FNDELAY);
}
}

