/********************************************************************************************************************************************************************************************************
 *Name: David Jansen
 *Date: 8/9/2017
 *class: CS 344
 *Assignment: 4
 *Description: This program randomly generates a key for the encrypt and decrypt programs.
 * ******************************************************************************************************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
//An array to hold all of the letters of the alphabet plus the space which I can use to easily randomly generate from.
char Key_Letters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
//This is required to consistently be generating random numbers.
srand(time(NULL));
	//Here I am chacking to make sure there is a number for how long the user wants the message to be, and printing an error if there was not message entered.
	if(argc < 2)
	{
	perror("Error Please enter a length for the key");
	exit(1);;
	}
	//If there was a correct entry for the size of message go here.
	else
	{
	//Convert the size to an integer so I know how many letters they want.
	int length = atoi(argv[1]);
	int i;

	//Generate random letters from my key using this for loop.
		for(i = 0; i <= length-1; i++)
		{
		int rando = rand() % 27;
	    printf("%c", Key_Letters[rando]);
		}
	}
//add a new line terminator at the end for stdout purposes.
printf("\n");
return 0;
}
