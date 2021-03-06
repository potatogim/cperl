#!./perl

BEGIN {
    chdir 't' if -d 't';
    require "./test.pl";
    set_up_inc('../lib');
    require "./charset_tools.pl";
}

use utf8;

# It is important that the script contains at least one newline character
# that can be expanded to \r\n on DOSish systems.
# On cperl test that the BOM turns on utf8 and use feature 'unicode_strings'.
# [cperl #269], [perl #121292]
my $tests = [
  ["print 1;\nprint 2" => "12"],
  # no utf8 with perl5. before swallow_bom
  "$^V" =~ /c$/ ? ["sub österreich {q(ok)}\nprint österreich;\n" => "ok"] : (),
  # filter after swallow_bom
  "$^V" =~ /c$/ ? ["\nsub österreich {q(oknl)}\nprint österreich;\n" => "oknl"] : (),
  # with explicit hint
  # ["use utf8;\nsub österreich {q(ok8)}\nprint österreich;\n" => "ok8"],
  # test the hints bits (ignore 0x100 block scope)
  ["BEGIN{ print \$^H & 0x800 ? 'uni ' : '',\n  \$^H & 0x00800000 ? 'utf8' : '';}"
   => "$^V" =~ /c$/ ? "uni utf8" : ""],
  ];

plan(tests => 3 * @$tests);

for (@$tests) {
  my ($script, $result) = ($_->[0], $_->[1]);

  {
    use bytes;
    fresh_perl_is(byte_utf8a_to_utf8n("\xEF\xBB\xBF") . $script,
                  $result, {}, "UTF-8 BOM $result" );
  }

  # Big- and little-endian UTF-16
  for my $littleendian (0, 1) {
    my $encoding = $littleendian ? 'UTF-16LE' : 'UTF-16BE';
    my $prog = join '', map chr($_), map {
      $littleendian ? @$_[0, 1] : @$_[1, 0]
    } (
      # Create UTF-16.
      [ 0xFE, 0xFF ],
      map [ 0, utf8::native_to_unicode(ord($_)) ], split //, $script
    );
    fresh_perl_is($prog, $result, {}, "$encoding BOM $result");
    if ($ENV{PERL_TEST_KEEP_TMP}) {
      open(my $f,">","$encoding-$result.pl");
      print $f $prog;
      close $f;
    }
  }
}

