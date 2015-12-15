BEGIN {
    if( $ENV{PERL_CORE} ) {
        @INC = '../../lib';
    }
}


use Test::More tests => 2, import => [qw(!fail)];

can_ok(__PACKAGE__, qw(ok pass like isa_ok));
ok( !__PACKAGE__->can('fail'),  'fail() not exported' );
