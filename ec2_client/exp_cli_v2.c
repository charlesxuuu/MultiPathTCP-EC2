#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define DEBUG 


int mycall(const char * cmdstring);
void error (const char * msg);


 int main(int argc, char *argv[])
 {
		
	mycall("rm -rf log.txt mp.txt");
	FILE* fd = fopen("/home/ubuntu/log.txt", "w");
	if(fd != NULL){
		#ifdef DEBUG
		fprintf(fd, "argc= %d ", argc);
		fprintf(fd, "argv[0]= %s ", argv[0]);
		fprintf(fd, "argv[1]= %s ", argv[1]);
		fprintf(fd, "argv[2]= %s ", argv[2]);
		#endif
	}
	char iperfcmd[50] = "";
	//iperf -c 10.3.1.180 -t 5 |grep Gbits/sec |awk '{print $8 $9}'
	sprintf(iperfcmd, "iperf -c %s -t %s |grep bits/sec |awk '{print $8 $9}' >>/home/ubuntu/mp.txt", argv[1], argv[2]);

	if(fd != NULL){
	#ifdef DEBUG
	fprintf(fd, "iperfcmd= %s", iperfcmd);
	#endif
	}
	fclose(fd);

	
	//preprocessing	
	mycall("sudo ifup eth3");
	mycall("sudo ifup eth2");
	mycall("sudo ifup eth1");
	mycall("sudo ifdown eth0");
	mycall("sudo ifup eth0");

	mycall("sudo wondershaper eth0 250000 250000");
	mycall("sudo wondershaper eth1 250000 250000");
	mycall("sudo wondershaper eth2 250000 250000");
	mycall("sudo wondershaper eth3 250000 250000");

	mycall("sudo insmod /mnt/data/mptcp/net/mptcp/mptcp_shift.ko");
	
	mycall("sudo sysctl net.mptcp.mptcp_enabled=1 >> /home/ubuntu/log.txt");
	mycall("sudo sysctl net.mptcp.mptcp_path_manager='fullmesh' >> /home/ubuntu/log.txt");

	
	
	//mptcp enabled (shift and lia) 4 interface	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);		
	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);		


	//mptcp enabled (shift and lia) 2 interface
	mycall("sudo ifdown eth2");
	mycall("sudo ifdown eth3");
	mycall("sleep 2");

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);		
	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);		

	//mptcp disabled tcp 1 interface
	mycall("sudo ifdown eth1");
	mycall("sleep 1");
	mycall("sudo sysctl net.mptcp.mptcp_enabled=0 >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);		

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='dctcp' >> /home/ubuntu/log.txt");
	mycall("sleep 2");
	mycall(iperfcmd);

     	return 0;
}

int mycall(const char * cmdstring){
    pid_t pid;
    int status;

	if(cmdstring == NULL){
    		return (1); //if cmdstring is null，return 1
	}

	if((pid = fork())<0){
    		status = -1; //failed fork，return -1
	}
	else if(pid == 0){ //child process
    		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
    		_exit(127); // if execl failed, return 127
	}
	else{ //father process
    		while(waitpid(pid, &status, 0) < 0){
        		if(errno != EINTR){
            			status = -1; //If waitpid is interrupted，return -1
            			break;
        		}
    		}
	}
    return status; //If waitpid succeed，return status
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

