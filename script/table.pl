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




while (1) {
    $net = shift @ARGV;
    $sub = shift @ARGV;
    if (not defined $sub) {
        last;
    }
    ($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
    $yearOffset += 1900;
    printf "<TR><TH align=\"right\"> %d/%02d/%02d %d:%02d</TH>", $yearOffset, $month, $dayOfMonth, $hour, $minute;
    my $val = `$rklogger $net $sub 1e 14 8`;
    print "<TH align=\"right\"> $hardware_type_ids[$val]</TH>";

    $val = `$rklogger $net $sub 2 46 8`;
    printf("<TH align=\"right\"> %dW</TH>", $val);

    $val = `$rklogger $net $sub 2 4a 8`;
    printf("<TH align=\"right\"> %.1fkWh</TH>", $val/1000.0);

    $val = `$rklogger $net $sub 1 2 8`;
    printf("<TH align=\"right\"> %.1fkWh</TH>", $val/1000.0);

    $val = `$rklogger $net $sub a 2 8`;
    my $r = ((tlx_op_mode_id($val) == 4) or (tlx_op_mode_id($val) == 1) or (tlx_op_mode_id($val) == 2)) ? 255 : 0;
    my $g = ((tlx_op_mode_id($val) == 3) or (tlx_op_mode_id($val) == 1) or (tlx_op_mode_id($val) == 2)) ? 255 : 0;
    my $b = (tlx_op_mode_id($val) == 0) ? 255 : 0;
    printf "<TH bgcolor=\"#%.2X%.2X%.2X\">%s</TH></TR>\n", $r,$g,$b, $tlx_op_modes[tlx_op_mode_id($val)];
}
