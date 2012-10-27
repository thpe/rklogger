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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>


#include "config.H"
#include "serialport.H"
#include "protocol_comlynx.H"

#define SERIAL_DEVICE "/dev/ttyUSB0"
#define NETWORK1 0x32
#define NETWORK2 0x32
#define HOST1 0x01
#define HOST2 0x02

namespace RK {
}

int main (int argc, char** argv)
{
    std::string device(SERIAL_DEVICE);
    try {
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("device,d", boost::program_options::value<std::string>(&device)->default_value(SERIAL_DEVICE), "produce help message")
        ;

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    if (argc != 1 and argc != 6 and argc != 3) {
        exit(EXIT_FAILURE);
    }

    {
    RK::Config config (device);
    RK::SerialPort port (config);

    RK::ComLynx::Protocol protocol (port);

    if (argc == 1) {
        protocol.embedded_can (NETWORK1, HOST1);
        protocol.embedded_can (NETWORK2, HOST2);
    } else if (argc == 3) {
       uint32_t network;
       sscanf (argv[1], "%X", &network);
       uint32_t host;
       sscanf (argv[2], "%X", &host);
       if (not protocol.ping (network, host)) {
           exit(EXIT_FAILURE);
       }
    } else if (argc == 6) {
       uint32_t network;
       sscanf (argv[1], "%X", &network);
       uint32_t host;
       sscanf (argv[2], "%X", &host);
       uint32_t parm_idx;
       sscanf (argv[3], "%X", &parm_idx);
       uint32_t parm_subidx;
       sscanf (argv[4], "%X", &parm_subidx);
       uint32_t parm_modidx;
       sscanf (argv[5], "%X", &parm_modidx);
       return protocol.embedded_can (network, host, parm_idx, parm_subidx, parm_modidx);
    }
    }

    exit (EXIT_SUCCESS);
}
