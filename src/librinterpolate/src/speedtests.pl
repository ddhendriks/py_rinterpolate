#!/usr/bin/env perl
use strict;

# run various combinations of optimization and compiler
# to see what is fastest when testing the rinterpolation library

my %fastest;
my $n=0;
my $vb = 0;

foreach my $compiler (compilers())
{
    foreach my $optflags ('-O2',
                          '-O3')
    {
        foreach my $executable ({cmd => './test_rinterpolate', label => 'STANDARD'},
                                {cmd => './pgo.sh', label => 'PGO'})
        {
            my $label = "CC=$compiler COPTFLAGS=$optflags : $executable->{label}";
            my $cmd = "make clean; CC=$compiler COPTFLAGS=$optflags make ; $executable->{cmd}";
            print $cmd,"\n";
            my $r = `$cmd 2>/dev/null`;
            $n++;
            while($r=~/Report\:\s+([^\:]+)\:\s+(\S+)/g)
            {
                print "CF $1 ($2) to $fastest{$1}\n"if($vb);
                if(!defined $fastest{$1} ||
                   $2 < $fastest{$1})
                {
                    $fastest{$1} = $2.' '.$label;
                }
                print "NOW $fastest{$1}\n"if($vb);
            }
            #goto END if($n>2);
        }
    }
}
 END:
                
print "\n\nFastest:\n\n\n";
foreach my $k (sort keys %fastest)
{
    printf "%20s : %s\n",$k,$fastest{$k}; 
}

exit;

############################################################x

sub compilers
{
    my @c = `dpkg --list |grep 'C compiler'`;
    my %v;
    my %compilers;
    my @compilers_list;
    my $vb=0;
    foreach my $cc (@c)
    {
        print "CC $cc\n"if($vb);
        my @cc = split(/\s+/,$cc);
        my $which = `which $cc[1]`;chomp $which;
        if($which)
        {
            my $fullpath = `readlink -e $which`; chomp $fullpath;
            my $v = `$cc[1] --version 2>\&1| head -1`;
            if($v=~/(\d+\.\d+(?:\.\d+))/)
            {
                my $version = $1;
                chomp $v;
                print "Found compiler : $cc[1] (at $fullpath) -> $v -> $version\n"if($vb);
                if($compilers{$fullpath} eq $version)
                {
                    print "Already in use\n"if($vb);
                }
                else
                {
                    $compilers{$fullpath} = $version;
                    push(@compilers_list,$cc[1]);
                }
            }
        }
    }
    return @compilers_list;
}
