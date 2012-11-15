#!/usr/bin/perl
# Copyright 2012 Thomas Petig
#
# This file is part of rklogger.
#
# rklogger is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# rklogger is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with rklogger.  If not, see <http://www.gnu.org/licenses/>.


use FindBin;

my $rklogger = "$FindBin::Bin/../src/bin/rklogger";

my @hardware_type_ids = ( 
    "", 
    "AC module, 1600W",
    "AC module, 3200W",
    "AC module, 4800W",
    "DC module, Medium Voltage",
    "DC module, High Voltage",
    "10kW inverter",
    "12,5kW inverter",
    "15kW inverter",
);

my @tlx_op_modes = ( 
    "Shutdown",
    "Connecting: booting",
    "Connecting: monitoring grid",
    "On Grid",
    "Fail safe",
    "Off Grid",
    "Unkown"
);

sub tlx_op_mode_id {
    my $val = $_[0];
    if ($val < 10) {
        return 0;
    } elsif ($val < 50) {
        return 1;
    } elsif ($val < 60) {
        return 2;
    } elsif ($val < 70) {
        return 3;
    } elsif ($val < 80) {
        return 4;
    } elsif ($val < 90) {
        return 5;
    }   
    return 6;
}

    ($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
    $yearOffset += 1900;


while (1) {
    $net = shift @ARGV;
    $sub = shift @ARGV;
    if (not defined $sub) {
        last;
    }
    print $yearOffset-1;
    {
    my $val  = sprintf "%x ",51;
    my $res = `$rklogger $net $sub 78 $val 8`;
    print ",$res";
    }
    for (my $i = 31; $i <= 42-12+$month+1; ++$i) {
        print ",0";
    }
    for (my $i = 31+$month+1; $i <= 42; ++$i) {
        my $val  = sprintf "%x ", $i;
        my $res = `$rklogger $net $sub 78 $val 8`;
        print ",$res";
    }
    print "\n";
    print $yearOffset;
    {
    my $val  = sprintf "%x ",50;
    my $res = `$rklogger $net $sub 78 $val 8`;
    print ",$res";
    }
    for (my $i = 31; $i <= 42-12+$month+1; ++$i) {
        my $val  = sprintf "%x ", $i;
        my $res = `$rklogger $net $sub 78 $val 8`;
        print ",$res";
    }
    print "\n";
}
