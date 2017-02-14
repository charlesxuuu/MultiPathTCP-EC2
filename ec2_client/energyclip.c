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

int kk=0;
for(kk=1;kk<=10;kk++){		
  	mycall("rm -rf shift4.txt shift4.dat lia4.txt lia4.dat  shift2.txt shift2.dat lia2.txt lia2.dat tcp1.txt tcp1.dat dc1.txt dc1.dat");
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

	char iperfshift4cmd[100] = "";
	sprintf(iperfshift4cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> shift4.txt  &  sudo ./real/real10 ", argv[2]);
	

	char iperflia4cmd[100] = "";
	sprintf(iperflia4cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> lia4.txt & sudo ./real/real10 ", argv[2]);

        char iperfshift2cmd[100] = "";
        sprintf(iperfshift2cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> shift2.txt & sudo ./real/real20", argv[2]);

	char iperflia2cmd[100] = "";
        sprintf(iperflia2cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> lia2.txt & sudo ./real/real20", argv[2]);


        char iperftcp1cmd[100] = "";
        sprintf(iperftcp1cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> tcp1.txt & sudo ./real/real40", argv[2]);

        char iperfdc1cmd[100] = "";
        sprintf(iperfdc1cmd, "iperf -c %s -n 128M -f m -P 10 |grep bits/sec >> dc1.txt & sudo ./real/real40", argv[2]);




	char scpshift4cmd[100] = "";
	sprintf(scpshift4cmd, "scp -i infocom_pair.pem shift4.dat ubuntu@%s:~/iperfrs/shift4-%s-%s.txt",argv[4], argv[1], time_stamp() );

	char scplia4cmd[100] = "";
        sprintf(scplia4cmd, "scp -i infocom_pair.pem lia4.dat ubuntu@%s:~/iperfrs/lia4-%s-%s.txt",argv[4], argv[1], time_stamp() );


	char scpshift2cmd[100] = "";
        sprintf(scpshift2cmd, "scp -i infocom_pair.pem shift2.dat ubuntu@%s:~/iperfrs/shift2-%s-%s.txt",argv[4], argv[1], time_stamp() );

	char scplia2cmd[100] = "";
        sprintf(scplia2cmd, "scp -i infocom_pair.pem lia2.dat ubuntu@%s:~/iperfrs/lia2-%s-%s.txt",argv[4], argv[1], time_stamp() );

	char scptcp1cmd[100] = "";
        sprintf(scptcp1cmd, "scp -i infocom_pair.pem tcp1.dat ubuntu@%s:~/iperfrs/tcp1-%s-%s.txt",argv[4], argv[1], time_stamp() );

	char scpdc1cmd[100] = "";
        sprintf(scpdc1cmd, "scp -i infocom_pair.pem dc1.dat ubuntu@%s:~/iperfrs/dc1-%s-%s.txt",argv[4], argv[1], time_stamp() );



	if(fd1 != NULL){
	#ifdef DEBUG
	//fprintf(fd1, "iperfcmd= %s\n", iperfcmd);
	//fprintf(fd1, "scpcmd= %s\n", scpcmd);
	#endif
	}
	fclose(fd1);



	//char filepath[]="mp.txt";
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

	//mycall("sudo wondershaper eth0 250000 250000");
	//mycall("sudo wondershaper eth1 250000 250000");
	//mycall("sudo wondershaper eth2 250000 250000");
	//mycall("sudo wondershaper eth3 250000 250000");

         mycall("sudo tc qdisc add dev eth0 root tbf rate 250Mbit burst 1Mb latency 0.1ms");
	 mycall("sudo tc qdisc add dev eth1 root tbf rate 250Mbit burst 1Mb latency 0.1ms");
	 mycall("sudo tc qdisc add dev eth2 root tbf rate 250Mbit burst 1Mb latency 0.1ms");
	 mycall("sudo tc qdisc add dev eth3 root tbf rate 250Mbit burst 1Mb latency 0.1ms");



	mycall("sudo insmod /mnt/data/mptcp/net/mptcp/mptcp_shift.ko");
	
	mycall("sudo sysctl net.mptcp.mptcp_enabled=1 >> log.txt");
	mycall("sudo sysctl net.mptcp.mptcp_path_manager='fullmesh' >> log.txt");


	//mptcp enabled (shift and lia) 4 interface	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "shift");
	strcpy(interfaces, "4");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall("sleep 2");
	mycall(iperfshift4cmd);
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> shift4.dat");
	mycall("rm -rf j.dat");
	mycall("sleep 5");


	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "lia");
	strcpy(interfaces, "4");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall("sleep 2");
	mycall(iperflia4cmd);	
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> lia4.dat");
	mycall("rm -rf j.dat");
	mycall("sleep 5");

	//mptcp enabled (shift and lia) 2 interface
	mycall("sudo ifdown eth2");
	mycall("sudo ifdown eth3");
	mycall("sleep 2");

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='shift' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "shift");
	strcpy(interfaces, "2");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall(iperfshift2cmd);		
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> shift2.dat");
	mycall("rm -rf j.dat");	
	mycall("sleep 5");

	
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='lia' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "lia");
	strcpy(interfaces, "2");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall(iperflia2cmd);
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> lia2.dat");
	mycall("rm -rf j.dat");
	mycall("sleep 5");

	//mptcp disabled tcp 1 interface
	mycall("sudo ifdown eth1");
	mycall("sleep 1");
	mycall("sudo sysctl net.mptcp.mptcp_enabled=0 >> log.txt");
	mycall("sudo sysctl net.ipv4.tcp_congestion_control='cubic' >> log.txt");
	mycall("sleep 2");

	//mycall("sudo tc qdisc change dev eth0 root tbf rate 25Mbit burst 1Kb latency 0.1ms");

	strcpy(scheme, "tcp");
	strcpy(interfaces, "1");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall(iperftcp1cmd);		
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> tcp1.dat");
	mycall("rm -rf j.dat");
	mycall("sleep 5");
			

	mycall("sudo sysctl net.ipv4.tcp_congestion_control='dctcp' >> log.txt");
	mycall("sleep 2");

	strcpy(scheme, "dc");
	strcpy(interfaces, "1");
	//sprintf(output, "%s\t%s\t%s\t%s%s\n", sip, rip, time, scheme, interfaces);
	//writetext(filepath, output);
	mycall(iperfdc1cmd);
	mycall("cat j.dat | awk '{ sum+=$1} END {print sum}' >> dc1.dat");
	mycall("rm -rf j.dat");
	mycall("sleep 5");


	//TODO: Send back the result:
	mycall("sleep 2");
	
	//if(countline(filepath)==7){

		mycall("head -n -1 shift4.txt >> shift4.tmp");
		mycall("head -n -1 lia4.txt >> lia4.tmp");
		mycall("head -n -1 shift2.txt >> shift2.tmp");
		mycall("head -n -1 lia2.txt >> lia2.tmp");
		mycall("head -n -1 tcp1.txt >> tcp1.tmp");
		mycall("head -n -1 dc1.txt >> dc1.tmp");


		mycall("mv shift4.tmp shift4.txt");
		mycall("mv lia4.tmp lia4.txt");
		mycall("mv shift2.tmp shift2.txt");
		mycall("mv lia2.tmp lia2.txt");
		mycall("mv tcp1.tmp tcp1.txt");
		mycall("mv dc1.tmp dc1.txt");
		

		int send=0;
		if( access( "shift4.dat", F_OK) == -1) { send=1; }
		if( access( "lia4.dat", F_OK) == -1) { send=1; }
		if( access( "shift2.dat", F_OK) == -1) { send=1; }
		if( access( "lia2.dat", F_OK) == -1) { send=1; }
		if( access( "tcp1.dat", F_OK) == -1) { send=1; }
		if( access( "dc1.dat", F_OK) == -1) { send=1; }

		if (send==0){
		mycall(scpshift4cmd);	
		mycall(scplia4cmd);
		mycall(scpshift2cmd);
		mycall(scplia2cmd);
		mycall(scptcp1cmd);
		mycall(scpdc1cmd);
		}	
	mycall("sleep 2");
}
	//}
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

