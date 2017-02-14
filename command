sip="10.3.1.180"

sip=$(ifconfig |head -n 2 |grep inet| awk -F "[: \t]" '{print $13 }')

cip="10.3.1.181" #localhost
now=$(date +"%T")
result=$(iperf -c $sip -t 5 |grep bits/sec |awk '{print $8 $9}')


10.3.1.180 10.3.1.181 10:23:22 shift 4 250Mbits/sec

cat * | wc -l
