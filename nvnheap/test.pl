#!/usr/bin/perl

$something = "how are you";

$something =~ /(are)/;
print "$1\n";

$something_else = `ls /`;

print $something_else."\n";
