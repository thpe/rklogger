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
use Scalar::Util qw(looks_like_number);


if (@ARGV != 1) {
    print "usage: $0 <database.csv>\n";
    exit;
}

my $rklogger = "$FindBin::Bin/../src/bin/rklogger";

my $csv = Text::CSV->new();

my ($second, $minute, $hour, $dayOfMonth, $month, $year, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
$year += 1900;

my $filename = $ARGV[0];

open FILE, ">$filename.new" or die $!;
my $line = <>;
$csv->parse($line);
my @columns = $csv->fields();

my $net = $columns[0];
my $sub = $columns[1];
my $start_year = $columns[2];

while (<>) {
    print FILE $line;
    $line = $_;
    ++$start_year;
}
for (; $start_year <= $year; ++$start_year) {
    print FILE $line."\n";
    $line = "";
}
$csv->parse($line);
@columns = $csv->fields();
my $res =`$rklogger $net $sub 1 2 8`;
if (looks_like_number($res)) {
    if (looks_like_number($columns[$month])) {
        if ($res > $columns[$month]) {
            $columns[$month] = $res;
        }
    } else {
        $columns[$month] = $res;
    }
}
$csv->combine(@columns);
print FILE $csv->string();

close FILE;
system "rm $filename";
system "mv $filename.new $filename";
