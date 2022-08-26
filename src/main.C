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
#include <vector>
#include <sstream>
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
    std::string         device(SERIAL_DEVICE);
    std::vector< int >  parameter;
    bool                scan = false;
    bool                linefeed = false;

    try {
        boost::program_options::options_description desc("Allowed options");
        std::vector< std::string > string_parm ;
        desc.add_options()
            ("help,h",   "produce help message")
            ("device,d", boost::program_options::value<std::string>(&device)->default_value(SERIAL_DEVICE), "change the communication device")
            ("scan,s",   "perform a network scan")
            ("param",    boost::program_options::value< std::vector<std::string> >(&string_parm), "<network> <node> <param_idx> <param_subidx> <param_modulidx>")
            ("lf",   "add a line feed")
        ;


        boost::program_options::positional_options_description p;
        p.add("param", -1);



        boost::program_options::variables_map vm;
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv)
            .options(desc)
            .positional(p)
            .run(), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }
        if (vm.count("scan")) {
            scan = true;
        }
        if (vm.count("lf")) {
            linefeed = true;
        }
        if (vm.count("param")) {
            for (size_t i = 0; i < string_parm.size(); ++i) {
                std::stringstream interpreter;
                interpreter << std::hex << string_parm[i];
                int var;
                interpreter >> var;
                parameter.push_back(var);
            }
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    size_t num_parm = parameter.size();
    if (num_parm != 5 and num_parm != 2 and not scan) {
        std::cerr << "Invalid number of arguments (should be 2 or 5): " << num_parm << std::endl;
        std::cerr << "Type '" << argv[0] << " --help' to get more information." << std::endl;
        exit(EXIT_FAILURE);
    }

    {
    RK::Config config (device);
    RK::SerialPort port (config);
    for (size_t i = 0; i < 5; ++i) {
        if (port.open_port()) {
            break;
        }
        sleep (2);
    }

    RK::ComLynx::Protocol protocol (port);

    if (scan) {
        return protocol.scan();
    }

    if (num_parm == 2) {
       if (not protocol.ping (parameter[0], parameter[1])) {
           if (linefeed) {
              std::cout << std::endl;
           }
           exit(EXIT_FAILURE);
       }
    } else if (num_parm == 5) {
       return protocol.embedded_can (parameter[0],
                                     parameter[1],
                                     parameter[2],
                                     parameter[3],
                                     parameter[4]);
    }
    }

    if (linefeed) {
        std::cout << std::endl;
    }
    exit (EXIT_SUCCESS);
}
