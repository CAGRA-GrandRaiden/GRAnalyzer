#!/usr/bin/perl
# script to move the array declaration 
# 'static int dr_data_index' with a Flag
# in the *.c files

@substitute = ("","");
@linelabels = (0,0);
$linenumber = 0;
$flag1 = "//FLAG\n";
$flag2 = "static int dr_data_index";

#copy the file to an array
open(FILE, ,'<', @ARGV) or die "Couldn't open required file, $!";
@filearray = <FILE>;
close(FILE);

#scan the array for the line to be switched, and their line number
foreach $temp (@filearray){
	my ($LHS, $RHS) = split /\[/, $temp;
	if($temp eq $flag1) {
		$substitute[0] = $temp;
		$linelabels[0] = $linenumber;
	}
	elsif($LHS eq $flag2) {
		$substitute[1] = $temp;
		$linelabels[1] = $linenumber;
	}
	$linenumber += 1;
}

#move the good line to the position of the flag
$filearray[$linelabels[0]] = $substitute[1];
$filearray[$linelabels[1]] = "";

open(FILE, '>', @ARGV) or die "Couldn't open required file, $!";
print FILE @filearray;
close(FILE);
