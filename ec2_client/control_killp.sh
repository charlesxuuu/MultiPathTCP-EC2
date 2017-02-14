##!/bin/sh
#comments

# Performing pair iperf transmission and return data result
# 
# Author: Charles Xu 
# Email: xuchi.int@gmail.com
# 
# 
#
#


CONTROL="10.3.0.24"
TIME=10
ROUND=5

RSTART=101
REND=120

	for ((i=$RSTART; i<=$REND; i++))
	do
		ssh -f -n -i infocom_pair.pem ubuntu@10.3.1.$i "sudo pkill iperf" &
	done
	

SSTART=121
SEND=140

	for (( j=$SSTART; j<=$SEND; j++ ))
	do
		ssh -f -n -i infocom_pair.pem ubuntu@10.3.1.$j "sudo pkill iperf" &
	done
	

exit
