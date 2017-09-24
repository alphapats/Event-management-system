/*
FILE NAME: FILESERVER.C
CREATED BY:163054001 AMIT PATHANIA
This files creates a central server. File takes one commandline input arguments ie own listning port.
Clients will use Server IP and port to connect this server for publish and search files.
The code has following main operations:
JOIN : Whenever new client want to join network, server will be connected
PUBLISH : Clients can publish file to server for sharing with filename,filepath and 
their port no.Details of the file will be stored in publish list file. (filelist.txt)
SEARCH : Clients can search file using keyword . It uses bash script (searchscript.sh) for searching file in publish list.
once file search is complete ,the client is disconnected to allow other clients to connect and reduce load on server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <pthread.h> 

#define ERROR     -1  // defines error message
#define MAX_CLIENTS    10//defines maximum number of clients that can connect simultaneously
#define MAX_BUFFER    256 //used to set max size of buffer for send recieve data 

int add_IP(char*); // function to add IP to the list
int update_IP(char *);//function to update IP_list once client disconnected
int add_transaction(char *);//function to add_transaction
time_t current_time; // variable to store current time
int fork (void);
void *client_handler(void *);

struct event
{
    char event_description[MAX_BUFFER];
    char event_tag[MAX_BUFFER];
    char event_creater[MAX_BUFFER];

};

struct user
{
    char uname[MAX_BUFFER];
    char upassword[MAX_BUFFER];
    char uevent[MAX_BUFFER];

};






int main(int argc, char **argv)  
{
	int sock; // sock is socket desriptor for server 
	int new; // socket descriptor for new client
	struct sockaddr_in server; //server structure 
	struct sockaddr_in client; //structure for server to bind to particular machine
	int sockaddr_len=sizeof (struct sockaddr_in);	//stores length of socket address
	int *new_sock;


	


	int pid;// to manage child process

	if (argc < 2)    // check whether port number provided or not
	{ 
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}


	/*get socket descriptor */
	if ((sock= socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{ 
		perror("server socket error: ");  // error checking the socket
		exit(-1);  
	} 
	 
	/*server structure */ 
	server.sin_family = AF_INET; // protocol family
	server.sin_port =htons(atoi(argv[1])); // Port No and htons to convert from host to network byte order. atoi to convert asci to integer
	server.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY means server will bind to all netwrok interfaces on machine for given port no
	bzero(&server.sin_zero, 8); //padding zeros
	
	/*binding the socket */
	if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR) //pointer casted to sockaddr*
	{
		perror("bind");
		exit(-1);
	}

	/*listen the incoming connections */
	if((listen(sock, MAX_CLIENTS)) == ERROR) // listen for max connections
	{
		perror("listen");
		exit(-1);
	}

		pthread_t thread_id;

	while(new = accept(sock, (struct sockaddr *)&client, &sockaddr_len))
	{
		puts("connection acceptedd");
		

		pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new;
         
        if( pthread_create( &thread_id , NULL ,  client_handler , (void*) &new) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        printf("New client connected from port no %d and IP %s\n", ntohs(client.sin_port), 
inet_ntoa(client.sin_addr));
	 	
        puts("Handler assigned ");
    }

    if ((new < 0)) // accept takes pointer to variable containing len of struct
		{
			perror("ACCEPT.Error accepting new connection");
			exit(-1);
		}

	
     return 0;
   }

int update_IP(char *client_ip)
{
//updating Client IP in IP List
		char* clientinfo = "log.txt";
		FILE *clientdet = fopen(clientinfo, "a+");

		if(clientdet==NULL) // if unable to open file
        {
            printf("Unable to open log File.Error");
            return -1;  
        }   
        
        else
		{
		fwrite("\n", sizeof(char), 1, clientdet);	
			
		// write details of IP of client
		fwrite(client_ip,1, strlen(client_ip), clientdet);		//adding client IP address to given file	
		char update[] = "  disconnected from server at ";
		fwrite(update,1, strlen(update), clientdet);
		current_time = time(NULL);
		fwrite(ctime(&current_time),1, strlen(ctime(&current_time)), clientdet);	
		
		
		fclose(clientdet);
	}
}

int add_IP(char *client_ip)
{
//adding Client IP in IP List
		char* clientinfo = "log.txt";
		FILE *clientdet = fopen(clientinfo, "a+");

		if(clientdet==NULL) // if unable to open file
        {
            printf("Unable to open log File.Error");
            return -1;  
        }   
        
        else
		{

			fwrite("\n", sizeof(char), 1, clientdet);	
			fwrite(client_ip,1, strlen(client_ip), clientdet);		//adding client IP address to given file	
			char update[] = "  connected to server at  ";
			fwrite(update,1, strlen(update), clientdet);
			current_time = time(NULL);
			fwrite(ctime(&current_time),1, strlen(ctime(&current_time)), clientdet);
			//fwrite(" ", sizeof(char), 1, clientdet);	
			
		fclose(clientdet);
	}
}

int add_transaction(char *msg)
{
//adding Client IP in IP List
		char* clientinfo = "log.txt";
		FILE *clientdet = fopen(clientinfo, "a+");

		if(clientdet==NULL) // if unable to open file
        {
            printf("Unable to open log File.Error");
            return -1;  
        }   
        
        else
		{
			fwrite("\n", sizeof(char), 1, clientdet);	
			fwrite(msg,1, strlen(msg), clientdet);		//adding client IP address to given file
			fwrite(" ", sizeof(char), 1, clientdet);
			current_time = time(NULL);
			fwrite(ctime(&current_time),1, strlen(ctime(&current_time)), clientdet);
			//fwrite(" ", sizeof(char), 1, clientdet);		
		// write details of IP of client
				
		
			fclose(clientdet);
	}
}




/*
 * This will handle connection for each client
 * 
 */
void *client_handler(void *socket_cl)
{
    //Get the socket descriptor
    int new = *(int*)socket_cl;
   

    //variables for add new user operation
	char buffer[MAX_BUFFER]; // Receiver buffer; 
	//char file_name[MAX_BUFFER];//Buffer to store filename,path and port recieved from client
	//char *client_ip;//variable to store IP address of client
	
	int success=0;

	//variable for authenticating existinguser operation
	char user_name[MAX_BUFFER];
	char user_key[MAX_BUFFER];//file keyword for search by user
	int len;// variable to measure MAX_BUFFER of incoming stream from user
	

     
    struct sockaddr_in client;
    socklen_t sin_size = sizeof(client);
    int res = getpeername(new, (struct sockaddr *)&client, &sin_size);
    char *client_ip ;
    client_ip=inet_ntoa(client.sin_addr);
    //printf(client_ip);
    //strcpy(client_ip, inet_ntoa(client.sin_addr));
		
	add_IP(client_ip); //wadding Client IP into IP List
	 	
		while(1)
		{
		len=recv(new, buffer , MAX_BUFFER, 0);
		buffer[len] = '\0';
		printf("%s\n",buffer);

		//conenctionerror checking
			if(len<=0)//connection closed by client or error
				{
				if(len==0)//connection closed
				{
					printf("client %s hung up\n",inet_ntoa(client.sin_addr));
					update_IP(client_ip);
					return 0;
					
				}
				else //error
				{
					perror("ERROR IN RECIEVE");
					return 0;
				}
			
			}//clos if loop

			//ADD NEW USER OPERATION
		if(buffer[0]=='n' && buffer[1]=='e' && buffer[2]=='w') // check if user wants to publish a file
		{
	
		//Recieve username and password
		char login_name[MAX_BUFFER];
		char login_password[MAX_BUFFER];
		char desired_event[MAX_BUFFER];
		struct user new_user = {0}; //Initializing to null
			
		recv(new, login_name, MAX_BUFFER, 0); //recv user name from client, data is pointer where data recd will be stored
		char Report1[] = "Username recieved"; 
		send(new,Report1,sizeof(Report1),0);	

		recv(new, login_password, MAX_BUFFER, 0); //recv user password from client, data is pointer where data recd will be st
		char Report2[] = "Password recieved"; 
		send(new,Report2,sizeof(Report2),0);	

		recv(new, desired_event, MAX_BUFFER, 0); //recv events from client, data is pointer where data recd will be st
		strcpy(new_user.uname, login_name);
        strcpy(new_user.upassword, login_password);
        strcpy(new_user.uevent,desired_event);


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		//INSERT CODE TO ADD USERS TO DATABASE



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if (success==1)
		{
		char Report3[] = "USER ADDED   "; 
		send(new,Report3,sizeof(Report3),0);	
		printf("%s\n",Report3);
		strcat(Report3,login_name);
		add_transaction(Report3);
		}
		else
		{
		char Report3[] = "ADDING USER FAILED   "; 
		send(new,Report3,sizeof(Report3),0);
		printf("%s\n",Report3);	
		strcat(Report3,login_name);
		add_transaction(Report3);
		}
		

		
		
		
		//client_ip = inet_ntoa(client.sin_addr); // return the IP
		
			
		
		}


		

		//AUTHENTICATE USER
		else if(buffer[0]=='g' && buffer[1]=='e' && buffer[2]=='t') //check keyword for search sent by client
		{
		
		bzero(buffer,MAX_BUFFER); // clearing the buffer by padding

		len=recv(new, user_name, MAX_BUFFER, 0); //recv user_name from client to search
		char Report1[] = "username recieved"; 
		send(new,Report1,sizeof(Report1),0);
		user_name[len] = '\0';
		printf("%s\n",user_name);

		len=recv(new, user_key, MAX_BUFFER, 0); //recv user_name from client to search
		char Report2[] = "password recieved"; 
		send(new,Report2,sizeof(Report2),0);
		user_key[len] = '\0';
		printf("%s\n",user_key);

		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		//INSERT CODE TO AUTHENTICATE USER FROM DATABASE



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		success=1;
		if (success)
			{
				char Report3[] = "Login Successfull   "; 
				send(new,Report3,sizeof(Report3),0);
				strcat(Report3,user_name);
				add_transaction(Report3);

			}
		else
		{
			char Report3[] = "Login Unsuccessfull.Try again   ";
			send(new,Report3,sizeof(Report3),0);
			strcat(Report3,user_name);
			add_transaction(Report3);

		}	
		


	    client_ip = inet_ntoa(client.sin_addr); 
        //update_IP(client_ip);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		//INSERT CODE TO GET EVENTS FROM DATABASE DATABASE



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        char events[MAX_BUFFER]="events";
        send(new,events,sizeof(events),0);

        if(success==1)
        {
        char Report4[] = "EVENT SUCCESFULLY PUSHED TO   "; 
        strcat(Report4,user_name);
        strcat(Report4,"  at IP address ");
        strcat(Report4,client_ip);
		add_transaction(Report4);	
        }
        else
        {
        char Report4[] = "FAILED TO PUSH EVENTS TO   "; 
        strcat(Report4,user_name);
        strcat(Report4,"  at IP address ");
        strcat(Report4,client_ip);
		add_transaction(Report4);
        }


	    printf("Events pushed to client!!!!");
	    printf("%s\n", client_ip);
	    
	    
		}// close get condition




		else if(buffer[0]=='c' && buffer[1]=='r' && buffer[2]=='e')
		{


	//variable for event creation
	char event_name[MAX_BUFFER];
	char event_tag[MAX_BUFFER];//file keyword for search by user
	//char report[MAX_BUFFER];
	struct event new_event = {0}; //Initializing to null

		bzero(buffer,MAX_BUFFER); // clearing the buffer by padding

		len=recv(new, user_name, MAX_BUFFER, 0); //recv user_name from client to search
		char Report1[] = "username recieved"; 
		send(new,Report1,sizeof(Report1),0);
		user_name[len] = '\0';
		printf("%s\n",user_name);

		len=recv(new, user_key, MAX_BUFFER, 0); //recv user_name from client to search
		char Report2[] = "password recieved"; 
		//send(new,Report2,sizeof(Report2),0);
		user_key[len] = '\0';
		//printf("%s\n",user_key);


		


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		//INSERT CODE TO AUTHENTICATE USER FROM DATABASE



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		success=1;

		if (success)
			{
				char Report3[] = "Login Successfull   "; 
				send(new,Report3,sizeof(Report3),0);
				strcat(Report3,user_name);
				add_transaction(Report3);

			}
		else
		{
			char Report3[] = "Login Unsuccessfull  ";
			send(new,Report3,sizeof(Report3),0);
			strcat(Report3,user_name);
			add_transaction(Report3);

		}	
		

		len=recv(new, event_name, MAX_BUFFER, 0);
		event_name[len] = '\0';
		printf("%s\n",event_name);
		char report[]="Event_name recieved";
		send(new,report,sizeof(report),0);
		len=recv(new, event_tag, MAX_BUFFER, 0);
		event_tag[len] = '\0';
		printf("%s\n",event_tag);
		char report1[]="Event_tag recieved";
		send(new,report1,sizeof(report1),0);
		client_ip = inet_ntoa(client.sin_addr); //
        //update_IP(client_ip);

        strcpy(new_event.event_description, event_name);
        strcpy(new_event.event_tag, event_tag);
        strcpy(new_event.event_creater,user_name);
         ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		//INSERT CODE TO INSERT EVENTNAME AND EVENTTAG INTO DATABASE DATABASE



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(success==1)
        {
        char Report4[] = "EVENT SUCCESFULLY ADDED BY   "; 
        strcat(Report4,user_name);
        strcat(Report4,"  from IP address ");
        strcat(Report4,client_ip);
		add_transaction(Report4);	
        }
        else
        {
        char Report4[] = "EVENT CREATION FAILED   "; 
        strcat(Report4,user_name);
        strcat(Report4,"  from IP address ");
        strcat(Report4,client_ip);
		add_transaction(Report4);
        }



	    printf("Events created by ");
	    printf("%s\n",client_ip );
	    
	    
		} //close create loop
	
	
		//TERMINATE OPERATION:when user want to disconnect from server
		else if(buffer[0]=='t' && buffer[1]=='e' && buffer[2]=='r')
		{
		printf("Client disconnected from port no %d and IP %s\n", ntohs(client.sin_port), 
inet_ntoa(client.sin_addr));
		client_ip = inet_ntoa(client.sin_addr); // return the IP
		update_IP(client_ip);
		return 0;
			
		} //close terminate loop
	
	}// close while loop inside fork.server will keep listening client till disconnected
	
	
         
    //Free the socket pointer
    free(socket_cl);
     
    return 0;
}
