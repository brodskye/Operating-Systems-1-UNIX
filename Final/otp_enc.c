/***********************************************************************************************************************************************************************
 *Name: David Jansen
 *Date:8/14/17
 *Class: CS344
 *Assignment #: 4
 *Description: This is the client portion of the encryption.  All this program does it takes the content from the plain text and key text, send them to the server for encryption and then receives the encrypted text.
 * ***************************************************************************************************************************************************************************/

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
perror(msg); 
exit(2); 
}

//This is where all of the client part of the encryption is happening, most of this code was provided to us in our lectures.  I have made many changes in order to make the client code work for our requirements. 
int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	//These are what I used to hold both the plain and key texts.
	char P_Text[70000];
	char Key_Text[70000];
    FILE* P_Text_F = NULL;
	FILE* Key_Text_F = NULL;
	//Here is what I used for comparisons to make sure the text in the plaintext was allowed.
	char The_Letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	//Here I am making sure the amount of arguments is correct.
	if (argc != 4) 
	{
	error("USAGE: opt_enc plaintext key port\n");
	}

	//Make sure the second argument it s a file and if so open it.
	if(P_Text_F = fopen(argv[1], "r"))
	{
		//Get the files contents and put them in to our plain text array, then close the file.
		fgets(P_Text, sizeof(P_Text), P_Text_F);
		fclose(P_Text_F);
	}
	//If the file can't open print an error.
	else
	{
	error("Error: invalid Plain text file");
	}

	int i;
	int k;
	//Here I am looping through the plain text file and making sure that the letters in it are accepted.
	for(i = 0; i < strlen(P_Text) - 1; i++)
	{
	int good_in = 0;

		for(k = 0; k < strlen(The_Letters) - 1; k++)
		{
			//loop through all the valid letters and compare them to each part of the plain text.
			if(P_Text[i] == The_Letters[k] || P_Text[i] == ' ')
			{
			good_in = 1;
			break;
			}	  
		}
	//If one of the letters is invalid it will make it so good_in remains 0 which will output an error.
		if(good_in == 0)
		{
		error("otp_enc error: input plaintext_5 contains bad characters");
		}	

	}
	//Check to see if the third argument is a file and if it is open it.
	if(Key_Text_F = fopen(argv[2], "r"))
	{
		//grab the contents of that file and store them in our key text array, then close out of the file.
		fgets(Key_Text, sizeof(Key_Text), Key_Text_F);
		fclose(Key_Text_F);
	}
	//If the file is invalid print an error.
	else
	{
	error("Error: invalid Key text file");
	}

	//Check to make sure the key is large enough for the plain text file.
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
	if (serverHostInfo == NULL) 
	{ 
	error("CLIENT: ERROR, no such host\n"); 
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
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
	
	// Send the plain text to the server.
	charsWritten = send(socketFD, P_Text, 70000, 0); 
	//Print an error it there was a problem sending the plain text.
	if (charsWritten < 0) 
	{
	error("CLIENT: ERROR writing to socket");
	}
//If all the data was not written to the socket print an error
	if (charsWritten < strlen(P_Text))
	{
	printf("CLIENT: WARNING: Not all data written to socket!\n");
	}
	
//Send the key text to the server.
	charsWritten = send(socketFD, Key_Text, 70000, 0);
//If there is a problem with sending the key text.
	if(charsWritten < 0)
	{
	error("Clicnet: ERROR writing to socket");
	}
//Make sure all of the data gets written to socket
	if(charsWritten < strlen(Key_Text))
	{
	printf("CLIENT: WARNING not all data written to socket!\n");
	}



	//Clear out my arrays for reuse
	memset(P_Text, '\0', sizeof(P_Text)); 
	memset(Key_Text, '\0', sizeof(Key_Text));
	//Receive the response from the server
	charsRead = recv(socketFD, P_Text, sizeof(P_Text) - 1, 0);

	//If there was an error reading from the server.
	if (charsRead < 0)
	{
	error("CLIENT: ERROR reading from socket");
	}
	//Send the encrypted text to stdout.
	printf("%s", P_Text);

	//Close the socket.
	close(socketFD); 
	return 0;
}
