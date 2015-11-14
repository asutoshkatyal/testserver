#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/time.h>


#define block_size 1450 


#define threshold_size 1500
#define rtt 25
#define window_size_start 250
#define min_threshold 50

typedef struct sender_struct
{
	char data_read[block_size];
	int seq_no;
	int last_packet;
	int last;
	long time_sent;
}sender;

typedef struct sender_ack
{
	int ack;
	int seq_no_received;
	long time_sent_ack;
}sender_ack;

char * window;
int window_size;
int window_start;
int sockfd;
int last_received_ack;
int total_bytes;
FILE * f;
int last_packet_received;
long time_sent_packet;
long time_received_ack;
void * receive()
{
	//sleep(5);
	////printf("in receiver\n");
	sender_ack * acknow = malloc(sizeof(sender_ack));
	int number_of_bytes_sent;
	struct timeval timer;
	while(1)
	{
		//printf("inside receiver while\n");
		////printf("last_received_ack : %d\n",last_received_ack);
		if((number_of_bytes_sent = recvfrom(sockfd,acknow,sizeof(sender_ack),0,NULL,NULL))==-1)
		{
			perror("recvfrom");
        	exit(1);
		}
		////printf("ack_no received%d\n", acknow->ack);
		if(acknow->ack == -1)
		{
			//printf("last packet has been received\n");
			last_packet_received = 1;
			break;
		}
		gettimeofday(&timer,0);
		time_sent_packet = acknow -> time_sent_ack;
		time_received_ack = (timer.tv_sec * (uint64_t)1000) + (timer.tv_usec / 1000);
		//printf("Time sent packet : %ld\n",time_sent_packet);
		//printf("Time received ack : %ld\n",time_received_ack);

		last_received_ack = acknow->ack;
	}
}
void window_decrease(int amount)
{
	if(window_size-amount<=min_threshold)//min threshold
	{
		window_size=window_size;
		return;
	}
	fseek(f,-(amount*block_size),SEEK_CUR);
	window_size-=amount;
	window = realloc(window, window_size*block_size);
}
void window_increase(int amount)
{
	if(window_size+amount>=threshold_size)
	{
		window_size = window_size; 
		return;	
	}
	window = realloc(window,(window_size+amount)*block_size);
	fread(window+(window_size*block_size),1,amount*block_size,f);
	window_size+=amount;
}

void window_shift(int shift)
{
	////printf("Shifting window\n");
	////printf("SHIFT: %d\n",shift);
	////printf("WINDOW_SIZE: %d\n", window_size);
	////printf("window_start: %d\n", window_start);
	if(shift==window_size)
	{
		fread(window,1,window_size*block_size,f);
		window_start +=shift;
	}
	if(shift<window_size)
	{
		memcpy(window,window+(block_size*shift),(window_size-shift)*block_size);
		window_start+=shift;
		fread(window+((window_size-shift)*block_size),1,shift*block_size,f);
	}
	//shift > window_size?
}
void reliablyTransfer(char* hostname, char * hostUDPport, char* filename, unsigned long long int bytesToTransfer)
{
	////printf("inside reliablyTransfer\n");
	total_bytes = bytesToTransfer;
	struct addrinfo hints, *servinfo,*p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	last_received_ack=0;

	f = fopen(filename,"r");
	window_size=window_size_start;
	window_start=1;
	window = malloc(window_size*block_size);
	fread(window,1,window_size*block_size,f);


	int rv;
	if ((rv = getaddrinfo(hostname,hostUDPport, &hints, &servinfo)) != 0) 
	{
		//printf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	////printf("before for loop\n");
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

	if (p == NULL)  
	{
		//printf(stderr, "server: failed to bind\n");
		return;
	}
	
	pthread_t id;
	pthread_create(&id,0,receive,(void*)0);
	long long int count;
	
	sender * buff = (sender*)malloc(sizeof(sender));
	
	
	size_t ans; 
	int number_of_bytes_sent;
	int bytes_left;
	int no_of_packets_needed = bytesToTransfer/block_size;
	int shift = 0;
	if(bytesToTransfer % block_size!=0)
	{
		no_of_packets_needed+=1;
		bytes_left = bytesToTransfer % block_size;
	}
	//printf("No of Packets in all : %d\n", no_of_packets_needed);
	////printf("No of bytes in last packet : %d\n", bytes_left);
	////printf("No of packets needed : %d\n",no_of_packets_needed);
	////printf("last_received_ack : %d\n",last_received_ack);
	
	struct timeval timer;

	while(1)
	{
		
		if(last_packet_received==1)
		{
			//printf("whole file has been sent\n");
			break;
		}
		////printf("Inside while\n");
		/*
		if ((no_of_packets_needed - last_received_ack) == 1 )
		{
			//printf("sending last packet \n");
			fread(buff->data_read,1,bytes_left,f);	
			buff->last=1;
			buff->last_packet=bytes_left;
		}
		*/
		////printf("before for loop\n");

		if(time_received_ack - time_sent_packet  > rtt )
		{
			//printf("Window size before decrease : %d\n", window_size);
			window_decrease(window_size/3);
			//printf("Window size after decrease : %d\n", window_size);
		}
		else 
		{
			//printf("Window size before increase : %d\n", window_size);
			window_increase(1+(window_size*3/4));
			//printf("Window size after increase : %d\n", window_size);
		}
		int i;
		//printf("window size%d\n",window_size);
		for(i=0;i<window_size;i++)
		{
			memcpy(buff->data_read,window+(i*block_size),block_size);
			gettimeofday(&timer,0);
			(*buff).seq_no = window_start+i;
			(*buff).last = 0;
			(*buff).time_sent = (timer.tv_sec * (uint64_t)1000) + (timer.tv_usec / 1000);
			////printf("Packt no :%d\n",(*buff).seq_no);
			////printf("Time sent : %ld\n", (*buff).time_sent);
			////printf("Packt no :%d Time : %ld\n",(*buff).seq_no, (*buff).time_sent  );
			if((*buff).seq_no == no_of_packets_needed )
			{
				//printf("sending last packet \n");
				////printf("Seq No : %d\n", ((*buff).seq_no ));
				// fread(buff->data_read,1,bytes_left,f);	
				buff->last=1;
				buff->last_packet=bytes_left;
			}
			if((number_of_bytes_sent = sendto(sockfd,buff,sizeof(sender),0,p->ai_addr,p->ai_addrlen))==-1)
			{
				perror("recvfrom");
        		exit(1);
        	}
        	usleep(100);
        }
		////printf("within for loop\n");
		if (last_received_ack==0)
		{
			/* code */
		}
		if(last_received_ack > window_start+window_size)
		{
			printf("WAY GREATER\n");

			fseek(f,(last_received_ack+1 - window_start- window_size)*block_size,SEEK_CUR);
			window_start = last_received_ack+1;
			fread(window,1,window_size*block_size,f);
		}
		else if(last_received_ack > window_start)
		{
			if((last_received_ack - window_start)< window_size)
			{
				shift = last_received_ack - window_start;
				if(shift<0)
				{
					// //printf("last received ack %d\n",last_received_ack);
					// //printf("window_start %d\n",window_start);
				}
				//window_start = last_received_ack+1;
				window_shift(shift);
			}	
			else if ((last_received_ack - window_start)>window_size)
			{
				//window_start=last_received_ack+1;
				shift=window_size;
				window_shift(shift);
			}
		}
		else if(last_received_ack+1<window_start)
		{
			printf("I DONT EVEN KNOW WHY\n");
			printf("WINDOW START: %d\n", window_start);
			printf("last_received_ack: %d\n", last_received_ack);
			printf("WINDOW SIZE: %d\n\n\n", window_size);


			fseek(f, -((window_start-(last_received_ack+1)+window_size)*block_size),SEEK_CUR);
			window_start=last_received_ack+1;
			fread(window,1,window_size*block_size,f);
		}
		
	
	}
	free(buff);
	fclose(f);
	freeaddrinfo(servinfo); // all done with this structure
	close(sockfd);
}

int main(int argc, char** argv)
{
		unsigned long long int numBytes;	
	if(argc != 5)
	{
		//printf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
		exit(1);
	}
	numBytes = atoll(argv[4]);
	reliablyTransfer(argv[1], argv[2], argv[3], numBytes);	
}


