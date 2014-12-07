#!/bin/perl

if (@ARGV < 1) 
{
  print STDERR "USAGE :: $0 <Number_of_chars>\n";
  exit 1;
}

my $num_of_chars = shift;
print STDOUT "Number of chars : $num_of_chars\n";

print map { ("a".."z","A".."Z",0..9," "," "," ",",",";",".",":","-","_","#","*","+","~","!","§","\$","%","&","(",")","="," "," ","?","{","}","[","]","|","<",">") [rand 87] } (1..$num_of_chars);
print "\n"
