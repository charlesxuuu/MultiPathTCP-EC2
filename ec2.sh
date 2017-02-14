#!/bin/bash
# 
# Sequential create Amazon EC2 instances, then create and attach ENI. 
# 
# Author: Charles Xu 
# Email: xuchi.int@gmail.com
# 
# Prerequisite:  1) AWS CLI tools 2) python for processing the json data 
#
#




#define IP range of the experimental machine

START=101
END=140

for (( c=$START; c<=$END; c++ ))
do
	#echo -n "$c "
	#create ec2 instances  !!no space between newinstance and '='
	instanceid[$c]=`aws ec2 run-instances --image-id ami-aa19d5ca --count 1 --instance-type c4.xlarge \
			--key-name infocom_pair --security-group-ids sg-1b8b187d \
			--subnet-id subnet-72c70b2a --private-ip-address 10.3.1.$c |\
	 		python -c 'import json,sys;obj=json.load(sys.stdin);print obj["Instances"][0]["InstanceId"];'`		
	echo ${instanceid[$c]}
	

	#create ENI interface
 
	eniid2[$c]=`aws ec2 create-network-interface --subnet-id subnet-c4c00c9c --description "2-$c" \
			--groups sg-1b8b187d --private-ip-address 10.3.2.$c |\
			python -c 'import json,sys;obj=json.load(sys.stdin);print obj["NetworkInterface"]["NetworkInterfaceId"];'`


	echo ${eniid2[$c]}


	eniid3[$c]=`aws ec2 create-network-interface --subnet-id subnet-ee06c9b6 --description "3-$c" \
			--groups sg-1b8b187d --private-ip-address 10.3.3.$c |\
			python -c 'import json,sys;obj=json.load(sys.stdin);print obj["NetworkInterface"]["NetworkInterfaceId"];'`

	echo ${eniid3[$c]}

	eniid4[$c]=`aws ec2 create-network-interface --subnet-id subnet-908c42c8 --description "4-$c" \
			--groups sg-1b8b187d --private-ip-address 10.3.4.$c |\
			python -c 'import json,sys;obj=json.load(sys.stdin);print obj["NetworkInterface"]["NetworkInterfaceId"];'`

	echo ${eniid4[$c]}
	
done

sleep 20

#attach ENI interface
for (( c=$START; c<=$END; c++ ))
do
	aws ec2 attach-network-interface --network-interface-id ${eniid2[$c]} --instance-id ${instanceid[$c]} --device-index 1

	aws ec2 attach-network-interface --network-interface-id ${eniid3[$c]} --instance-id ${instanceid[$c]} --device-index 2

	aws ec2 attach-network-interface --network-interface-id ${eniid4[$c]} --instance-id ${instanceid[$c]} --device-index 3

done


