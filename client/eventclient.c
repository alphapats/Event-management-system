/*
FILE NAME: FILECLIENT.C
CREATED BY:163054001 AMIT PATHANIA
This files create a peer. File takes three commandline input arguments server-ip,server port and own listning port.
Server IP and port will be used to connect the server and publish and search files.
Own listening port will be used to listen to incoming fetch requests fom other peers.
The code has following main operations:
JOIN : Connects the peer server
PUBLISH : Publish file to server for sharing with filename,filepath and own port no.
SEARCH : Search file using keyword from server.
FETCH :Fetch desired file from client using client IP,Port no, filename and path.
Also handle incoming file fetch requests from other clients.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<arpa/inet.h>
#include <signal.h>

#define ERROR     -1			//returns -1 on error as message
#define BUFFER    512      //this is max size of input and output buffer used to store send and recieve data
//#define LISTENING_PORT 
#define MAX_CLIENTS    4     //defines maximum number of clients for listening


int main(int argc, char **argv)  //IP and port mentioned
{
	int sock; // sock is socket desriptor for connecting to remote server 
	struct sockaddr_in remote_server; // contains IP and port no of remote server
	char input[BUFFER];  //user input stored 
	char output[BUFFER]; //recd from remote server
	int len;//to measure length of recieved input stram on TCP
	char *temp; // variable to store temporary values
	int choice;//to take user input
		

	//variables declared for fetch operation
	char file_fet[BUFFER];//store file name keyword to be fetched
	char peer_ip[BUFFER];//store IP address of the peer for connection
	char peer_port[BUFFER];//store port no of the peer for fetch
	struct sockaddr_in peer_connect; // contains IP and port no of desired peer for fetch
	int peer_sock; // socket descriptor for peer during fetch

	
		
	//client takes two arguements SERVER IP ADDRESS,SERVER PORT 
	if (argc < 2)    // check whether port number provided or not
	{ 
		fprintf(stderr, "ERROR, ENTER SERVER IP ADDRESS AND PORT \n");
		exit(-1);
	}

	//for connecting with server for publishing and search files
	if ((sock= socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{ 
		perror("socket");  // error checking the socket
		exit(-1);  
	} 
	  
	remote_server.sin_family = AF_INET; // family
	remote_server.sin_port =htons(atoi(argv[2])); // Port No and htons to convert from host to network byte order. atoi to convert asci to 		integer
	remote_server.sin_addr.s_addr = inet_addr(argv[1]);//IP addr in ACSI form to network byte order converted using inet
	bzero(&remote_server.sin_zero, 8); //padding zeros
	
	if((connect(sock, (struct sockaddr *)&remote_server,sizeof(struct sockaddr_in)))  == ERROR) //pointer casted to sockaddr*
	{
		perror("connect");
		exit(-1);
	}
	printf("%s","Connected to server\t");

	//setting up own port for listening incoming connections for fetch

	while(1)
	{
		//DISPLAY MENU FOR USER INPUTS
		printf("\n#####################WELCOME#########################################\n");
		printf("##################EVENT MANAGEMENT SYSTEM############################ \n");
		printf("1.NEW USER\n");
		printf("2.GET EVENTS\n");
		printf("3.CREATE EVENTS\n");
		printf("4.TERMINATE THE CONNECTION TO SERVER\n");
		printf("\n############################################################## \n");
		printf("Enter your choice : ");
		if(scanf("%d",&choice)<=0)
			{
	    	printf("Enter only an integer from 1 to 4\n");
	    	exit(0);
			} 
		else
			{
	   		switch(choice)
				{
				case 1:  //ADD NEW USER 
					
					temp="new"; // keyword to be send to server so that server knows it is a publish operation

					send(sock, temp, sizeof(temp) ,0); // send input to server
					
					printf("Enter the username     ");
		   			//fgets(input,BUFFER,stdin); //take input from user
					scanf(" %[^\t\n]s",input); //recieve user input
					send(sock, input, strlen(input) ,0); // send input to server
					len = recv(sock, output, BUFFER, 0); // recieve confirmation message from server
					output[len] = '\0';
					printf("%s\n" , output); // display confirmation message
					bzero(output,BUFFER); // pad buffer with zeros

					printf("Enter the password     ");
		   			//fgets(input,BUFFER,stdin); //take input from user
					scanf(" %[^\t\n]s",input); //recieve user input
					send(sock, input, strlen(input) ,0); // send input to server
					len = recv(sock, output, BUFFER, 0); // recieve confirmation message from server
					output[len] = '\0';
					printf("%s\n" , output); // display confirmation message
					bzero(output,BUFFER); // pad buffer with zeros

					printf("Enter the events     ");
		   			//fgets(input,BUFFER,stdin); //take input from user
					scanf(" %[^\t\n]s",input); //recieve user input
					send(sock, input, strlen(input) ,0); // send input to server
					len = recv(sock, output, BUFFER, 0); // recieve confirmation message from server
					output[len] = '\0';
					printf("%s\n" , output); // display confirmation message
					bzero(output,BUFFER); // pad buffer with zeros
					
					
					
                 	break;

      		 	case 2:   //AUTHENTICATE USER

				temp="get"; // keyword to be send to server so that server knows it is a search operation

				send(sock, temp, sizeof(temp) ,0); // send input to server
				printf("Enter the username    ");
		   			
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER);  

				printf("Enter password  ");	
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER); 
							
				printf("Server searching...... Waiting for event updates\n");
				recv(sock, output, BUFFER, 0);
				printf("%s\n", output);
				bzero(output,BUFFER);
				
				//close(sock); // Disconnect from server
				printf("SEARCH COMPLETE!!! \n");
				
				break;

        		case 3: //publish event
        		temp="cre"; // keyword to be send to server so that server knows it is a create event operation

				send(sock, temp, sizeof(temp) ,0); // send input to server
				printf("Enter the username    ");
		   			
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER);  

				printf("Enter password  ");	
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER); 

				printf("Enter event name  ");	
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER); 

				printf("Enter event tag  ");	
				scanf(" %[^\t\n]s",input);
				send(sock, input, strlen(input) ,0); // send input keyword to server
				len = recv(sock, output, BUFFER, 0);
				output[len] = '\0';
				printf("%s\n" , output);
				bzero(output,BUFFER); 
							
				printf("EVENT CREATED \n");
			
	                   	break;

        		case 4:  //when client want to terminate connection with server  
					temp="ter"; // keyword to be send to server so that server knows client wants to terminate connection to server
					send(sock, temp, sizeof(temp) ,0); // send input to server
					close(sock);
					printf("Connection terminated with server.\n");
					return 0;
		
       			

        		default:    printf("Invalid option\n");
			} // terminates switch case
		
		} // terminates else statement
	} // terminates while loop 
		

	
	return (0);
}


		
	
	
