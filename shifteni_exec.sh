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



	
instanceid[$A]=i-ff2fb322
echo ${instanceid[$A]}
 
eniid2[$A]=eni-f55598aa
echo ${eniid2[$A]}

instanceid[$B]=i-f82fb325		
echo ${instanceid[$B]}
	
eniid2[$B]=eni-25519c7a
echo ${eniid2[$B]}



#attach ENI interface

attachid1[$A]=eni-attach-e76a6d47
echo ${attachid1[$A]}
attachid1[$B]=eni-attach-c0686f60
echo ${attachid1[$B]}
	

#change ENI interface
	
	
detachid1[$A]=`aws ec2 detach-network-interface --attachment-id ${attachid1[$A]}`
echo ${detachid1[$A]}	
detachid1[$B]=`aws ec2 detach-network-interface --attachment-id ${attachid1[$B]}`
echo ${detachid1[$B]}

sleep 20

attachid1[$A]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$B]} --instance-id ${instanceid[$A]} --device-index 1`
echo ${attachid1[$A]}	
attachid1[$B]=`aws ec2 attach-network-interface --network-interface-id ${eniid2[$A]} --instance-id ${instanceid[$B]} --device-index 1`
echo ${attachid1[$B]}	


exchange interface causing  1min service down 

