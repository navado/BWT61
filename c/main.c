#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

float a[3],w[3],angle[3],T;
char Re_buf[11];
unsigned char sign;
unsigned char sum;
int can_run = 1;
char separator =  '\t';


void sig_handler(int signo)
{
  if (signo == SIGINT)
    fprintf(stderr,"\n\nIinterrupted - Stopping\n");
    can_run = 0;
}

void parseBuffer(){
    if(Re_buf[0]==0x55)
    {
        switch(Re_buf [1])
        {
          case 0x51:
              a[0] = ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*16;
              a[1] = ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*16;
              a[2] = ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*16;
              break;
          case 0x52:
              w[0] = ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*2000;
              w[1] = ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*2000;
              w[2] = ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*2000;
              break;
          case 0x53:
              angle[0] = ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
              angle[1] = ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
              angle[2] = ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
              break;
            default:
                printf("unknown word %x\n",Re_buf[1]);
                break;
        }
        T = ((short)(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;
        sum = Re_buf[10];
    }
}

int checksum(){
    int res = 0;
    int i = 0;
    for(;i<11;i++){
        res += Re_buf[i];
    }
    return res == Re_buf[11];
}
void printData(int iter){
    fprintf(stderr,"===== Data frame %d sum=%d %s =======\r",iter,sum,checksum()==0?"Correct":"Corrupted!!!");
    float * arr;
    char * mark;
    switch(Re_buf [1])
    {
        case 0x51:
            mark = "A"; arr = a;
            break;
        case 0x52:
            mark = "W"; arr = w;
            break;
        case 0x53:
            mark = "D"; arr = angle;
            break;
    }

    printf("%s%c",mark,separator);
    for(int i = 0;i<3;i++)
        printf("%0.3f%c",arr[i],separator);
    printf("%0.2f%c%d \n",T,separator,checksum());
}
void main()
{
    int counter = 0;
    int frame_no = 0;
    // Register to SIGINT (Ctrl+C)
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("can't catch SIGINT - Continue without\n");
    //scroll input data untill first data marker (01010101)
    while((Re_buf [counter] = getchar()) != 0x55){
        if(!can_run)
            exit(0);
    }
    fprintf(stderr,"Stream Synchronized.\n");
    counter++;
    while(can_run)
    {
        Re_buf [counter++] = getchar();
        if(counter==11){
            parseBuffer();
            printData(frame_no++);
            counter=0;
        }
    }
}