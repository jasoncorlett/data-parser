#!/usr/bin/env perl
# Generate some enums

use strict;
use warnings;

use IO::File;

my @enums = (
    {
        name => 'token_type',
        tostring => 'token_type_name',
        members => [ qw(
            ERROR NULL TRUE FALSE STRING NUMBER COMMA COLON EOF
            LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET
        ) ],
        initial => -1,
        prefix => 'TOKEN_',
    },
    {
        name => 'json_type',
        tostring => 'json_type_name',
        members => [ qw(ERROR NULL STRING BOOLEAN NUMBER ARRAY OBJECT) ],
        initial => -1,
        prefix => 'JSON_',
    }
);

my $h = IO::File->new('enum.h', 'w');
my $c = IO::File->new('enum.c', 'w');

$h->print("#pragma once\n\n");
$c->print("#include \"enum.h\"\n");
$_->print("// Genrated code, see bin/gen-enum.pl\n") for $h, $c;

for my $enum (@enums) {
    my $initial = $enum->{initial};
    $h->print("typedef enum {\n");
    for my $member (@{$enum->{members}}) {
        $h->print("\t", $enum->{prefix}, $member);

        if ($initial) {
            $h->print(" = ", $initial);
            undef $initial;
        }

        $h->print(",\n");
    }
    $h->print("} ", $enum->{name}, ";\n\n");
    my $decl = "char *" . $enum->{tostring} . "(" . $enum->{name} . " val)";
    $h->print($decl, ";\n\n");

    if ($enum->{tostring}) {
        $c->print($decl, " {\n");
        $c->print("\tswitch (val) {\n");

        for my $member (@{$enum->{members}}) {
            $c->print("\tcase ", $enum->{prefix}, $member, ": return \"", $member, "\";\n");
        }

        $c->print("\tdefault: return \"<UNKNOWN ", $enum->{name}, ">\";\n");
        $c->print("\t}\n}\n\n");
    }
}

$h->close;
$c->close;
