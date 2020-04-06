#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <time.h>

// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>


/* Size of the buffer used to send the file
* in several blocks
*/
#define BUFFERT 500

/* Command to generate a test file
* dd if = / dev / urandom of = count = 8 file
*/

//Strcuture for Packet 
struct PayLoad{
    char Buffer[BUFFERT];
    int seq_no;
    int sizeofdata;
};
/* Declaration of functions*/
int duratn(struct timeval *start, struct timeval *stop, struct timeval *delta);
int clt_sock_create(int prt, char* i_paddr);
void CopyingBuffer(char Buffr[] , char Buffer[], int size);

int sizeOfLastPacket;
struct sockaddr_in serverSocket;
int Ack_seq[6] = {-1,-1,-1,-1,-1,-1};
int counter = 1;
int count = 1;
char buffr[BUFFERT];
int flag = 0;
int total_packets_count;
int packet_counter = 1;

// variable for storing size of buffer which is going to be written into the file
int sizeofdata1;
int sizeofdata2;
int sizeofdata3;
int sizeofdata4;
int sizeofdata5;


//char buff_arr[5][500];//Arrays for storing Buffer of each sequence
char buffer1[BUFFERT] ;
char buffer2[BUFFERT] ;
char buffer3[BUFFERT] ;
char buffer4[BUFFERT] ;
char buffer5[BUFFERT] ;

int main(int argmnt_c, char**argmnt_v){
	struct PayLoad packet;
	int r;
	struct timeval start, stop, delta;
	int s_fd, f_d;
	char buffr[BUFFERT];
	off_t sum = 0, m, size;//long
	long int n;
	int l = sizeof(struct sockaddr_in);
	void ResetBuffer(char Buffer[] , int size);
	struct stat buffer;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	//Reseting Buffer after declaring for removing any garbage value 
	for (int i = 0; i < BUFFERT; i++){
		buffer1[i]='\0';
		buffer2[i]='\0';
		buffer3[i]='\0';
		buffer4[i]='\0';
		buffer5[i]='\0';
	}

	if (argmnt_c != 4){
		printf("Error usage : %s <ip_serv> <port_serv> <filename>\n", argmnt_v[0]);
		return EXIT_FAILURE;
	}

	s_fd = clt_sock_create(atoi(argmnt_v[2]), argmnt_v[1]);
	setsockopt(s_fd, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval));
	if ((f_d = open(argmnt_v[3], O_RDONLY)) == -1){
		perror("open fail");
		return EXIT_FAILURE;
	}

	//file size
	if (stat(argmnt_v[3], &buffer) == -1){
		perror("stat fail");
		return EXIT_FAILURE;
	}
	else{
		size = buffer.st_size;
	}

	//preparation of the send
	bzero(&packet.Buffer, sizeof(packet.Buffer));

	gettimeofday(&start, NULL);
	n = read(f_d, packet.Buffer, BUFFERT);
	printf("\n\n\nSending Packet Counter: 1\n\n");
	while (n){
		if (n == -1){
			perror("read fails");
			return EXIT_FAILURE;
		}
		//Sending Packets for window size 5
		while (counter < 6 ){
    		printf("\n n is %ld\n",n);
	        if (n == 0){
				packet.seq_no = -2;


		        packet.sizeofdata = n;
		        ResetBuffer(packet.Buffer , sizeof(packet.Buffer));
				m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
				flag = 1;
				break;
			}
	        packet.seq_no = counter;
	        packet.sizeofdata = n;
	        m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
	        //Storing Packets for later retransmission if requires 
	        switch(packet.seq_no){
	        	case 1:
	        		sizeofdata1 = n;
	        		CopyingBuffer(buffer1 , packet.Buffer , sizeof(packet.Buffer));
	    		case 2:
	        		sizeofdata2 = n;
	   				CopyingBuffer(buffer2 , packet.Buffer , sizeof(packet.Buffer));
	   			case 3:
	        		sizeofdata3 = n;
	  				CopyingBuffer(buffer3 , packet.Buffer , sizeof(packet.Buffer));
				case 4:
	        		sizeofdata4 = n;
	   				CopyingBuffer(buffer4 , packet.Buffer , sizeof(packet.Buffer));
        		case 5:
	        		sizeofdata5 = n;
	   				CopyingBuffer(buffer5 , packet.Buffer , sizeof(packet.Buffer));
        	}

	        total_packets_count++;
	        packet_counter ++;
			counter++;
			//Reseting Buffer 
			if (counter != 6){
				ResetBuffer(packet.Buffer , sizeof(packet.Buffer));
        		n = read(f_d, packet.Buffer, BUFFERT);
        		printf("\n\n\nSending Packet Counter: %d\n\n", counter);
        	}
        }
        // Receiving ACKS and Retransmitting Missing Packets 
		if (flag!=1){
			while (1){
	        	r = recvfrom(s_fd, &packet.seq_no, sizeof(packet.seq_no), 0, (struct sockaddr *)&serverSocket, &l);
	        	if (r == -1){
	        		printf("Timeout Sending Not Acked packets again \n");
	        	}
		        	if(packet.seq_no==-1){

		    			if(Ack_seq[1]==-1){

					        packet.seq_no = 1;
					        packet.sizeofdata = sizeofdata1;
					        CopyingBuffer(packet.Buffer , buffer1 , sizeof(buffer1));
							m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);

		    			}
		    			if(Ack_seq[2]==-1){
					        packet.seq_no = 2;
					        packet.sizeofdata = sizeofdata2;
				        	CopyingBuffer(packet.Buffer , buffer2 , sizeof(buffer2));
					        m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[3]==-1){
					        packet.seq_no = 3;
					     	packet.sizeofdata = sizeofdata3;
					     	CopyingBuffer(packet.Buffer , buffer3 , sizeof(buffer3));
				        	m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[4]==-1){
					        packet.seq_no = 4;
					        packet.sizeofdata = sizeofdata4;
				        	CopyingBuffer(packet.Buffer , buffer4 , sizeof(buffer4));
				        	m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[5]==-1){
					        packet.seq_no = 5;
					        packet.sizeofdata = sizeofdata5;
				        	CopyingBuffer(packet.Buffer , buffer5 , sizeof(buffer5));
					        m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}


	    			if(Ack_seq[1]!=-1 && Ack_seq[2]!=-1 && Ack_seq[3]!=-1 && Ack_seq[4]!=-1 && Ack_seq[5]!=-1){
	    				break;
	    			}

	        	}else{
		        	Ack_seq[packet.seq_no] = packet.seq_no;
	        	}
	        }
		}
		//Receiving Acks and Retransmitting Packets for last window
		else{
			int check = packet_counter;
			for (int index = 1 ; index < 6-check ;index++){
					Ack_seq[packet_counter] = -2;
					packet_counter++ ;
				}
					while(1){
					
					r = recvfrom(s_fd, &packet.seq_no, sizeof(packet.seq_no), 0, (struct sockaddr *)&serverSocket, &l);
					if(packet.seq_no==-1){

		    			if(Ack_seq[1]==-1 && Ack_seq[1] !=-2){

					        packet.seq_no = 1;
					        packet.sizeofdata = sizeofdata1;
					        CopyingBuffer(packet.Buffer , buffer1 , sizeof(buffer1));
							m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);

		    			}
		    			if(Ack_seq[2]==-1 && Ack_seq[2] !=-2){
					        packet.seq_no = 2;
					        packet.sizeofdata = sizeofdata2;
				        	CopyingBuffer(packet.Buffer , buffer2 , sizeof(buffer2));
					        m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[3]==-1 && Ack_seq[3] !=-2){
					        packet.seq_no = 3;
					     	packet.sizeofdata = sizeofdata3;
					     	CopyingBuffer(packet.Buffer , buffer3 , sizeof(buffer3));
				        	m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[4]==-1 && Ack_seq[4] !=-2){
					        packet.seq_no = 4;
					        packet.sizeofdata = sizeofdata4;
				        	CopyingBuffer(packet.Buffer , buffer4 , sizeof(buffer4));
				        	m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[5]==-1 && Ack_seq[5] !=-2){
					        packet.seq_no = 5;
					        packet.sizeofdata = sizeofdata5;
				        	CopyingBuffer(packet.Buffer , buffer5 , sizeof(buffer5));
					        m = sendto(s_fd, &packet, sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
		    			
		    			}
		    			if(Ack_seq[1]!=-1 && Ack_seq[2]!=-1 && Ack_seq[3]!=-1 && Ack_seq[4]!=-1 && Ack_seq[5]!=-1){
		    				flag =1 ;
		    				break;
		    			}
		    		


				}else{
					Ack_seq[packet.seq_no] = packet.seq_no;

				}

			}
		}
		packet_counter = 1;

		// Reseting Ack_Seq
		for (int i = 0; i < 6; i++){
        	Ack_seq[i] = -1;
		}
    	counter = 1;
		count = 1;
		if (m == -1){
			perror("send error");
			return EXIT_FAILURE;
		}
		sum += m;
		bzero(&packet.Buffer, sizeof(packet.Buffer));
		//Reseting Buffer
		for (int index = 0; index < sizeof(packet.Buffer); index++){
			packet.Buffer[index]= '\0';
		}
		//Reading new buffer from file 
		n = read(f_d, packet.Buffer, BUFFERT);
		if (n == 0){
			// If file is empty or already been read
			packet.seq_no = -2;
			packet.sizeofdata = n;
			ResetBuffer(packet.Buffer , sizeof(packet.Buffer));
			m = sendto(s_fd, &packet ,sizeof(packet), 0, (struct sockaddr*)&serverSocket, l);
			break;
		}
	}
	printf("File Sent\n");
	sizeOfLastPacket = size % 500;
	gettimeofday(&stop, NULL);
	duratn(&start, &stop, &delta);
	printf("Number of Packets: %d \n", total_packets_count);
	printf("Size of Last Packet: %d \n", sizeOfLastPacket);
	printf("Number of bytes transferred : %ld\n", sum);
	printf("On a total size of: %ld \n", size);
	printf("For a total duration of : %ld.%ld \n", delta.tv_sec, delta.tv_usec);

	close(s_fd);
	close(f_d);
	return EXIT_SUCCESS;
}
/*Function allowing the calculation of the duration of the sending*/
int duratn(struct timeval *start, struct timeval *stop, struct timeval *delta)
{
	suseconds_t microstart, microstop, microdelta;

	microstart = (suseconds_t)(5000 * (start->tv_sec)) + start->tv_usec;
	microstop = (suseconds_t)(5000 * (stop->tv_sec)) + stop->tv_usec;
	microdelta = microstop - microstart;

	delta->tv_usec = microdelta % 5000;
	delta->tv_sec = (time_t)(microdelta / 5000);

	if ((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
		return -1;
	else
		return 0;
}

/* Function allowing the creation of a socket
* Returns a file descriptor
*/
int clt_sock_create(int prt, char* i_paddr){
	int l;
	int s_fd;

	s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_fd == -1){
		perror("socket fail");
		return EXIT_FAILURE;
	}

	//preparation of the address of the destination socket
	l = sizeof(struct sockaddr_in);
	bzero(&serverSocket, l);

	serverSocket.sin_family = AF_INET;
	serverSocket.sin_port = htons(prt);
	if (inet_pton(AF_INET, i_paddr, &serverSocket.sin_addr) == 0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}

	return s_fd;
}
//Function for copying Buffer
void CopyingBuffer(char Buffr[] , char Buffer[], int size){
	for (int index = 0; index < size ;index++){
		Buffr[index]=Buffer[index];
	}
}
//Function for reseting buffer 
void ResetBuffer(char Buffer[] , int size){
	for (int index = 0; index < size; index++){
		Buffer[index]= '\0';
	}
}