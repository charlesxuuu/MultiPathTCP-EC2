#include<stdio.h>


int countline(char* filepath);


int main(){
	char filepath[]="mp.txt";	
	printf("lines: %d \n",  countline(filepath));

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

