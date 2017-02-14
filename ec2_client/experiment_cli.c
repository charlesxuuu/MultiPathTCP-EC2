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
int writetext(char* filepath, char* string);
char *time_stamp();
int countline(char* filepath);

int main(int argc, char *argv[])
 {
		
  	mycall("rm -rf mp.txt");
	FILE* fd1 = fopen("log.txt", "a");
	if(fd1 != NULL){
		#ifdef DEBUG
		fprintf(fd1, "argc= %d ", argc);
		fprintf(fd1, "argv[0]= %s ", argv[0]);
		fprintf(fd1, "argv[1]= %s ", argv[1]);
		fprintf(fd1, "argv[2]= %s ", argv[2]);
		fprintf(fd1, "argv[3]= %s ", argv[3]);	
		fprintf(fd1, "argv[4]= %s ", argv[4]);
		#endif
	}

	char iperfcmd[50] = "";
	sprintf(iperfcmd, "iperf -c %s -t %s -f m |grep bits/sec >> mp.txt", argv[2], argv[3]);
	char scpcmd[100] = "";


	
	sprintf(scpcmd, "scp -i infocom_pair.pem mp.txt ubuntu@%s:~/iperfrs/mp-%s-%s.txt",argv[4], argv[1], time_stamp() );

	if(fd1 != NULL){
	#ifdef DEBUG
	fprintf(fd1, "iperfcmd= %s\n", iperfcmd);
	fprintf(fd1, "scpcmd= %s\n", scpcmd);
	#endif
	}
	fclose(fd1);



	char filepath[]="mp.txt";
	char sip[50];
	strcpy(sip, argv[1]); //sender ip 
	char rip[50];
	strcpy(rip, argv[2]); //receiver ip
	char time[50];
	strcpy(time, argv[3]); //time length
	char scheme[20];        //shift lia tcp dctcp
	char interfaces[20];    //4 2 1

	char output[100]="";
	

	//preprocessing	
	mycall("sudo ifup eth3");
	mycall("sudo ifup eth2");
	mycall("sudo ifup eth1");
	//mycall("sudo ifdown eth0");
	mycall("sudo ifup eth0");

	mycall("sudo wondershaper eth0 250000 250000");
	mycall("sudo wondershaper eth1 250000 250000");
	mycall("sudo wondershaper eth2 250000 250000");
	mycall("sudo wondershaper eth3 250000 250000");

	mycall("sudo insmod /mnt/data/mptcp/net/mptcp/mptcp_shift.ko");
	
	mycall("sudo sysctl net.mptcp.mptcp_enabled=1 >> log.txt");
	mycall("sudo sysctl net.mptcp.mptcp_path_manager='fullmesh' >> log.txt");

	
	
	//mptcp enabled (shift and lia) 4 interface	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "shift");
	strcpy(interfaces, "4");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall("sleep 2");
	mycall(iperfcmd);
	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "lia");
	strcpy(interfaces, "4");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall("sleep 2");
	mycall(iperfcmd);	


	//mptcp enabled (shift and lia) 2 interface
	mycall("sudo ifdown eth2");
	mycall("sudo ifdown eth3");
	mycall("sleep 2");

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "shift");
	strcpy(interfaces, "2");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall(iperfcmd);		
	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "lia");
	strcpy(interfaces, "2");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall(iperfcmd);		

	//mptcp disabled tcp 1 interface
	mycall("sudo ifdown eth1");
	mycall("sleep 1");
	mycall("sudo sysctl net.mptcp.mptcp_enabled=0 >> log.txt");
	 mycall("sudo sysctl net.ipv4.tcp_congestion_control='cubic' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "tcp");
	strcpy(interfaces, "1");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall(iperfcmd);		

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='dctcp' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "dc");
	strcpy(interfaces, "1");
	sprintf(output, "%s\t%s\t%s\t%s%s\t", sip, rip, time, scheme, interfaces);
	writetext(filepath, output);
	mycall(iperfcmd);


	//TODO: Send back the result:
	mycall("sleep 2");
	
	if(countline(filepath)==7){
		mycall(scpcmd);	
	}
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

int writetext(char* filepath, char* string){
	if (filepath!=NULL){
	FILE* fd = fopen(filepath, "a");
	fprintf(fd, string);
	fclose(fd);
	}
}

int countline(char* filepath){
	FILE* fd = fopen(filepath, "r");
	int ch, numlines = 0;
	
	do{
		ch = fgetc(fd);
		if(ch == '\n')
			numlines++;
	} while(ch != EOF);

	if(ch != '\n' && numlines != 0)
		numlines++;
	fclose(fd);

	return numlines;
}


char *time_stamp(){

	char *timestamp = (char *)malloc(sizeof(char) * 16);
	time_t ltime;
	ltime=time(NULL);
	struct tm *tm;
	tm=localtime(&ltime);

	sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon, 
    	tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	return timestamp;
}



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

