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
#include <sys/time.h>
#define block_size 1450

typedef struct
{
    char received_data[block_size];
    int seq_no;
    int last_packet;
    int last;
    long pac_sent;
}receiver;
typedef struct
{
    int ack;
    int seq_no_received;
    long pac_sent_ack;
}receiver_ack;
void reliablyReceive(char* myUDPport, char* destinationFile)
{
    printf("reliablyReceive\n");
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    int bytes;
    int packets;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, myUDPport, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("listener: socket");
            continue;
        }

       if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) 
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return;
    }

    freeaddrinfo(servinfo);


    addr_len = sizeof(their_addr);
    receiver * buff = malloc(sizeof(receiver));
    receiver_ack * ans = malloc(sizeof(receiver_ack));

    int packet_number_received=0;
    printf("Opening File %s\n", destinationFile);
    FILE * output_file = fopen(destinationFile,"w");
    //char buff2[1490];
    struct timeval timer;
    gettimeofday(&timer,0);
    while(1)
    {
        printf("Reciever: waiting to recvfrom...\n");
        if ((numbytes = recvfrom(sockfd, buff, sizeof(receiver), 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        //printf("Out Of Reciever\n");
        int seq = (*buff).seq_no;
         printf("Recieved Sequence Number: %d\n", seq);
        // printf("Buff_last: %d\n", buff->last);
        // printf("Last_bytes: %d\n", buff->last_packet);
	printf("Time_sent: %ld\n", (*buff).pac_sent);

        if(seq==(packet_number_received+1) && (buff->last)!=1)
        {
            bytes+=block_size;
            packets++;
            printf("Writing To file\n");
            fwrite(buff->received_data,1,block_size,output_file);
            packet_number_received++;
        }   
        else if (seq==(packet_number_received+1) && buff->last==1)
        {
            printf("Writing To file Last\n");
            bytes+=buff->last_packet;
            packets++;
            fwrite(buff->received_data,1,buff->last_packet,output_file);

            if(buff->last==1)
            {
            printf("sending last packet ack\n");
            printf("WROTE: %d\n", bytes);
             printf("PACKET COUNT: %d\n", packets);
            (*ans).ack = -1;
            (*ans).seq_no_received = seq;
            (*ans).pac_sent_ack =  (*buff).pac_sent;

            if((numbytes = sendto(sockfd,ans,sizeof(receiver_ack),0,(struct sockaddr *)&their_addr, addr_len))== -1)
            {
                perror("recvfrom");
                exit(1);
            }
            break;
            
            }
        }
        

        (*ans).ack = packet_number_received;
        (*ans).seq_no_received = seq;
        (*ans).pac_sent_ack =  (*buff).pac_sent;
	 printf("sent_ack:%ld \n",(*ans).pac_sent_ack);

        if((numbytes = sendto(sockfd,ans,sizeof(receiver_ack),0,(struct sockaddr *)&their_addr, addr_len))== -1)
        {
            perror("recvfrom");
            exit(1);
        }
        //printf("Sent Ack For: %d\n", packet_number_received);

    }
    fclose(output_file);
    close(sockfd);
}

int main(int argc, char** argv)
{
    unsigned short int udpPort;
    
    if(argc != 3)
    {
        fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);
        exit(1);
    }
        
    reliablyReceive(argv[1], argv[2]);
}
