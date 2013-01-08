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

use strict;
use warnings;
use Text::CSV;
use FindBin;

my $rklogger = "$FindBin::Bin/../src/bin/rklogger";

my $csv = Text::CSV->new();

my ($second, $minute, $hour, $dayOfMonth, $month, $year, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
$year += 1900;
++$month;

my $net = shift @ARGV;
my $sub = shift @ARGV;
my @buffer;
my $year_found = 0;
while (<>) {
    if ($csv->parse($_)) {
        my @columns = $csv->fields();
        if ($columns[0]!=$year) {
            print $_;
            next;
        } else {
            $year_found = 1;
            print "$columns[0]";
            {
                my $val  = sprintf "%x ",50;
                my $res = `$rklogger $net $sub 78 $val 8`;
                $res = ($res>$columns[1])?$res:$columns[1];
                print ",$res";
            }
            my $i;
            if (@columns == $month+2) {
                for ($i = 2; $i < @columns-1; ++$i) {
                        print ",$columns[$i]";
                }
                {
                    my $val  = sprintf "%x ", 30;
                    my $res = `$rklogger $net $sub 78 $val 8`;
                    print ",$res";
                }
            } elsif (@columns == $month + 1) {
                for ($i = 2; $i < @columns-1; ++$i) {
                        print ",$columns[$i]";
                }
                {
                    my $val  = sprintf "%x ", 31+(($month-2)%13);
                    my $res = `$rklogger $net $sub 78 $val 8`;
                    print ",$res";
                    $val  = sprintf "%x ", 30;
                    $res = `$rklogger $net $sub 78 $val 8`;
                    print ",$res";
                }
            } else {
                die "Data file broken";
            }
        }
    } else {
        my $err = $csv->error_input;
        print "Failed to parse line: $err";
    }
}
if ($year_found == 0) {
    if ($month != 1) {
        die "Data file broken";
    }
    print   "\n$year";
    {
        my $val  = sprintf "%x ",50;
        my $res = `$rklogger $net $sub 78 $val 8`;
        print ",$res";
    }
    {
        my $val  = sprintf "%x ", 30;
        my $res = `$rklogger $net $sub 78 $val 8`;
        print ",$res";
    }
}
print "\n";
