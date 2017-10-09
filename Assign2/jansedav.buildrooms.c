/***************************************************************************************************************
 *Name: David Jansen
 *Date: 7/22/2017
 *Assignment #: 2
 *Class: CS 344
 *Description:  This file randomly generates 7 rooms, generates a minimum of 3 connections for each room, assigns the rooms as either START, MID or END rooms and than stores all of this data in to a directory which contains a file for each of these rooms.  
 * ************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>



/*************************************************************************************************************************************/
//Creating my struct that will contain information for each room.
typedef struct room
{
	char* name;
	int connections;
	char* room_type;
	struct room* Connected_rooms[6];
}room;
/*************************************************************************************************************************************/


/*************************************************************************************************************************************/
//Here is how I shuffle my array of names in order to get randomly selected rooms, the size integer allows me to select how much of the array I would like selected.
void shuffle_array(int size, room array[10])
{
	int i;

//Here I am using a simply shuffle array algorithm to get my array of rooms randomized.   
	for(i = 0; i < size; i++)
	{
		room temp = array[i];
		int Random = rand() % size;
		array[i] = array[Random];
		array[Random] = temp;

	}
}
/*************************************************************************************************************************************/


/*************************************************************************************************************************************/
//This function simply connects two rooms and increase the count for that rooms connections.
void ConnectRoom(room* A, room* B)
{
A->Connected_rooms[A->connections] = B;
A->connections++;
}
/*************************************************************************************************************************************/


/*************************************************************************************************************************************/
//This function returns a random room out of the first 7 rooms in the array which were selected randomly to be the rooms for the game.
room *GetRandomRoom(room randomroom[10])
{
int i;

//I use a random number generator to grab me one of the seven rooms I am using randomly. 
i = rand() % 7;

room* A;
A = &randomroom[i];

return A;
}
/*************************************************************************************************************************************/


/*************************************************************************************************************************************/
//This function is a helper function to tell when the rooms each have enough connections.
bool IsGraphFull(room Rooms[10])
{
	
	int i;
 //I am checking to see if my rooms have at least 3 connections.
	for(i = 0; i < 7; i++)
	{
		if(Rooms[i].connections < 3)
		{
		return false;
		}
	}

return true;
}
/*************************************************************************************************************************************/


/*************************************************************************************************************************************/
bool CanAddConnectionFrom(room* A, room* B)
{
	//First I check to see if the rooms I want to add a connection to have space.
	if(A->connections >= 6 || B->connections >= 6)
	{
	return false;
	}
	
	//If they do have space I than check to see if they are already connected.
	int i;
	for(i = 0; i < A->connections; i++)
	{
		
		if( A->Connected_rooms[i] == B)
		{
		return false;
		}

	}
	
return true;
}
/*************************************************************************************************************************************/



/*************************************************************************************************************************************/
//This function will randomly grab two rooms and add a connection between them.
void AddRandomConnection(room Connect[10])
{

room* A;
room* B;

//Grab a random room for both A and B.
A = GetRandomRoom(Connect);

B = GetRandomRoom(Connect);

//I am checking to see here if A and B are the same and if I can add a connection between the two of them.
	if(A->name != B->name && CanAddConnectionFrom(A, B) == true)
	{
//If I can connect them.
		ConnectRoom(A, B);
		ConnectRoom(B, A);
	return;
	}
//If I cant recursively run the function until I can.
else
	AddRandomConnection(Connect);

}
/*************************************************************************************************************************************/

//This function creates the directory with the pid, it also fills that directory with 7 random rooms and all of their information.
void CreateFilesandDirect(room Room[10])
{

//Grabbing my pid. 
int pid = getpid();
//I found out online the max size for a process id is 5 characters so I added that to my directory names size, included the null terminator and found out I need 21 chars of space.

//Allocated memory for the directory.
char *Direct = malloc(21 * sizeof(char));
//This is the prefix I am using
char *name = "jansedav.rooms.";
//This is how I connected both my prefix and pid in to a single string.
snprintf(Direct, 21, "%s%d", name, pid);
//Make that directory.
mkdir(Direct, 0755);



int i;

	//For all 7 rooms.
	for(i = 0; i < 7; i++)
	{	
	//Create a file name for each room.
	char filename[260];
	sprintf(filename, "%s/%s_room", Direct, Room[i].name);

	//Create that rooms file.
	int filedescriptor = open(filename, O_RDWR | O_CREAT, 0600);

	FILE *fp;
	//open that file
	fp = fopen(filename, "w");
	
	//Prints the rooms information here.
	fprintf(fp, "ROOM NAME: %s", Room[i].name);
	fprintf(fp, "\n");
	int j;
		for(j = 0; j < Room[i].connections; j++)
		{
			
			fprintf(fp, "CONNECTION %d: %s\n", j+1, Room[i].Connected_rooms[j]->name);

		}
	fprintf(fp, "ROOM_TYPE: %s\n", Room[i].room_type);
//Close out of that file
fclose(fp);
}
//Free up memory.
free(Direct);

}






/*************************************************************************************************************************************/
int main()
{


//Here I am defining an array of rooms to hold all 10 of the rooms.
room Rooms[10];

//Here I give each of the rooms different names.
Rooms[0].name = "Hawk";
Rooms[1].name = "Eagle";
Rooms[2].name = "Falcon";
Rooms[3].name = "Osprey";
Rooms[4].name = "Vulture";
Rooms[5].name = "Crow";
Rooms[6].name = "Raven";
Rooms[7].name = "Owl";
Rooms[8].name = "Kestrel";
Rooms[9].name = "Condor";

//This is important for the number generators to output different random numbers after every run.
srand(time(NULL));
int i;
int j;



//Here I am setting each room to start with zero connections.
for(i = 0; i < 10; i++)
{
	Rooms[i].connections = 0;
}



//Here I am shuffling the starting array and grabbing the first 7 random rooms and assigning them to either a start, mid or end room.
shuffle_array(10, Rooms);
Rooms[0].room_type = "START_ROOM";
Rooms[1].room_type = "MID_ROOM";
Rooms[2].room_type = "MID_ROOM";
Rooms[3].room_type = "MID_ROOM";
Rooms[4].room_type = "MID_ROOM";
Rooms[5].room_type = "MID_ROOM";
Rooms[6].room_type = "END_ROOM";



//All I am doing here is adding random connections until every room has at least 3 connections.
while(IsGraphFull(Rooms) == false)
{
	AddRandomConnection(Rooms);
}

CreateFilesandDirect(Rooms);


return 0;
}
/*************************************************************************************************************************************/

