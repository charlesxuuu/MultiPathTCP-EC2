#!/bin/bash
# 
# Shift ENI between two instances. 
# 
# Author: Charles Xu 
# Email: xuchi.int@gmail.com
# 
# Prerequisite:  1) AWS CLI tools 2) python for processing the json data 
#
#

A=230
B=231



	
instanceid[$A]=`aws ec2 run-instances --image-id ami-d114d6b1 --count 1 --instance-type t2.medium \
		--key-name infocom_pair --security-group-ids sg-1b8b187d \
		--subnet-id subnet-72c70b2a --private-ip-address 10.3.1.$A |\
	 	python -c 'import json,sys;obj=json.load(sys.stdin);print obj["Instances"][0]["InstanceId"];'`		
echo ${instanceid[$A]}
	

	#create ENI interface
 
eniid2[$A]=`aws ec2 create-network-interface --subnet-id subnet-c4c00c9c --description "2-$A" \
		--groups sg-1b8b187d --private-ip-address 10.3.2.$A |\
		python -c 'import json,sys;obj=json.load(sys.stdin);print obj["NetworkInterface"]["NetworkInterfaceId"];'`

echo ${eniid2[$A]}


instanceid[$B]=`aws ec2 run-instances --image-id ami-d114d6b1 --count 1 --instance-type t2.medium \
		--key-name infocom_pair --security-group-ids sg-1b8b187d \
		--subnet-id subnet-72c70b2a --private-ip-address 10.3.1.$B |\
	 	python -c 'import json,sys;obj=json.load(sys.stdin);print obj["Instances"][0]["InstanceId"];'`		
echo ${instanceid[$B]}
	

#create ENI interface
 
eniid2[$B]=`aws ec2 create-network-interface --subnet-id subnet-c4c00c9c --description "2-$B" \
		--groups sg-1b8b187d --private-ip-address 10.3.2.$B |\
		python -c 'import json,sys;obj=json.load(sys.stdin);print obj["NetworkInterface"]["NetworkInterfaceId"];'`

echo ${eniid2[$B]}


sleep 20


#attach ENI interface

attachid1[$A]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$A]} --instance-id ${instanceid[$A]} --device-index 1`
echo ${attachid1[$A]}
attachid1[$B]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$B]} --instance-id ${instanceid[$B]} --device-index 1`
echo ${attachid1[$B]}
	
sleep 20

#change ENI interface
	
	
detachid1[$A]=`aws ec2 detach-network-interface --attachment-id ${attachid1[$A]}`
echo ${detachid1[$A]}	
detachid1[$B]=`aws ec2 detach-network-interface --attachment-id ${attachid1[$B]}`
echo ${detachid1[$B]}

attachid1[$A]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$B]} --instance-id ${instanceid[$A]} --device-index 1`
echo ${attachid1[$A]}	
attachid1[$B]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$A]} --instance-id ${instanceid[$B]} --device-index 1`
echo ${attachid1[$B]}	




