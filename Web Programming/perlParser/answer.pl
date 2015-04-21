#!/bin/perl
#
# Brian Taylor
#
# Answer

%teamRank;
%teamWins = ();
%teamPlays = ();
%teamAWP = ();
%teamGoals = ();

for ($txt = 1; $txt < 13; $txt++) {
	if ($txt == 5) { $txt++; }
#foreach my $fp (glob("./*.txt")) {
	open my $fh, "<", "$txt.txt" or die "can't open '$txt.txt': $OS_ERROR";
	$count = 1;
	while (<$fh>) {
		while ($_ =~ s/([A-Z]+|[A-Z]+\s[A-Z]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(.\d+)\s+(.\d+)\s+(.\d+)\s+(.\d+|\d.\d+)\s+(.\d+)\s+(.\d+)\s+(\d+.\d+)//) {
			$teamRank->{$txt}->{$1} = $count;
			$count++;

			if (exists($teamWins{$1})) {
				$teamWins{$1} += $3;
			}
			else {
				$teamWins{$1} = $3;
			}
			if (exists($teamPlays{$1})) {
				$teamPlays{$1} += $2;
			}
			else {
				$teamPlays{$1} = $2;
			}
			if (exists($teamGoals{$1})) {
				$teamGoals{$1} += $6;
			}
			else {
				$teamGoals{$1} = $6;
			}
#			print "$1: $teamWins{$1}\n";
#			print "$1\t$2\t$3\t$4\t$5\t$6\t$7\t$8\t$9\t$10\t$11\t$12\t$13\t$14\t$15\t$16\t$17\t$18\t$19\t$20\t$21\n";
		}
	}

	close $fh or die "can't close '$fh': $OS_ERROR";
}

sub rank
{
	if (!exists($teamRank->{12}->{$_[0]})) {
		print "\nError! $_[0] not found in stats.\n\n";
		return;
	}

	print "\nStatistics for $_[0]:\n";
	for ($i = 12; $i > 0; $i--) {
		if ($i == 5) { $i--; }
		if ($i > 10) { print "20".($i-1)."-20".($i).": $teamRank->{$i}->{$_[0]}\n"; }
		elsif ($i == 10) { print "200".($i-1)."-20".($i).": $teamRank->{$i}->{$_[0]}\n"; }
		else { print "200".($i-1)."-200".($i).": $teamRank->{$i}->{$_[0]}\n"; }
	}
	print "\n";
}

sub yearranking
{
	if ($_[0] > 2000 && $_[0] != 2005 && $_[0] < 2013) {
		print "\nStatistics for the ".($_[0]-1)."-$_[0] season:\n";
		@sorted = sort { $teamRank->{$_[0]-2000}->{$a} <=> $teamRank->{$_[0]-2000}->{$b} } keys $teamRank->{$_[0]-2000};
		for ($i = 1; $i <= 30; $i++) {
			print "$i $sorted[$i-1]\n";
		}
	}
	else
	{
		print "No statistics for the ".($_[0]-1)."-$_[0] season.\n";
	}
	print "\n";
}

sub awp
{
	foreach $key (sort keys %teamWins) {
		$teamAWP{$key} = $teamWins{$key} / $teamPlays{$key};
	}
	print "\nWin Percentages for all seasons:\n";
	@sorted = sort { $teamAWP{$b} <=> $teamAWP{$a} } keys %teamAWP;
	for ($i = 0; $i < 30; $i++) {
		print "$sorted[$i]:\t";
		if (length($sorted[$i]) < 7) { print "\t"; }
		print $teamAWP{$sorted[$i]}."\n";
	}
	print "\n";
}

sub goals
{
	print "\nGoals Scored for all seasons:\n";
	@sorted = sort { $teamGoals{$b} <=> $teamGoals{$a} } keys %teamGoals;
	for ($i = 0; $i < 30; $i++) {
		print "$sorted[$i]:\t";
		if (length($sorted[$i]) < 7) { print "\t"; }
		print $teamGoals{$sorted[$i]}."\n";
	}
	print "\n";
}

while (<STDIN>) {
	chomp($_);
# if team name
	$menu = uc($_);
	if ($menu =~ m/[A-Z]+/) {
		if ($menu =~ m/AWP/) {
			awp();
		}
		elsif ($menu =~ m/GOALS/) {
			goals();
		}
		else {
			rank($menu);
		}
	}

# if year number
	if ($menu =~ m/\d+/) {
		yearranking($menu);
	}
}