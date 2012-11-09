#!/usr/bin/perl
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
    for (my $i = 31; $i <= 42-12+$month+1; ++$i) {
        my $val  = sprintf "%x ", $i;
        my $res = `$rklogger $net $sub 78 $val 8`;
        print ",$res";
    }
    print "\n";
}
