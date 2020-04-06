						UDP File Transfer Program
Introduction:
The project is composed of two folder representing the two way to send a file using the local network. User have to give arguments to the program to use it: IP Address, Port No ,File to send.

Prerequisites:
A UNIX distribution and a C compiler: gcc.

How to compile the application:
* Navigate to the right folder UDP or FTP with a console.
* Enter :
gcc –o <object file name you want to create> <.c file you want to compile>
Example:
gcc –o receiver receiver.c

How to Run the application:
Example
./sender <IP-server> <port no> <filetosend>
./receiver <port no>


