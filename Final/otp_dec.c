/**********************************************************************************************************************************************************************************
 *Name: David Jansen
 *Date: 9/12/2017
 *Classe: CS344
 *Assignment: #4
 *Description: This program grabs the data from the encrypted file and the key file, sends them to be decrypted, recieves the decrypted array and prints it to stdout.
 * ********************************************************************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//Function used to report any errors
void error(const char *msg) 
{
perror(msg); exit(2); 
}


int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	//These arrays are size 70000 because the largest plain taxt file is 69000
	char P_Text[70000];
	char Key_Text[70000];
    //File variables
	FILE* P_Text_F = NULL;
	FILE* Key_Text_F = NULL;


	//Check to make sure there is a correct amount of arguments.
	if (argc != 4) 
	{
	error("USAGE: opt_edc plaintext key port\n");
	}

	
	//Open the first argument and put it in to the plain text array.
	if(P_Text_F = fopen(argv[1], "r"))
	{
		fgets(P_Text, sizeof(P_Text), P_Text_F);
		fclose(P_Text_F);
	}
	//If the file doesn't exist send an error.
	else
	{
	error("Error: invalid Plain text file");
	}

	//Open the second armuent and put it in to the key array.
	if(Key_Text_F = fopen(argv[2], "r"))
	{
		fgets(Key_Text, sizeof(Key_Text), Key_Text_F);
		fclose(Key_Text_F);
	}
	//If the file doesn't exist send an error
	else
	{
	error("Error: invalid Key text file");
	}

	//Check to make sure the key is not smaller than the plain text file.
	if(strlen(P_Text) > strlen(Key_Text))
	{
	printf("Error: key '%s' is too short\n", argv[2]);
	exit(1);
	}


	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	//Check to make sure the host we are connecting to exists.
	if (serverHostInfo == NULL) 
	{ 
	error("CLIENT: ERROR, no such host\n"); 
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket to the server
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) 
	{
	error("CLIENT: ERROR opening socket");
	}

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
	{
	error("CLIENT: ERROR connecting");
	}

	// Send message to server of our Plain text.
	charsWritten = send(socketFD, P_Text, 70000, 0); // Write to the server
	if (charsWritten < 0) 
	{
	error("CLIENT: ERROR writing to socket");
	}

	//Make sure the all of the characters get written to the socket.
	if (charsWritten < strlen(P_Text))
	{
	printf("CLIENT: WARNING: Not all data written to socket!\n");
	}
	
	//Send the key text to the server for decryption.
	charsWritten = send(socketFD, Key_Text, 70000, 0);
	if(charsWritten < 0)
	{
	error("Clicnet: ERROR writing to socket");
	}
	//Make sure all of the key text gets written to the socket.
	if(charsWritten < strlen(Key_Text))
	{
	printf("CLIENT: WARNING not all data written to socket!\n");
	}



	//Clear out your arrays to reuse them.
	memset(P_Text, '\0', sizeof(P_Text));
	memset(Key_Text, '\0', sizeof(Key_Text));
	//Get the response from the server which is the decrypted text.
	charsRead = recv(socketFD, P_Text, sizeof(P_Text) - 1, 0); 

	if (charsRead < 0)
	{
	error("CLIENT: ERROR reading from socket");
	}
	//Send the decrypted text to standard out.
	printf("%s", P_Text);

	close(socketFD); // Close the socket
	return 0;
}
