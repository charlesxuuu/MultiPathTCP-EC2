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
TIME=30
ROUND=300

RSTART=101
REND=120

	for ((i=$RSTART; i<=$REND; i++))
	do
		ssh -f -n -i infocom_pair.pem ubuntu@10.3.1.$i "./experiment_ser" &
	done
	
	echo "receiver ok sleep 60s"
	sleep 60

SSTART=121
SEND=140

	#time wise r=round
	for ((r=1; r<=$ROUND; r++))
	do
		#configuration wise l=loop
		for ((l=1; l<=$[$SEND-$SSTART+1]; l++))
		do	
			for (( j=$SSTART; j<=$SEND; j++ ))
			do
				echo $j
				
				target=$[$RSTART+ (($j+$l-1)-$SSTART)%($REND-$RSTART+1)]
				
				echo $target
				ssh -f -n -i infocom_pair.pem ubuntu@10.3.1.$j "./experiment_clip 10.3.1.$j 10.3.1.$target $TIME $CONTROL " &
			done
			
			echo "sender ok "
			#wait until the loop end	
       			sleep $[$TIME*6+100]
		done
	done

	
#TODO: Process the file

exit
