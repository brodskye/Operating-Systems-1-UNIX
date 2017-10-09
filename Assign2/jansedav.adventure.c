/************************************************************************************************************************************
 *Name: David Jansen
 *Date: 7/23/2017
 *Class: CS344
 *Assignment#: 2
 *Description: This file grabs data from our files that contain the room contents.  It then starts the user in the start room and gives them the ability to choose which connected room they would like to.  While doing this the program makes sure the user inputs an actual room as well as counts the number of rooms the user has visited, at any time the user can also get the time and date output to them.
 * **********************************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//This is the struct i will use to store my room information.
typedef struct room
{
	char* name;
	char* room_type;
	int connect;
	char** connections;
}room;



/************************************************************************************************************************************/
//This function was almost entirely supplied to us by the proffesor and is used to find the most current directory. (2.4 Manipulating Directories).  I did make a few changes so it will work for this assignment though.
char * Access_new_dir()
{
int newestDirTime = -1;
//Set the prefix for my directory.
char targetDirPrefix[32] = "jansedav.rooms.";
char newestDirName[256];
memset(newestDirName, '\0', sizeof(newestDirName));

DIR* dirToCheck;
struct dirent *fileInDir;
struct stat dirAttributes;

//Open the directory we are currently in.
dirToCheck = opendir(".");


	if (dirToCheck > 0)
	{
	//While there are still files in this directory run.
		while((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
			//Here I am grabbing the attributes for the directory, most importantly when it was made.
			stat(fileInDir->d_name, &dirAttributes);
			
			//This if statement simply compares the time each directory was made and sets the variable newestDirName to the newest directory based off when it was created.
				if((int)dirAttributes.st_mtime > newestDirTime)
				{
				newestDirTime = (int)dirAttributes.st_mtime;
				memset(newestDirName, '\0', sizeof(newestDirName));
				strcpy(newestDirName, fileInDir->d_name);
				}
			} 
		}

	}
//Close the directory.
closedir(dirToCheck);

//Here is the main change I did to this function.  All I am doing is returning a string with the newest directories name.
char* str = malloc(sizeof(char) * sizeof(newestDirName));

strcpy(str, newestDirName);

return str;

}
/************************************************************************************************************************************/


/************************************************************************************************************************************/
//This function returns me the names of each of the files that contain room information.
char** get_file_names()
{

//Get the name of the newest directory.
char* newDir;
newDir = Access_new_dir();

//Allocate space for the file names.
char** room_names = (char**)malloc(10*sizeof(char*));;
char* all_names[10];

//open that directory.
DIR *d;
struct dirent *dir;
d = opendir(newDir);

	if(d)
	{
	int i;
	i = 0;

		while((dir = readdir(d)) != NULL)
		{
		//Here I am grabbing all of the file names in the directort and storing them in an array.		
		all_names[i] = dir->d_name;
		i++;

		}


		for(i = 2; i < 9; i++)
		{
		//Here I remove the first two elements of that array because they are not files I need to work with.
		room_names[i-2] = all_names[i];
	
		}
	//close the directory.
	closedir(d);
	}

//Free up allocated memory from the Access_new_dir() function.
free(newDir);
return room_names;
}
/************************************************************************************************************************************/


/************************************************************************************************************************************/
//This function allows the user to play the game.
void play_game(room Rooms[7])
{
//These are a bunch of variables I used throughout the function
int i;
int j;
int steps = 0;
room current;
room newcurrent;
char* visited[10];
int game_end = 0;
char user_in[20];

//Here I am finding the starting room for the game start.
	for(i = 0; i < 7; i++)
	{
		if(strcmp(Rooms[i].room_type, "START_ROOM") == 0)
		{
		current = Rooms[i];
		newcurrent = Rooms[i];
		}
	}

	while(game_end == 0)
	{
	//Print out the current room and it's cu=onnections.
	printf("CURRENT LOCATION: %s\n", current.name);
	printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < current.connect - 1; i++)
		{
		printf("%s, ", current.connections[i]);
		}
		//Prompt the user to get the info for which room they would like to go to.
		printf("%s.\n", current.connections[current.connect-1]);
		printf("WHERE TO? >");
		scanf("%s", user_in);
	
		for(i = 0; i < current.connect; i++)
		{
			//This if statement checks to see if the user input matches any of the connections to the current room.
			if(strcmp(current.connections[i], user_in) == 0)
			{
				for(j = 0; j < 7; j++)
				{
					//Here I am finding the Room that matches the user input so I can get it's information, as well as counting the steps and keeping track of them.
					if(strcmp(Rooms[j].name, user_in) == 0)
					{
					newcurrent = Rooms[j];	
					visited[steps] = Rooms[j].name;
					steps++;
					}
				}
			}
		}

		printf("\n");

		if(strcmp(user_in, "time") == 0)
		{
		//Variables used to get and store the time.
		time_t tm;
		struct tm *Timer;
		char time_stamp[100];
		
		
		tm = time(NULL);		
		//Get the current local time.
		Timer = localtime(&tm);
		//This is putting the correct string for the timing in this assignment in to a single string.
		strftime(time_stamp, 100, "%I:%M%p, %A, %B, %d, %Y", Timer);
		//Output the time to the user.
		printf("%s\n", time_stamp);
		printf("\n");

		//Here I am creating a file with write permission and putting the timestamp in to that file.
		FILE* TF = fopen("currentTime.txt", "w+");

		fprintf(TF, "%s", time_stamp);

		fclose(TF);


		}
		
		//Check to see if the users input matches any of the connections to the current room.		
		else if(strcmp(current.name, newcurrent.name) == 0)
		{	
		printf("\n");
		printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		printf("\n");
		}
		
		//Check to see if the room 	
		else if(strcmp(newcurrent.room_type, "END_ROOM") == 0) 
		{
		//Print the that the user won the game, how many steps they took, and which path they took.
		printf("\n");
		printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
		printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
			for(i = 0; i < steps; i++)
			{
				printf("%s\n", visited[i]);
			}
		//End the loop and exit the program.
		game_end = 1;
		exit(0);
		}
		
	//If the new room is not the same as the current room the room has changed so continue with the game.	
		else
		{
		
		current = newcurrent;	

		}

	}



}
/************************************************************************************************************************************/


/************************************************************************************************************************************/
//This function grabs and stores all of the connections for each room.
void set_connections(room Rooms[7])
{
//Here I grab the name of the most recent directory my buildrooms has made.
char* Dir;
Dir = Access_new_dir();

//Here I am getting the names of all the files where my room data is stored.
char** file_names = get_file_names();

char *name = malloc(50 * sizeof(char*));
char str[100];


int i;
int j;
int h;
	//For each room.
	for(i = 0; i < 7; i++)
	{
	int count = 0;
	int con_num = 0;
	int count2 = 0;
	int connects = Rooms[i].connect + 1;
	//Here I am creating a string that tells the function which directory and file to access.	
	sprintf(name, "%s/%s", Dir ,file_names[i]);
	//Open the file and set it to read.
	FILE* fp;
	//Open each file
	fp	= fopen(name, "r");
		//Right here I am going to the lines where the connections are held.
		for(j = 1; j < connects; j++)
		{
			//Go through the file line by line.
			while(fgets(str, 100, fp) != NULL)
			{
				//Only gather information from the line if it is one of the lines that holds a connection.
				if(count2 > Rooms[i].connect)
				{
					//Here I am removing the new line.
					strtok(str, "\n");
					char* cutter = strdup(str) + 11;
					//Here I am setting my room type for each room
					Rooms[i].room_type = cutter;
					
				}
				
				else if(count == j)
				{
					//Here I get rid of the new line.
					strtok(str, "\n");
					//Here I am cutting off the unecessary piece of the line.
					char* Cut = strdup(str) + 14;
					//Store the name of the connections in an array.
					Rooms[i].connections[con_num] = Cut;
					//This counter is for my connections
					con_num++;
					//This counter tells me when I am at the room type line.
				 	count2++;
							
				}
				else
				{
					//This is used to compare if one of the lines is a line holding a connection or not.
					count++;
					count2++;
				}
			
				
			}
		}		
	
	fclose(fp);
	}
//Free up the memory I used.
free(name);
free(file_names);
}
/************************************************************************************************************************************/


/************************************************************************************************************************************/
//This function returns how many connections each one of the rooms have.
void find_connect(room Rooms[7])
{

//Here I grab the name of the most recent directory my buildrooms has made.
char* Dir;
Dir = Access_new_dir();

//Here I am getting the names of all the files where my room data is stored.
char** file_names = get_file_names();

char *name = malloc(50 * sizeof(char*));
char str[100];


int i;
int connections;

	for(i = 0; i < 7; i++)
	{
	connections = 0;
	//Here I am creating a string that tells the function which directory and file to access.	
	sprintf(name, "%s/%s", Dir ,file_names[i]);
	//Open the file and set it to read.
	FILE* fp;
	//Open each file
	fp	= fopen(name, "r");

		while(fgets(str, 100, fp) != NULL)
		{
		//Plus one for every line in the file.
		connections++;		
		}

	//set our rooms number of connections to connections - 2 because two of the lines aren't connections.
	Rooms[i].connect = (connections - 2);

	fclose(fp);
	}

//Free up the memory I used.
free(name);
free(file_names);
}
/************************************************************************************************************************************/


/************************************************************************************************************************************/
void fill_struct_names(room Rooms[7])
{
//Here I grab the name of the most recent directory my buildrooms has made.
char* Dir;
Dir = Access_new_dir();

//Here I am getting the names of all the files where my room data is stored.
char** file_names = get_file_names();

char *name = malloc(50 * sizeof(char*));
char str[10];


int i;

	for(i = 0; i < 7; i++)
	{
	//Here I am creating a string that tells the function which directory and file to access.	
	sprintf(name, "%s/%s", Dir ,file_names[i]);
	//Open the file and set it to read.
	FILE* fp;
	fp	= fopen(name, "r");
	

	//Here I am grabbing each line individually and going out 11 characters in order to grab the name of the room.
	fseek(fp, 11, SEEK_SET); 
	//Now I store that name in a character array.
	fgets(str, 20, fp);
	//I used the strtok function in order to remove the new line character from the end of each of my names.
	strtok(str, "\n");
	//This is where I set the names to be stored in my array of room structs.
	Rooms[i].name = strdup(str);
	

fclose(fp);
}

//Free up the memory I used.
free(name);
free(file_names);
}
/************************************************************************************************************************************/





int main()
{

room Rooms[7];
int i;



//Memory allocation for the struct.
for(i = 0; i < 7; i++)
{
Rooms[i].connections = (char**)malloc(6*sizeof(char*));
Rooms[i].room_type = (char*)malloc(1*sizeof(char*));
}

//Run all the functions that are needed for the game.
fill_struct_names(Rooms);

find_connect(Rooms);

set_connections(Rooms);

play_game(Rooms);


//Free up memory.
for(i = 0; i < 7; i++)
{
free(Rooms[i].connections);
}


return 0;
}



