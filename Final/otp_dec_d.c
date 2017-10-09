/***********************************************************************************************************************************************************************
 *Name: David Jansen
 *Date: 8/14/17
 *Assignment #: 4
 *Class: CS344
 * **********************************************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(2); } // Error function used for reporting issues



//This is another server that we were supplied much of the code for in class, this server listens to the otp_dec client to receive two array one cipher text and one key text, it then decrypts the cipher text and returns it to the client.
int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	pid_t spawnPid;
	//Arrays to hold the cipher and key text.
	char Key_Text[70000];
	char P_Text[70000];
	int i, j;
	int rando;
	//These numbers represnt the letter value in the letters array.
	int Key_Num, P_Num, C_Num;
	//This array is how I compare my cipher and key text letters.
	char The_Letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2)
   	{ 
	fprintf(stderr,"USAGE: %s port\n", argv[0]); 
	exit(1); 
	} 

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
	{
	error("ERROR opening socket");
	}

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect to port
	{
	error("ERROR on binding");
	}

	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections


while (1){
	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

	//Here is where we are listening.
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);

	if (establishedConnectionFD < 0)
	{
	error("ERROR on accept");
	}

	//Fork off a child process.
	spawnPid = fork();
	switch (spawnPid)
	{
		//If there is a problem with the fork.
		case -1:
		{
		error("Hull Breach!\n");
		exit(2);
		break;
		}
		case 0:
		{
		//Empty out the arrays.
		memset(P_Text, '\0', 70000);
		memset(Key_Text, '\0', 70000);
	//Receive the encrypted text from our plain text, which will actually be cipher text.
		charsRead = recv(establishedConnectionFD, P_Text, 70000, 0);
			if(charsRead < 0)
			{
			error("Error reading text from socket");
			}
	//Receive the key we used in order to encrypt the plain text so we need to use it to decrypt.
		charsRead = recv(establishedConnectionFD, Key_Text, 70000, 0);
			if(charsRead < 0)
			{
			error("Error reading key from socket");
			}
	//Loop through the entirety of the plain text array.
			for(i = 0; i < strlen(P_Text)-1; i++)
			{
				//Compare the letters to the letter array so we can get its number value.
				for(j = 0; j < strlen(The_Letters)-1; j++)
				{
					//Here if the actual comparison
					if(P_Text[i] == The_Letters[j])
					{
					//Set the number for the letter.
					P_Num = j;
					}
					//If the letter is actually a space set the number to 26
					else if(P_Text[i] == ' ')
					{
					P_Num = 26;
					}
				}
				//Compare the letters to the letters array so we can get its number value.
				for(j = 0; j < strlen(The_Letters)-1; j++)
				{
					//Here is the actual comparison
					if(Key_Text[i] == The_Letters[j])
					{
					//Set the number for the letter.
					Key_Num = j;
					}	
					//If the letter is actually a space set the number to 26.
					else if(Key_Text[i] == ' ')
					{
					Key_Num = 26;
					}
				}
			//Here we subtract the key from the letter because we added earlier (Opposite of how we encrypted).
			C_Num = P_Num - Key_Num;

			//And if the number is now negative add 27 (opposite of encryption).
				if(C_Num < 0)
				{		
				C_Num = C_Num + 27;
				}
		//Now set the Plain text that we are going to send back to the client to the correct letter.
			P_Text[i] = The_Letters[C_Num];
			}

			//Here we send back the decrypted array and close thise connection.
			charsRead = send(establishedConnectionFD, P_Text, 70000, 0);
			close(establishedConnectionFD);
			break;			
		}
		//Here is the parent process which waits for the child to end.
		default:
		{
		pid_t actualPid = waitpid(spawnPid, &rando, 0);
		break;
		}
	}
}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
