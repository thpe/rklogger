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
#include "common.H"


class Logger {
    public:
    virtual ~Logger () {};
    virtual Logger& start_host (uint8_t network, uint8_t host) = 0;
    virtual Logger& log_val    (uint8_t format, uint32_t val) = 0;
    virtual Logger& log_err    (void) = 0;
    virtual Logger& end_host   (void) = 0;
};


class DirectLogger : public Logger {
    public:
    virtual ~DirectLogger();
    virtual DirectLogger& start_host (uint8_t network, uint8_t host) {return *this;}
    virtual DirectLogger& log_val    (uint8_t format, uint32_t val);
    virtual DirectLogger& log_err    (void);
    virtual DirectLogger& end_host   (void) {return *this;}
};

