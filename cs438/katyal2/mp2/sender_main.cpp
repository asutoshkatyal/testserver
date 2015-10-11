#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>    
#include <sys/queue.h> 
#include <stdarg.h>  
#include <assert.h>
#include <ctime>
#include <string>
#include <cstring>
#include <thread>
#include <mutex> 
#include "sender_main.h"
#define SYN "SYN"
#define ACK "ACK"
#define END "END\n"  
#define THRESHOLD 64

char * file; 
unsigned long long int bytes; 
thread receiver;
thread sender;

int sockfd;
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
socklen_t addr_len; 
unsigned long long int lastACK;
int timeout_FLAG;
int done_FLAG;
unsigned int DUPACKctr;
unsigned long long int bytesSent; 
CongestionWindow cw; 
unsigned long long int SEQ;


void Packet::setSequenceNum(unsigned long long int num) {
    sequence_num = num;
}

unsigned long long int Packet::getSequenceNum() {
    return sequence_num;
}

int Packet::setPacketData(char * buf, unsigned int size) {
    if (buf == NULL) {
        cout << "setPacketData: Packet data buffer is NULL.\n";
        return -1;
    }
    packet_len = size;
    memcpy(data, buf, size);
    return 0;
}

void Packet::getPacketData(char * buf) {
    memcpy(buf, data, packet_len);
}

unsigned int Packet::getPacketSize() {
    return packet_len;
}



/* CONGESTION WINDOW METHODS */

void CongestionWindow::setLowestSeqNum(unsigned long long int new_num) {
    lowest_seq_num = new_num;
}

unsigned long long int CongestionWindow::getLowestSeqNum() {
    return lowest_seq_num;
}
    
void CongestionWindow::setHighestSeqNum(unsigned long long int new_num) {
    highest_seq_num = new_num;
}

unsigned long long int CongestionWindow::getHighestSeqNum() {
    return highest_seq_num;
}

void CongestionWindow::setLastACK(unsigned long long int ACK_num) {
    last_ACK = ACK_num;
}

unsigned long long int CongestionWindow::getLastACK() {
    return last_ACK;
}

void CongestionWindow::setLastSent(unsigned long long int num) {
    lastSent = num;
}

unsigned long long int CongestionWindow::getLastSent() {
    return lastSent;
}

void CongestionWindow::setWindowSize(int size) {
    window_size = size;
}

int CongestionWindow::getWindowSize() {
    return window_size;
}

void CongestionWindow::addPacket(char * buf, unsigned int size, unsigned long long int seqnum, int sockfd, struct addrinfo * p) {
    //int numbytes;
    Packet pkt;

    pkt.setSequenceNum(seqnum);
    pkt.setPacketData(buf, size);
    window.push_back(pkt); // make sure this will be FIFO
}

unsigned long long int CongestionWindow::sendWindow(int sockfd, struct addrinfo * p) {
    int numbytes;
    unsigned long long int seqnum;

    for (int i=0; i<window_size; i++) {
        seqnum = window[i].getSequenceNum();
        char data[MAX_DATA_SIZE];
        window[i].getPacketData(data);
        unsigned int size = window[i].getPacketSize();

        string seq_num = to_string(seqnum);
        int seq_num_size = seq_num.size();
        int pkt_size = size + seq_num_size + 1; //data + sequence num + new line
        char msg[pkt_size];
        //char new_msg[pkt_size];

        char seq_num_str[seq_num_size];
        strcpy(seq_num_str, seq_num.c_str());
        strcat(seq_num_str,"\n");

        memcpy(msg, seq_num_str, seq_num_size + 1);
        memcpy(msg + seq_num_size + 1, data, size);

        //cout << "Sending packet: " << msg << "\n";
        cout << "*PKT" << seqnum << " -->\n";
        if ((numbytes = sendto(sockfd, msg, pkt_size, 0, p->ai_addr, p->ai_addrlen)) == -1) {
            perror("sender: sendto");
            exit(1);
        }   
        //lastSent = seqnum;
        //cout << "lastSent =" << lastSent << endl;
    }
    return seqnum;
}

unsigned long long int CongestionWindow::sendPacket(int index, int sockfd, struct addrinfo * p) {
    Packet pkt = window[index];
    int numbytes;

    unsigned long long int seqnum = pkt.getSequenceNum();
    //cout << "PKT" << seqnum << endl;
    char data[MAX_DATA_SIZE];
    pkt.getPacketData(data);
    unsigned int size = pkt.getPacketSize();

    string seq_num = to_string(seqnum);
    int seq_num_size = (int)seq_num.size();
    int pkt_size = size + seq_num_size + 1; //data + sequence num + new line
    char msg[pkt_size];
    char seq_num_str[seq_num_size];
    strcpy(seq_num_str, seq_num.c_str());
    strcat(seq_num_str,"\n");

    memcpy(msg, seq_num_str, seq_num_size + 1);
    memcpy(msg + seq_num_size + 1, data, size);

    if ((numbytes = sendto(sockfd, msg, pkt_size, 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sender: sendto");
        exit(1);
    }   
    //cout << "PKT" << seqnum << endl;
    //cout << "lastSent = " << lastSent << endl;
    //lastSent = seqnum;
    return seqnum;
}

void CongestionWindow::removePackets(int n) {
    for(int i=0; i < n; i++) {
        window.pop_front();
    }
}

int CongestionWindow::getNumPktsToAdd() {
    if(window.empty()){
        return window_size;
    }
    else {
        //cout << "num_pkts = " << window_size - (int)window.size() << endl;
        return window_size - (int)window.size(); // number of packets that need to be added
    }
}

void CongestionWindow::cutWindow(){
    if (window_size / 2 == 0) {
        window_size = 1;
    }
    else {
        window_size = window_size / 2;
    }
}

void CongestionWindow::panicMode() {
    window_size = 1;
}
void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer);

int main(int argc, char** argv)
{
	unsigned short int udpPort;
	unsigned long long int numBytes;
	
	if(argc != 5)
	{
		fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
		exit(1);
	}
	udpPort = (unsigned short int)atoi(argv[2]);
	numBytes = atoll(argv[4]);
	
	reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
} 
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void setup_UDP(char * hostname, unsigned short int port) {
	struct addrinfo hints, *servinfo;//, *p;
    int rv;
    //int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    string port_str = to_string(port);
    if ((rv = getaddrinfo(hostname, port_str.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return;
    }
    

} 

int initialize_TCP() {
	char buf[3]; //store 

    int numbytes;

    cout << "SYN -->\n";
    if ((numbytes = sendto(sockfd, SYN, 3, 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sender: sendto");
        exit(1);
    }
    
    if ((numbytes = recvfrom(sockfd, buf, 3, 0,
            p->ai_addr, (socklen_t *)&p->ai_addrlen)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    if (memcmp(buf,"ACK",3) != 0) {
        cout << "Received: " << buf << ".\n";
        return -1;
    }
    else {
        cout << "ACK <--\n";
    }

    return 0; // return estimated timeout
}  

unsigned long long int getACKnum(char * msg) {
    // strip the ACK number from message
    unsigned long long int num;
    sscanf(msg, "%llu", &num);
    return num;
}
void receiveACKs() {
    // listen for ACKs
    // set a timeout on the recvfrom syscall
    int numbytes, timeout;
    char buf[MAX_DATA_SIZE];

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 30000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }
    int running = 1;
    while (running) {
        timeout = 0;
        if ((numbytes = recvfrom(sockfd, buf, MAX_DATA_SIZE, 0,
                p->ai_addr, (socklen_t *)&p->ai_addrlen)) == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                timeout = 1;
            }
            else {
                exit(1);
            }
        }

        if(timeout) {
            
            timeout_FLAG = 1;
      
        }
        else {
            // we got an ACK

            if (memcmp(buf,END,4) == 0) {
                return;
            }
            unsigned long long int ACKnum = getACKnum(buf);
            // check if DUPACK

            if (ACKnum == lastACK) {
              
                DUPACKctr++;
                bytesSent = ACKnum * 1024;
            }
            else if (ACKnum > lastACK) {
                lastACK = ACKnum;
                //cout << "lastACK = " << lastACK << endl;
                //cout << "lastSent = " << cw.getLastSent() << endl;
                DUPACKctr = 0;
               // if(done_FLAG && (lastACK == cw.getLastSent())) {     Have to do something for this condition
                 //   running = 0;
                //}
                bytesSent = ACKnum * 1024;
               
            }
        }
    }

} 

void sendPackets() 
{ 
    unsigned long long int bytesRead = 0;
    bool slowStart = true;
    int partialPacket = 0; 

    

    ifstream myFile(file);
    if(!myFile.is_open()) {
        cout << "reliablyTransfer: Unable to open file.\n";
        return;
    }

    while (1) {
        //cout << "bytes = " << bytes << endl;
        //cout << "bytesRead = " << bytesRead << endl;
       
        if(bytesSent >= bytes) {
        
            break;
        }
       
        int numPktsToAdd, numPktsToSend, index, packet_size;

        numPktsToAdd = cw.getNumPktsToAdd();

        if(numPktsToAdd < 0) {
            numPktsToAdd = 0;
        }

        char buf[MAX_DATA_SIZE];
        for(int i=0; i<numPktsToAdd; i++) {
            //if (bytesRead >= bytes) { break; }

            unsigned long long int diff = bytes - bytesRead;
            myFile.read(buf, min(diff,(unsigned long long int)MAX_DATA_SIZE));
            bytesRead += min(diff,(unsigned long long int)MAX_DATA_SIZE);

            if(diff > 0 && diff < MAX_DATA_SIZE) { 
                packet_size = diff; 
            }
            else { 
                packet_size = MAX_DATA_SIZE; 
            }

            cw.addPacket(buf, packet_size, SEQ, sockfd, p); // this adds packets and sends them!
            cw.setHighestSeqNum(SEQ);
            SEQ++;
        }

        unsigned long long int lastSent = cw.getLastSent();
        unsigned long long int low = cw.getLowestSeqNum();
        numPktsToSend = (low + cw.getWindowSize()) - lastSent - 1;
        index = lastSent - low + 1;
        //cout << "index = " << index << endl;
        for(int i=index; i < cw.getWindowSize(); i++) {
            cw.sendPacket(i, sockfd, p);
        }

        

        // read file into packets and place in cw vector

        // CRITICAL SECTION
       // to read.
        if (timeout_FLAG) {
            cw.panicMode();
            slowStart = true;
            timeout_FLAG = 0;
        }
        else {
            //timed out. reset CW to 1

            if(lastACK == cw.getLastACK()) {
                //cout << "DUPACK!\n";
                if(DUPACKctr >= 5) { // we're getting DUPACKs
                    //cout << "window_size = " << cw.getWindowSize() << endl;
                    cw.cutWindow();
                    unsigned long long int newSEQ = cw.sendWindow(sockfd, p); // resend the window!
                    //cout << "resending window!\n";
                    SEQ = newSEQ + 1;
                    DUPACKctr = 0;
                }
                //cw.setHighestSeqNum(cw.getLowestSeqNum + cw.getWindowSize() - 1);
            }
            else if (lastACK > cw.getLastACK()) {
                int ws=0;
                //cout << "lastACK = " << lastACK << endl;
                cw.setLastACK(lastACK);
                int diff = lastACK - cw.getLowestSeqNum() + 1;
                cw.removePackets(diff); // pop off ACKd packets
                
                // increase congestion window
                if(slowStart){ws = cw.getWindowSize() + 1;}
                else{
                    partialPacket++;
                    if(partialPacket == cw.getWindowSize()) {ws = cw.getWindowSize() + 1;}
                }
                cw.setWindowSize(ws);
                if(ws >= THRESHOLD) {
                    slowStart = false; // break out of slow start
                }
                unsigned long long int new_low = lastACK + 1;
                //unsigned long long int new_high = ws + new_low - 1;
                cw.setLowestSeqNum(new_low);
                //cw.setHighestSeqNum(new_high);
            }
            else {
                // got an earlier ACK - we don't care
            }
        }
       
        // END CRITICAL SECTION
    }
    cout << "final PKT = " << SEQ << endl;
    cout << "bytesRead = " << bytesRead << endl;
    cout << "Sending termination packet!\n";
    int numbytes;
    if ((numbytes = sendto(sockfd, END, 4, 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sender: sendto");
        exit(1);
    }
   
    done_FLAG = 1;
 
}

void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer) {
    bytes = bytesToTransfer;

    file = filename;

    // set up UDP socket
    setup_UDP(hostname, hostUDPport);

    // initialize TCP connection
    if(initialize_TCP() == -1) {
        printf("%s\n","reliablyTransfer: Could not complete TCP handshake.\n" );
        return;
    } 
    sender = thread(sendPackets); 
    receiver = thread(receiveACKs); 

    sender.join(); 
    receiver.join(); 

    close(sockfd); 
    return;
}