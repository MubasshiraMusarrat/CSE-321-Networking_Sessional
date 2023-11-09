#!/bin/bash
./ns3 build

mkdir "scratch/StaticData"

rm "scratch/static1.dat"
rm "scratch/static2.dat"

mkdir "scratch/StaticData/ChangingNoOfNodes"

output1="scratch/static1.dat"
> "$output1"
output2="scratch/static2.dat"
> "$output2"
numberOfNodes=(20 40 60 80 100)
for node in "${numberOfNodes[@]}";do
    echo -n "$node"$'\t' >> "$output1"
    echo -n "$node"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_1 --nNodes=$node --nFlows=50 --nPacketsPerSec=100 --coverageArea=1"
done

mv "scratch/static1.dat" "scratch/StaticData/ChangingNoOfNodes/Throughput.dat"
mv "scratch/static2.dat" "scratch/StaticData/ChangingNoOfNodes/Packet_Delivery_Ratio.dat"

rm "plot_script.gp"
rm "scratch/StaticData/ChangingNoOfNodes/ThroughputVSNodes.png"
rm "scratch/StaticData/ChangingNoOfNodes/PacketDeliveryRatioVSNodes.png"

cat<<EOT >>plot_script.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/StaticData/ChangingNoOfNodes/ThroughputVSNodes.png"
set xlabel "Number of Nodes"
set ylabel "Throughput"

plot "scratch/StaticData/ChangingNoOfNodes/Throughput.dat" using 1:2 title 'Throughput vs Number of Nodes' with linespoint

set output "scratch/StaticData/ChangingNoOfNodes/PacketDeliveryRatioVSNodes.png"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"

plot "scratch/StaticData/ChangingNoOfNodes/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Nodes' with linespoint
EOT

chmod u+x plot_script.gp
gnuplot plot_script.gp


mkdir "scratch/StaticData/ChangingNoOfFlows"

output1="scratch/static1.dat"
> "$output1"
output2="scratch/static2.dat"
> "$output2"
numberOfFlows=(10 20 30 40 50)
for flow in "${numberOfFlows[@]}";do
    echo -n "$flow"$'\t' >> "$output1"
    echo -n "$flow"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_1 --nNodes=20 --nFlows=$flow --nPacketsPerSec=100 --coverageArea=1"
done

mv "scratch/static1.dat" "scratch/StaticData/ChangingNoOfFlows/Throughput.dat"
mv "scratch/static2.dat" "scratch/StaticData/ChangingNoOfFlows/Packet_Delivery_Ratio.dat"

rm "plot_script2.gp"
rm "scratch/StaticData/ChangingNoOfFlows/ThroughputVSFlows.png"
rm "scratch/StaticData/ChangingNoOfFlows/PacketDeliveryRatioVSFlows.png"

cat<<EOT >>plot_script2.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/StaticData/ChangingNoOfFlows/ThroughputVSFlows.png"
set xlabel "Number of Flows"
set ylabel "Throughput"

plot "scratch/StaticData/ChangingNoOfFlows/Throughput.dat" using 1:2 title 'Throughput vs Number of Flows' with linespoint

set output "scratch/StaticData/ChangingNoOfFlows/PacketDeliveryRatioVSFlows.png"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio"

plot "scratch/StaticData/ChangingNoOfFlows/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Flows' with linespoint
EOT

chmod u+x plot_script2.gp
gnuplot plot_script2.gp


mkdir "scratch/StaticData/ChangingNoOfPackets"

output1="scratch/static1.dat"
> "$output1"
output2="scratch/static2.dat"
> "$output2"
numberOfPackets=(100 200 300 400 500)
for packet in "${numberOfPackets[@]}";do
    echo -n "$packet"$'\t' >> "$output1"
    echo -n "$packet"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_1 --nNodes=20 --nFlows=10 --nPacketsPerSec=$packet --coverageArea=1"
done

mv "scratch/static1.dat" "scratch/StaticData/ChangingNoOfPackets/Throughput.dat"
mv "scratch/static2.dat" "scratch/StaticData/ChangingNoOfPackets/Packet_Delivery_Ratio.dat"

rm "plot_script3.gp"
rm "scratch/StaticData/ChangingNoOfPackets/ThroughputVSPackets.png"
rm "scratch/StaticData/ChangingNoOfPackets/PacketDeliveryRatioVSPackets.png"

cat<<EOT >>plot_script3.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/StaticData/ChangingNoOfPackets/ThroughputVSPackets.png"
set xlabel "Number of Packets per second"
set ylabel "Throughput"

plot "scratch/StaticData/ChangingNoOfPackets/Throughput.dat" using 1:2 title 'Throughput vs Number of Packets per Second' with linespoint

set output "scratch/StaticData/ChangingNoOfPackets/PacketDeliveryRatioVSPackets.png"
set xlabel "Number of Packets per Second"
set ylabel "Packet Delivery Ratio"

plot "scratch/StaticData/ChangingNoOfPackets/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Packets per Second' with linespoint
EOT

chmod u+x plot_script3.gp
gnuplot plot_script3.gp


mkdir "scratch/StaticData/ChangingCoverageArea"

output1="scratch/static1.dat"
> "$output1"
output2="scratch/static2.dat"
> "$output2"
CoverageArea=(1 2 4 5)
for area in "${CoverageArea[@]}";do
    echo -n "$area"$'\t' >> "$output1"
    echo -n "$area"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_1 --nNodes=20 --nFlows=10 --nPacketsPerSec=100 --coverageArea=$area"
done

mv "scratch/static1.dat" "scratch/StaticData/ChangingCoverageArea/Throughput.dat"
mv "scratch/static2.dat" "scratch/StaticData/ChangingCoverageArea/Packet_Delivery_Ratio.dat"

rm "plot_script4.gp"
rm "scratch/StaticData/ChangingCoverageArea/ThroughputVSCoverage.png"
rm "scratch/StaticData/ChangingCoverageArea/PacketDeliveryRatioVSCoverage.png"

cat<<EOT >>plot_script4.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/StaticData/ChangingCoverageArea/ThroughputVSCoverage.png"
set xlabel "Coverage Area"
set ylabel "Throughput"

plot "scratch/StaticData/ChangingCoverageArea/Throughput.dat" using 1:2 title 'Throughput vs Coverage Area' with linespoint

set output "scratch/StaticData/ChangingCoverageArea/PacketDeliveryRatioVSCoverage.png"
set xlabel "Coverage Area"
set ylabel "Packet Delivery Ratio"

plot "scratch/StaticData/ChangingCoverageArea/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Coverage Area' with linespoint
EOT

chmod u+x plot_script4.gp
gnuplot plot_script4.gp
