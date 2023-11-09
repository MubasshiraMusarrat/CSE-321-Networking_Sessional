#!/bin/bash
./ns3 build

mkdir "scratch/MobileData"

rm "scratch/mobile1.dat"
rm "scratch/mobile2.dat"

mkdir "scratch/MobileData/ChangingNoOfNodes"

output1="scratch/mobile1.dat"
> "$output1"
output2="scratch/mobile2.dat"
> "$output2"
numberOfNodes=(20 40 60 80 100)
for node in "${numberOfNodes[@]}";do
    echo -n "$node"$'\t' >> "$output1"
    echo -n "$node"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_2 --nNodes=$node --nFlows=50 --nPacketsPerSec=100 --Speed=5"
done

mv "scratch/mobile1.dat" "scratch/MobileData/ChangingNoOfNodes/Throughput.dat"
mv "scratch/mobile2.dat" "scratch/MobileData/ChangingNoOfNodes/Packet_Delivery_Ratio.dat"

rm "plot_script5.gp"
rm "scratch/MobileData/ChangingNoOfNodes/ThroughputVSNodes.png"
rm "scratch/MobileData/ChangingNoOfNodes/PacketDeliveryRatioVSNodes.png"

cat<<EOT >>plot_script5.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/MobileData/ChangingNoOfNodes/ThroughputVSNodes.png"
set xlabel "Number of Nodes"
set ylabel "Throughput"

plot "scratch/MobileData/ChangingNoOfNodes/Throughput.dat" using 1:2 title 'Throughput vs Number of Nodes' with linespoint

set output "scratch/MobileData/ChangingNoOfNodes/PacketDeliveryRatioVSNodes.png"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"

plot "scratch/MobileData/ChangingNoOfNodes/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Nodes' with linespoint
EOT

chmod u+x plot_script5.gp
gnuplot plot_script5.gp


mkdir "scratch/MobileData/ChangingNoOfFlows"

output1="scratch/mobile1.dat"
> "$output1"
output2="scratch/mobile2.dat"
> "$output2"
numberOfFlows=(10 20 30 40 50)
for flow in "${numberOfFlows[@]}";do
    echo -n "$flow"$'\t' >> "$output1"
    echo -n "$flow"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_2 --nNodes=20 --nFlows=$flow --nPacketsPerSec=100 --Speed=5"
done

mv "scratch/mobile1.dat" "scratch/MobileData/ChangingNoOfFlows/Throughput.dat"
mv "scratch/mobile2.dat" "scratch/MobileData/ChangingNoOfFlows/Packet_Delivery_Ratio.dat"

rm "plot_script6.gp"
rm "scratch/MobileData/ChangingNoOfFlows/ThroughputVSFlows.png"
rm "scratch/MobileData/ChangingNoOfFlows/PacketDeliveryRatioVSFlows.png"

cat<<EOT >>plot_script6.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/MobileData/ChangingNoOfFlows/ThroughputVSFlows.png"
set xlabel "Number of Flows"
set ylabel "Throughput"

plot "scratch/MobileData/ChangingNoOfFlows/Throughput.dat" using 1:2 title 'Throughput vs Number of Flows' with linespoint

set output "scratch/MobileData/ChangingNoOfFlows/PacketDeliveryRatioVSFlows.png"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio"

plot "scratch/MobileData/ChangingNoOfFlows/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Flows' with linespoint
EOT

chmod u+x plot_script6.gp
gnuplot plot_script6.gp


mkdir "scratch/MobileData/ChangingNoOfPackets"

output1="scratch/mobile1.dat"
> "$output1"
output2="scratch/mobile2.dat"
> "$output2"
numberOfPackets=(100 200 300 400 500)
for packet in "${numberOfPackets[@]}";do
    echo -n "$packet"$'\t' >> "$output1"
    echo -n "$packet"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_2 --nNodes=20 --nFlows=10 --nPacketsPerSec=$packet --Speed=5"
done

mv "scratch/mobile1.dat" "scratch/MobileData/ChangingNoOfPackets/Throughput.dat"
mv "scratch/mobile2.dat" "scratch/MobileData/ChangingNoOfPackets/Packet_Delivery_Ratio.dat"

rm "plot_script7.gp"
rm "scratch/MobileData/ChangingNoOfPackets/ThroughputVSPackets.png"
rm "scratch/MobileData/ChangingNoOfPackets/PacketDeliveryRatioVSPackets.png"

cat<<EOT >>plot_script7.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/MobileData/ChangingNoOfPackets/ThroughputVSPackets.png"
set xlabel "Number of Packets per second"
set ylabel "Throughput"

plot "scratch/MobileData/ChangingNoOfPackets/Throughput.dat" using 1:2 title 'Throughput vs Number of Packets per Second' with linespoint

set output "scratch/MobileData/ChangingNoOfPackets/PacketDeliveryRatioVSPackets.png"
set xlabel "Number of Packets per Second"
set ylabel "Packet Delivery Ratio"

plot "scratch/MobileData/ChangingNoOfPackets/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Number of Packets per Second' with linespoint
EOT

chmod u+x plot_script7.gp
gnuplot plot_script7.gp


mkdir "scratch/MobileData/ChangingSpeedOfNodes"

output1="scratch/mobile1.dat"
> "$output1"
output2="scratch/mobile2.dat"
> "$output2"
Speed=(5 10 15 20 25)
for sp in "${Speed[@]}";do
    echo -n "$sp"$'\t' >> "$output1"
    echo -n "$sp"$'\t' >> "$output2"
    ./ns3 run "scratch/1905088_2 --nNodes=20 --nFlows=10 --nPacketsPerSec=100 --Speed=$sp"
done

mv "scratch/mobile1.dat" "scratch/MobileData/ChangingSpeedOfNodes/Throughput.dat"
mv "scratch/mobile2.dat" "scratch/MobileData/ChangingSpeedOfNodes/Packet_Delivery_Ratio.dat"

rm "plot_script8.gp"
rm "scratch/MobileData/ChangingSpeedOfNodes/ThroughputVSSpeed.png"
rm "scratch/MobileData/ChangingSpeedOfNodes/PacketDeliveryRatioVSSpeed.png"

cat<<EOT >>plot_script8.gp
#!/usr/bin/gnuplot

set terminal png size 640,480
set output "scratch/MobileData/ChangingSpeedOfNodes/ThroughputVSSpeed.png"
set xlabel "Speed of Nodes"
set ylabel "Throughput"

plot "scratch/MobileData/ChangingSpeedOfNodes/Throughput.dat" using 1:2 title 'Throughput vs Speed of Nodes' with linespoint

set output "scratch/MobileData/ChangingSpeedOfNodes/PacketDeliveryRatioVSSpeed.png"
set xlabel "Speed of Nodes"
set ylabel "Packet Delivery Ratio"

plot "scratch/MobileData/ChangingSpeedOfNodes/Packet_Delivery_Ratio.dat" using 1:2 title 'Packet Delivery Ratio vs Speed of Nodes' with linespoint
EOT

chmod u+x plot_script8.gp
gnuplot plot_script8.gp
 
