#!./perl

# test that "use vars;" doesn't load Carp

BEGIN {
    chdir 't' if -d 't';
    @INC = '../lib';
    $ENV{PERL5LIB} = '../lib';
}

$| = 1;

print "1..1\n";

use vars;
# Carp not loaded yet; $VERSION is booted early in cperl, so check for a PP sub
print defined &Carp::get_subname ? "not ok 1\n" : "ok 1\n";
