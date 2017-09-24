LIST OF FILES/FOLDERS:

-------------------------------------SERVER FOLDER-----------------------------------------------------------------------
1.eventserver.cpp : This file creates a central server. File takes one commandline input arguments ie own listning port.
Clients will use Server IP and port to connect this server for publish and search files.
The code has following main operations:
JOIN : Whenever new client want to join network, server will be connected
PUBLISH : Clients can publish file to server for sharing with filename,filepath and 
their port no.Details of the file will be stored in publish list file. (filelist.txt)
SEARCH : Clients can search file using keyword . It uses bash script (searchscript.sh) for searching file in publish list.

2.log.txt :This file contains the list of files published by the peers.This file is maintained by server.It contains details in format:
file_name file_path peer_portno peer_IP_address

------------------------------CLIENT FOLDER-------------------------------------------------------------------------------------
1.fileclient.c : This file creates a client. File takes two commandline input arguments server-ip and server port.
Server IP and port will be used to connect the server and publish and pull events.
-----------------------------------------------------------------------------------------------------------------------------------
HOW TO RUN SERVER:

1. Open server folder.
2. Run make.
3. Run ./server <port no>

---------------------------------------------------------------------------------------------------------------------------------------
HOW TO RUN CLIENT:

1. Open client folder.
2. Run make.
3. Run ./client <server IP address> <Server listening port no> 
4. Follow commands on screen for file publish and subscribe.
	
 ------------------------------------------------------------------------------------------------------------------------------------

