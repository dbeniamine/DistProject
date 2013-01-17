#!/usr/bin/perl
use Getopt::Long;
my $help;
my $verbose;
my $oldproto="undef";
my $exp="undef";
my $init=0;
GetOptions("help|h"=>\$help,"verbose|v"=>\$verbose,"protocol|proto|p=s"=>\$oldproto,"experiment|exp|e=s"=>\$exp,"initiator|init|i=s"=>\$init);

if ($help || $exp=~m"undef" || $proto=~m"undef")
{
    print "usage : ./exp [options] --protocol=<proto> --experiment=<exp>\n";
    print "protocol | proto | p : L | T (see ./Broadcast -h\n";
    print "experiment | exp | e : latency | throughput\n";
    print "\nOptions :\n\n";
    print "help | h             : print this help and exist\n";
    print "verbose | v          : write the experiments output in files
                                  output_protocol_experiment_NbNods\n";
    print "initiator | init | i : N, if the experiment is latency, N is the id of the
                                  node which launch a broadcast\n";
    exit 1
}
$proto="-".$oldproto;

$i=$init>0?$init+1:1;
while ( $i <= 10000 ){
    $rounds=100*$i;
    $process=$i-1;
    $output="output_".$oldproto."_".$exp."_".$i;
    #create the experiment file
    open($file, ">",tmp) or die "cannot open file tmp";
    if ($exp =~m "throughput"){
        $Nmessages=$i;
        for ($j=0;$j<$i;$j++){
            print $file "$j broadcast\n";
        }
    }else{
        print $file "$init broadcast\n" ;
        $Nmessages=1;
    }
    #write the start for each runs
    for($j=0;$j<=$rounds;$j++){
        print $file "start\n";
    }
    close $file;

    #beginning the exp
    if($verbose){
        system("./Broadcast $proto -N $i -R $rounds < tmp > $output");
        $FirstEmptyRound=`cat $output | grep -A 1 \"Delivered\" | tail -1 | awk {\'print \$3\'}`;
    }else{
        $FirstEmptyRound=`./Broadcast $proto -N $i -R $rounds < tmp | grep -A 1 \"Delivered\" | tail -1 | awk {\'print \$3\'}`;
    }
    #Compute the results
    $totalRounds=$FirstEmptyRound-1;
    $throughput=$totalRounds==0 ? "NaN" :$Nmessages/$totalRounds;

    #and print them

    print "$Nmessages broadcast on $i nodes done in $totalRounds rounds\n";
    print "latency    : $totalRounds\n";
    print "throughput : $throughput\n";

    #get ready for the next exp
    if ( $i < 10 ){
        $i=$i+1;
    }else{
        $i=10*$i;
    }
    sleep 1;
    system("rm tmp");
}
