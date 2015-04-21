#!/bin/perl
#
# Brian Taylor
#
# HTML parser

use Getopt::Long;

my $input = '';
my $output = '';

GetOptions('input=s' => \$input,
		   'output=s' => \$output);


if (open (IN, $input)) 
{
	if (open(OUT, ">", $output))
	{
		while(<IN>){
			s/<(?:[^>'"]*|(['"]).*?\1)*>/ /gs;
			while ($_ =~ s/([A-Z]+|[A-Z]+\s[A-Z]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(.\d+)\s+(.\d+)\s+(.\d+)\s+(.\d+|\d.\d+)\s+(.\d+)\s+(.\d+)\s+(\d+.\d+)//) {

				print OUT "$1\t$2\t$3\t$4\t$5\t$6\t$7\t$8\t$9\t$10\t$11\t$12\t$13\t$14\t$15\t$16\t$17\t$18\t$19\t$20\t$21\n";
			}
		}
	}
	else
	{
		print "Could not write to $output\n";
	}
	close(OUT);
}
else 
{
	print "Could not open $input\n";
}
close(IN);
