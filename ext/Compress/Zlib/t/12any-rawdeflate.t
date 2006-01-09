BEGIN {
    if ($ENV{PERL_CORE}) {
	chdir 't' if -d 't';
	@INC = ("../lib", "lib/compress");
    }
}

use lib 't';
use strict;
use warnings;

use IO::Uncompress::AnyInflate qw($AnyInflateError) ;
use IO::Compress::RawDeflate   qw($RawDeflateError) ;
use IO::Uncompress::RawInflate qw($RawInflateError) ;

sub getClass
{
    'AnyInflate';
}


sub identify
{
    'IO::Compress::RawDeflate';
}

require "any.pl" ;
run();
