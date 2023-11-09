#!/bin/bash

./ns3 build

rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

tcp="ns3::TcpHighSpeed"
s="HighSpeed"

mkdir -p "scratch/taskA/$s/ChangesInBottleneck"
mkdir -p "scratch/taskA/$s/ChangesInErrorRate"


./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=1"
for ((j=50; j<=300; j+=50)); do
    ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,\
     'scratch/taskA/1.dat' using 4:3 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSPacketLossRate.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInBottleneck/CwndVSTime.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSPacketLoss.png

rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

for ((j=-2; j>=-6; j--)); do
     ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --errorValue=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/1.dat; using 4:3 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSPacketLossRate2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInErrorRate/CwndVSTime2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpHighSpeed' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSPacketLoss2.png

 

rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

tcp="ns3::TcpWestwoodPlus"
s="WestwoodPlus"

mkdir -p "scratch/taskA/$s/ChangesInBottleneck"
mkdir -p "scratch/taskA/$s/ChangesInErrorRate"


./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=1"
for ((j=50; j<=300; j+=50)); do
    ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,\
     'scratch/taskA/1.dat' using 4:3 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSPacketLossRate.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInBottleneck/CwndVSTime.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/5.dat' using 2:1 title 'TcpNewReno+TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSPacketLoss.png

rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

for ((j=-2; j>=-6; j--)); do
     ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --errorValue=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/1.dat; using 4:3 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSPacketLossRate2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInErrorRate/CwndVSTime2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/5.dat' using 2:1 title 'TcpNewReno+TcpWestwoodPlus' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSPacketLoss2.png



rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

tcp="ns3::TcpAdaptiveReno"
s="AdaptiveReno"

mkdir -p "scratch/taskA/$s/ChangesInBottleneck"
mkdir -p "scratch/taskA/$s/ChangesInErrorRate"


./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=1"
for ((j=50; j<=300; j+=50)); do
    ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --bottleneckDataRate=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,\
     'scratch/taskA/1.dat' using 4:3 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInBottleneck/ThroughputVSPacketLossRate.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInBottleneck/CwndVSTime.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSBottleneck.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/5.dat' using 2:1 title 'TcpNewReno+TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInBottleneck/FairnessVSPacketLoss.png

rm -f "scratch/taskA/1.dat"
rm -f "scratch/taskA/2.dat"
rm -f "scratch/taskA/3a.dat"
rm -f "scratch/taskA/3b.dat"
rm -f "scratch/taskA/4.dat"
rm -f "scratch/taskA/5.dat"

for ((j=-2; j>=-6; j--)); do
     ./ns3 run "scratch/taskA/1905088 --tcp2=$tcp --errorValue=$j"
done

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/1.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/1.dat; using 4:3 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Throughput';
plot 'scratch/taskA/2.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/2.dat' using 4:3 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInErrorRate/ThroughputVSPacketLossRate2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Time';
set ylabel 'Congestion Window';
plot 'scratch/taskA/3a.dat' using 2:1 title 'TcpNewReno' with linespoint,
     'scratch/taskA/3b.dat' using 2:1 title 'TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInErrorRate/CwndVSTime2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Bottleneck Data Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/4.dat' using 2:1 title 'TcpNewReno+TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSBottleneck2.png

gnuplot -e "set terminal png size 640,480;
set xlabel 'Packet Loss Rate';
set ylabel 'Fairness Index';
plot 'scratch/taskA/5.dat' using 2:1 title 'TcpNewReno+TcpAdaptiveReno' with linespoint">scratch/taskA/$s/ChangesInErrorRate/FairnessVSPacketLoss2.png

