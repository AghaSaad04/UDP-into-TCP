#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <poll.h>



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

/* Declaration of functions*/
int duratn(struct timeval *start, struct timeval *stop, struct timeval *delta);
int serverSocket(int prt);
void CopyingBuffer(char Buffr[] , char Buffer[], int size);
void ResetBuffer(char Buffer[] , int size);
int total_packets_count = 1;
//char buff_arr[5][BUFFERT];
int m;
//int column;	
char buffer1[BUFFERT] ;
char buffer2[BUFFERT] ;
char buffer3[BUFFERT] ;
char buffer4[BUFFERT] ;
char buffer5[BUFFERT] ;

int sizeofdata1;
int sizeofdata2;
int sizeofdata3;
int sizeofdata4;
int sizeofdata5;
int flag = 0;
int a = 0;
int count = 1;

int seq_arr[6] =  { -1, -1, -1, -1, -1, -1 };
int counter = 1;
struct PayLoad{
    char Buffer[BUFFERT];
    int seq;
    int sizeofdata;
};



struct sockaddr_in serverSock, client;


// New Declared
int packet_counter=1;
int missing_counter=0;
int missing_packets[5];



int main(int argmnt_c, char**argmnt_v){
	int f_d, s_fd;
	struct pollfd fd;
	int ret;
	//struct timeval tv;

	struct PayLoad packet;
	//char buffer[BUFFERT];
	off_t sum = 0, n; // long type
	char file_name[256];
	unsigned int l = sizeof(struct sockaddr_in);
	time_t intps;
	struct tm* tmi;
	int i;
	// tv.tv_sec = 3;
	// tv.tv_usec = 0;
	packet.seq = -1;

	if (argmnt_c != 2){
		printf("Error usage : %s <port_serv>\n", argmnt_v[0]);
		return EXIT_FAILURE;
	}
	s_fd = serverSocket(atoi(argmnt_v[1]));
	intps = time(NULL);
	tmi = localtime(&intps);
	bzero(file_name, 256);
	sprintf(file_name, "client.%d.%d.%d.%d.%d.%d", tmi->tm_mday, tmi->tm_mon + 1, 1900 + tmi->tm_year, tmi->tm_hour, tmi->tm_min, tmi->tm_sec);
	printf("Creating the output file : %s\n", file_name);
	if ((f_d = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0600)) == -1){
		perror("open fail");
		return EXIT_FAILURE;
	}
	bzero(&packet, sizeof(packet));
	fd.fd = s_fd;  
	fd.events = POLLIN;
	printf("events is %d\n",fd.events);
	ret = poll(&fd, 1, 10000); // 1 second for timeout
	switch (ret) {
	    case -1:
	        printf("Error");	
	        break;
	    case 0:
	        printf("Timeout"); 
	        break;
	    default:
	        printf("Receiving");
	        n = recvfrom(s_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client, &l); // get your data
	        break;
	}
	while (n){
		if (n == -1){
			perror("read fails");
			return EXIT_FAILURE;
		}
		while (packet_counter < 6){
			// packete.seq;
			// packet.Buffer;
			// packet.sizeofdata;

			printf("\n\n\nSeq received from sender: %d\n\n", packet.seq);
			if (packet.seq == -1){
			// packet.Buffer;
			
				flag = 1;
				break;
			}
			switch(packet.seq){
				case 1:
					sizeofdata1 = packet.sizeofdata;
					seq_arr[packet.seq] = packet.seq;
					for (int index = 0; index < sizeof(packet.Buffer);index++){
						buffer1[index]=packet.Buffer[index];
				}
					//CopyingBuffer(buffer1,packet.Buffer, sizeof(packet.Buffer));
				case 2:
					sizeofdata2 = packet.sizeofdata;
					seq_arr[packet.seq] = packet.seq;
					for (int index = 0; index < sizeof(packet.Buffer);index++){
						buffer2[index]=packet.Buffer[index];
					}
					//CopyingBuffer(buffer2,packet.Buffer, sizeof(packet.Buffer));
				case 3:
					sizeofdata3 = packet.sizeofdata;
					seq_arr[packet.seq] = packet.seq;
					for (int index = 0; index < sizeof(packet.Buffer);index++){
						buffer3[index]=packet.Buffer[index];
					}
					//CopyingBuffer(buffer3,packet.Buffer, sizeof(packet.Buffer));
				case 4:
					sizeofdata4 = packet.sizeofdata;
					seq_arr[packet.seq] = packet.seq;
					for (int index = 0; index < sizeof(packet.Buffer);index++){
						buffer4[index]=packet.Buffer[index];
					}
					// CopyingBuffer(buffer4,packet.Buffer, sizeof(packet.Buffer));
				case 5:
					sizeofdata5 = packet.sizeofdata;
					seq_arr[packet.seq] = packet.seq;
					for (int index = 0; index < sizeof(packet.Buffer);index++){
						buffer5[index]=packet.Buffer[index];
					}// CopyingBuffer(buffer5,packet.Buffer, sizeof(packet.Buffer));
				}
			packet_counter++;
			if(packet_counter != 6){
				ResetBuffer(packet.Buffer , sizeof(packet.Buffer));
				n = recvfrom(s_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client, &l);
				total_packets_count++;
				printf("size of data is %d\n",packet.sizeofdata );
				if (packet.seq == -2 || packet.sizeofdata == 0){
					flag = 1;
					break;
				}
			}
		}

		while (missing_counter >= 0){
			// Sending Ack to sender
			for(count =1 ; count < packet_counter ;count++){
				if(seq_arr[count]!=-1){
					packet.seq = count;
					printf("\n\n\nSequence is %d\n\n", count);
					m = sendto(s_fd, &packet.seq, sizeof(packet.seq), 0, (struct sockaddr*)&client, l);				
				}else{
					missing_counter++;
					missing_packets[missing_counter]=count;
				}
			}

			printf("\nMissing counter%d\n", missing_counter);

			packet.seq=-1;
			m = sendto(s_fd, &packet.seq, sizeof(packet.seq), 0, (struct sockaddr*)&client, l);

			// Reseting sequence array for getting missing sequence
			for (int i = 0; i < 6; i++){
				seq_arr[count]=-1;
			}

			int remaining_missing=missing_counter;

			// Recieving missing packets
			for (int i = 0; i < missing_counter; i++){
				n = recvfrom(s_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client, &l);
				switch(missing_packets[i]){
					case 1 :
						if (packet.seq == 1){
							sizeofdata1 = packet.sizeofdata;
							seq_arr[packet.seq] = packet.seq;
							for (int index = 0; index < sizeof(packet.Buffer);index++){
								buffer1[index]=packet.Buffer[index];
							}
					//CopyingBuffer(buffer1,packet.Buffer, sizeof(packet.Buffer));
							remaining_missing--;
						}
					case 2:
						if (packet.seq == 2){
							sizeofdata2 = packet.sizeofdata;
							seq_arr[packet.seq] = packet.seq;
							for (int index = 0; index < sizeof(packet.Buffer);index++){
								buffer2[index]=packet.Buffer[index];
							}
							//CopyingBuffer(buffer2,packet.Buffer, sizeof(packet.Buffer));
							remaining_missing--;
						}
					case 3:
						if (packet.seq == 3){
							sizeofdata3 = packet.sizeofdata;
							seq_arr[packet.seq] = packet.seq;
							// CopyingBuffer(buffer3,packet.Buffer, sizeof(packet.Buffer));
							for (int index = 0; index < sizeof(packet.Buffer);index++){
								buffer3[index]=packet.Buffer[index];
							}
							remaining_missing--;
					}
					case 4:
						if (packet.seq == 4){
							sizeofdata4 = packet.sizeofdata;
							seq_arr[packet.seq] = packet.seq;
							for (int index = 0; index < sizeof(packet.Buffer);index++){
								buffer4[index]=packet.Buffer[index];
							}
							// CopyingBuffer(buffer4,packet.Buffer, sizeof(packet.Buffer));
							remaining_missing--;
						}
					case 5:
						if (packet.seq == 5){
							sizeofdata5 = packet.sizeofdata;
							seq_arr[packet.seq] = packet.seq;
							for (int index = 0; index < sizeof(packet.Buffer);index++){
								buffer5[index]=packet.Buffer[index];
							}
							// CopyingBuffer(buffer5,packet.Buffer, sizeof(packet.Buffer));
							remaining_missing--;
						}
					}
				}
			missing_counter=remaining_missing;
			printf("\nNext Missing counter%d\n", missing_counter);
			if(missing_counter == 0){
				missing_counter=-1;
				printf("\nBreak\n");
				break;
			}
		}
		// Reseting the missing packets arr
		packet_counter=1;
		missing_counter=0;
		for (int i = 0; i < 5; i++){
			missing_packets[i]='\0';
		}
		for (counter = 1; counter < 6; counter ++){
			switch(counter){
				case 1:
					write(f_d, buffer1, sizeofdata1);
					// fputs(buffer1 , stdout);
					sizeofdata1 = 0;
					ResetBuffer(buffer1 , sizeof(buffer1));
				case 2:
					write(f_d, buffer2, sizeofdata2);
					sizeofdata2 = 0;
					// fputs(buffer2 , stdout);
					ResetBuffer(buffer2 , sizeof(buffer2));
				case 3:
					write(f_d, buffer3, sizeofdata3);
					sizeofdata3 = 0;
					// fputs(buffer3 , stdout);		
					ResetBuffer(buffer3 , sizeof(buffer3));
				case 4:
					write(f_d, buffer4, sizeofdata4);
					sizeofdata4 = 0;
					// fputs(buffer4 , stdout);
					ResetBuffer(buffer4 , sizeof(buffer4));
				case 5:
					write(f_d, buffer5, sizeofdata5);
					sizeofdata5 = 0;
					// fputs(buffer5 , stdout);
					ResetBuffer(buffer5 , sizeof(buffer5));
			}
		}
		if (flag == 1){
			break;
		}
		sum += n;
		total_packets_count += 1;
		for(counter = 1; counter < 6; counter ++){
			seq_arr[counter] = -1;
		}
		bzero(&packet, sizeof(packet));
		ResetBuffer(packet.Buffer , sizeof(packet.Buffer));
		n = recvfrom(s_fd, &packet, sizeof(packet), 0, (struct sockaddr *)&client, &l);
		printf("size of data is %d\n",packet.sizeofdata );
		printf("kast Packet buffer is ");
		fputs(packet.Buffer , stdout);
		if (packet.seq == -2 || packet.sizeofdata == 0){
			flag = 1;
			break;
		}
	}
	printf("Number of bytes transferred: %ld \n", sum);
	printf("Number of Packets received: %d \n", total_packets_count);

	close(s_fd);
	close(f_d);
	printf("File Received!! \n");
	return EXIT_SUCCESS;
}
	/* Function allowing the calculation of the duration of the sending*/
int duratn(struct timeval *start, struct timeval *stop, struct timeval *delta){
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

/* Function allowing the creation of a socket and its attachment to the system
* Returns a file descriptor in the process descriptor table
* bind allows its definition in the system */
int serverSocket(int prt){
	int l;
	int s_fd;

	s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_fd == -1){
		perror("socket fail");
		return EXIT_FAILURE;
	}

	//preparation of the address of the destination socket
	l = sizeof(struct sockaddr_in);
	bzero(&serverSock, l);

	serverSock.sin_family = AF_INET;
	serverSock.sin_port = htons(prt);
	serverSock.sin_addr.s_addr = htonl(INADDR_ANY);

	//Assign an identity to the socket
	if (bind(s_fd, (struct sockaddr*)&serverSock, l) == -1){
		perror("bind fail");
		return EXIT_FAILURE;
	}


	return s_fd;
}
// void CopyingBuffer(char Buffr[] , char Buffer[], int size){
// 	for (int index = 0; index < size ;index++){
// 		Buffr[index]=Buffer[index];
// 	}
	//sizeToBeWritten = sizeofdata ;
	// seq_arr[seq] = seq;
	// switch(seq){
	// 	case 1:
	// 		sizeofdata1 = sizeofdata;
	// 	case 2:
	// 		sizeofdata2 = sizeofdata;
	// 	case 3:
	// 		sizeofdata3 = sizeofdata;
	// 	case 4:
	// 		sizeofdata4 = sizeofdata;
	// 	case 5:
	// 		sizeofdata5 = sizeofdata; 
	// }

//}
void ResetBuffer(char Buffer[] , int size){
	for (int index = 0; index < size; index++){
		Buffer[index]= '\0';
	}
}
