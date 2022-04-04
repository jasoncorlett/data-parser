use strict;
use warnings;

my $EXE = './echo';

use constant {
    TEST => 0,
    COMMENT => 1,
};

sub trim { $_[0] =~ s/^\s+|\s+$//g; $_[0] }

my @test_cases = map {
    [ map { trim($_) } split '#' ]
} <DATA>;

printf "1..%d\n", scalar @test_cases;

for my $n (1 .. @test_cases) {
    my ($test, $comment) = @{$test_cases[$n-1]};
    my $result = qx($EXE '$test' 2>&1);
    trim($result);
    my $status = $result eq $test;
    printf "%s %d", $status ? 'ok' : 'not ok', $n;
    printf " - %s\n", $comment if $comment;
    if (!$status) {
        printf "\t# Expected: %s\n", $test;
        printf "\t# Got: %s\n", $result;
    }
}

__DATA__
"Hello, World!"	        # String
true			 	    # Boolean true
false			 	    # Boolean false
null			 	    # Null
3.14				    # Number (floating point)
42					    # Number (integer)
[1,true,"three"]    	# Array
{"pi":3.14}			# Object