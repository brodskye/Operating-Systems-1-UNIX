/**********************************************************************************************************************************************************************
 *Name: David Jansen
 *Date: 8/14/17
 *Assignment #: 4
 *Class: CS344
 * ********************************************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


//This is the server side of the encryption which receives the plain text and key text from the client send and sends back an encyrpted message to the client. A lot of this was taken from the lectures and changed to work for this assignment.
void error(const char *msg) { perror(msg); exit(2); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	pid_t spawnPid;
	//These are my arrays to hold the plain and key texts.
	char Key_Text[70000];
	char P_Text[70000];
	int i, j;
	int rando;
	//The ints hold the numbers that hold the letter values.
	int Key_Num, P_Num, C_Num;
	//This is the array to hold all of the acceptable inputs.
	char The_Letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	struct sockaddr_in serverAddress, clientAddress;

	//Check to make sure the amount of arguments is correct.
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

	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);

	if (establishedConnectionFD < 0)
	{
	error("ERROR on accept");
	}

	//Fork off our child proccess.
	spawnPid = fork();
	switch (spawnPid)
	{
		//If there was an error then print the error.
		case -1:
		{
		error("Hull Breach!\n");
		exit(2);
		break;
		}
		//We are now in the child process
		case 0:
		{
		//Clear out our arrays.
		memset(P_Text, '\0', 70000);
		memset(Key_Text, '\0', 70000);
	
		//Receive the content for the plain_text array.
		charsRead = recv(establishedConnectionFD, P_Text, 70000, 0);
			if(charsRead < 0)
			{
			error("Error reading text from socket");
			}
		//Receive the content for the Key_text array.
		charsRead = recv(establishedConnectionFD, Key_Text, 70000, 0);
			if(charsRead < 0)
			{
			error("Error reading key from socket");
			}
			//This is where I loop through the size of P_Text to encrypt it.
			for(i = 0; i < strlen(P_Text)-1; i++)
			{
				//This is where I loop through the letters array to find the number value of the letter.
				for(j = 0; j < strlen(The_Letters)-1; j++)
				{
					//Here is the actual comparison
					if(P_Text[i] == The_Letters[j])
					{
					//Set it to where it was in the array.
					P_Num = j;
					}
					//If the letter is actually a space set it to 26.
					else if(P_Text[i] == ' ')
					{
					P_Num = 26;
					}
				}
				
				//This is where I loop through the letters array to find the number value of the letter.
				for(j = 0; j < strlen(The_Letters)-1; j++)
				{
					//Here if the actual comparison
					if(Key_Text[i] == The_Letters[j])
					{
					//Set it to where it was in the for loop because it will be the same number.
					Key_Num = j;
					}	
					//If the letter is actually a space set it to 26.
					else if(Key_Text[i] == ' ')
					{
					Key_Num = 26;
					}
				}
			//Add the numbers together.
			C_Num = Key_Num + P_Num;
			//If they are now larger than 26 subtract it by 27
				if(C_Num > 26)
				{		
				C_Num = C_Num - 27;
				}
		//Set the P_Text array to the correct letter from the array holding the letters.
			P_Text[i] = The_Letters[C_Num];
			}
			//Send the content back to the client side and close out of that connection.
			charsRead = send(establishedConnectionFD, P_Text, 70000, 0);
			close(establishedConnectionFD);
			break;			
		}
		//Parent process waits until the child process has completed.
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
