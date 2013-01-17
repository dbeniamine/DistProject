#!/usr/bin/perl
if (scalar(@ARGV)!=2)
{
    print "usage : ./exp <protocol> <exp>\n";
    print "protocol : -T | -L see ./Broadcast -h\n";
    print "exp : latency | throughput\n";
    exit 1
}
$i=1;
while ( $i <= 10000 ){
    $rounds=100*$i;
    $process=$i-1;
    #create the experiment file
    open($file, ">",tmp) or die "cannot open file tmp";
    if ($ARGV[1] =~m "throughput"){
        $Nmessages=$i;
        for ($j=0;$j<$i;$j++){
            print $file "$j broadcast\n";
        }
    }else{
        print $file "0 broadcast\n" ;
        $Nmessages=1;
    }
    #write the start for each runs
    for($j=0;$j<=$rounds;$j++){
         print $file "start\n";
    }
    close $file;

    #beginning the exp

    $FirstEmptyRound=`./Broadcast $ARGV[0] -N $i -R $rounds < tmp | grep -A 1 \"Delivered\" | tail -1 | awk {\'print \$3\'}`;

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
