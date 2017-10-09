/***********************************************************************************************************************************************************
 *Name: David Jansen
 *Date: 8/6/2017
 *Class: CS344
 *Assignment: 3 smallsh
 *Description: Here we created a small command line using C which can input many different command line arguments and create outputs for them similar to how it is done in a command line in Unix.
***********************************************************************************************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


/***********************************************************************************************************************************************************/
//This checks the exit value for my program.
volatile sig_atomic_t Sig_Status = 0;
//This checks if we are in background only mode.
volatile sig_atomic_t Back_Check = 1;

/***********************************************************************************************************************************************************/
//This is a handler for the SIGCHLD handler I use it to catch a signal when a child ends.
void SIGCHLD_Handler(int sig)
{
	//Get the pid of the child.
	pid_t pid;
	pid = wait(NULL);
	//If the pid is an actual pid run this
	if(pid != -1)
	{
	//Print out the pid, and inform the process is done.
	printf("Background pid %d is done: ", pid);
	fflush(stdout);
	printf(": ");
	fflush(stdout);
	signal(SIGCHLD, SIGCHLD_Handler);
	//Print out the exit stats.
	User_Status();
	}
	else
	{

	}
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function is for making a process sleep in the background.
void Sleeper(char user_in[2048])
{
pid_t spawnPid = -5;
char* Direct;
//Here I am seperating out the input string with strtok
Direct = strtok(user_in, " ");
Direct = strtok(NULL, " ");

//Fork off to make a child process
spawnPid = fork();
	switch (spawnPid)
	{
		//If there was an error print it out and break.
		case -1:
		{
			perror("Hull Breach!\n"); exit(1); 
			break;
		}
		case 0:
		{
			//Here I print out the pid of the process before it starts sleeping.
			printf("background pid is %d\n", getpid());	
			printf(": ");
			fflush(stdout);
			//Here I actually sleep the process using execlp.
			execlp("sleep", "sleep", Direct, NULL);
			break;			
		}
		//Here I am waiting until the child process has completed.
		default:
		{
			//Set the status to 0 if the process returned from the child.
			Sig_Status = 0;				
			break;
		}
	}	


}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This is my sleeper function for sleeping in the foreground.
void Sleeper2(char user_in[2048])
{
pid_t spawnPid = -5;
char* Direct;
int rand;

//Seperate out the input string so I can use it.
Direct = strtok(user_in, " ");
Direct = strtok(NULL, " ");

//Fork out to a child process.
spawnPid = fork();
	switch(spawnPid)
	{	
		//If there was an error print it out and break.
		case -1:
		{
			perror("Hull Breach!\n"); exit(1);
		}
		case 0:
		{
			//Sleep the process in the foreground.
			execlp("sleep", "sleep", Direct, NULL);
			break;
		}
		default:
		{
			//Wait until the process is done sleeping.
			pid_t pid;	
			pid = waitpid(spawnPid, &rand, 0);
			break;
		}
	}
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This is a signal handler for control-c that catches a processes signal and kills the process.
void SignalTermination(int sig)
{
int the_pid;

//grab the pid from the signal we want to terminate
pid_t pid = wait(&the_pid);

//Terminate it.
kill(pid, the_pid);

//Tell the user we terminated the signal.
char* message = "Terminated by signal 2.\n";
char* prompt = ": ";
write(STDOUT_FILENO, message, 24);
fflush(stdout);
write(STDOUT_FILENO, prompt, 2);
fflush(stdout);
//Set the exit status to 2.
Sig_Status = 2;
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This is a handler for control-z that changes the program between foreground and background mode.
void STP_Handler(int sig)
{
//Prompts the user.
char* prompt = ": ";
//If the procees is in background mode change it to foreground.
	if(Back_Check == 1)
	{
	//Print out the require message.
	char* message = "Entering foreground-only mode (& is now ignored)\n";
	write(STDOUT_FILENO, message, 49);
	fflush(stdout);
	write(STDOUT_FILENO, prompt, 2);
	fflush(stdout);
	//Change the process to foreground mode.
	Back_Check = 0;
	}
	//If the process is in foreground mode change it to background mode.
	else
	{
	//Print the required message
	char*message = "Exiting foreground-only mode\n";
	write(STDOUT_FILENO, message, 29);
	fflush(stdout);
	write(STDOUT_FILENO, prompt, 2);
	//Change the process to background mode.
	Back_Check = 1;
	}

}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function handles the majority of the built in Commands that we are supposed to have.
void Built_In_Comm(char user_in[2048])
{	

		//Here I am removing any & because they don't matter to the built in commands.
		if(user_in[strlen(user_in) - 1] == '&')
		{
			user_in[strlen(user_in) - 1] = '\0';
		}	

			//Change the directory if the user wants to.
			else if((user_in[0] == 'c') && (user_in[1] == 'd'))
			{
				//Pointer to the name of the directory we will change to.
				char* Direct;
				//If the user doesn't name a directory after cd take them to the home directory.
				if(user_in[2] != ' ')
				{
				//getenv("HOME") allows me to get the name of my home directory.
				Direct = getenv("HOME");
				chdir(Direct);
				}
				//Otherwise change to the directory specified by the user.
				else
				{
				//strtok() is allowing me to grab a certain part of my string, in this case I am grabbing the name of the user input directory.
				Direct = strtok(user_in, " ");
				Direct = strtok(NULL, "\0");
				chdir(Direct);
				}
			}
			//If it is a comment don't do anything.
			else if(user_in[0] == '#')
			{
				printf("\n");
				fflush(stdout);
			}
return;	
}

/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function prints the status of the exit or signal termination.
int User_Status()
{
	//If it is an exit value print here.
	if(Sig_Status == 0 || Sig_Status == 1)
	{
	printf("exit value %d\n", Sig_Status);
	fflush(stdout);
	}
	//If it is a signal termination print here.
	else
	{
	printf("terminated by signal %d\n", Sig_Status);
	fflush(stdout);
	}
return 0;
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function creates a child process for many of the user inputs, runs them through execlp which ends up killing the child process and returning the desired values.
int User_Ls(char C_command[2048], char user_in[2048], int Which_exec)
{
//Create a pid variable for our child process.
pid_t spawnPid = -5;
int rand;
//Fork off a child process
spawnPid = fork();

	switch (spawnPid)
	{
		//If there was an error print it out and break.
		case -1:
		{
			perror("Hull Breach!\n"); exit(1); 
			break;
		}
		//Here is where we are using execlp to output to the user whatever the results from what they chose to input.
		case 0:
		{
			//If the user input is more than one word use this.
			if(Which_exec == 0)
			{
			execlp(C_command, C_command, user_in, NULL);
			perror("Child: exec failure!\n");
			exit(1);
			break;
			}
			//Otherwise use this.
			else if(Which_exec == 1)
			{
			execlp(C_command, C_command, NULL);
			perror("Child: exec failure!\n");
			exit(1);
			break;
			}
			//Set exit status to 1 if it didn't work.
			Sig_Status = 1;
			break;
		}
		//Here I am waiting until the child process has completed.
		default:
		{
			//Ignore the SIGCHLD signal so it won't prind out the process end.
			signal(SIGCHLD, SIG_IGN);	
			//Wait for the child process to end.
			pid_t actualPid = waitpid(spawnPid, &rand, 0);
			break;
		}
	}	

return rand;
}	
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
void  What_Input(char user_in[2048], char input2[2048], char If_money[2048])
{
int rand;
int i = 0;
int j;
char* One_Arg;
char* From_Here;
char* To_Here;
char* Args[512];
int Childd;
char* First_Arg;
char* Direct;
//Here I am breaking the user_in string in to different pieces using strtok.
	One_Arg = strtok(user_in, " ");
	//Run until it hits a null terminator.
	while(One_Arg != '\0')
	{
		Args[i] = One_Arg;
		One_Arg = strtok(NULL, " ");
		//i counts how many arguments there are.
		i++;
	}
	//Set the arguments at i to null so I know where to stop.
	Args[i] = '\0';
	//Grab the first argument.
	First_Arg = Args[0];

	From_Here = Args[0];
	To_Here = Args[1];
	//Here I am checking to see if the file is a good or bad file.
	if(strcmp(From_Here, "test") == 0 && strcmp(To_Here, "-f") ==0)
	{
		//If the file is a badfile or if it is a directory
		To_Here = Args[2];
		if(access(To_Here, F_OK) != -1)
		{		
			Sig_Status = 0;
		}
		//If it is a directory change to that directory
		else
		{
			//	Direct = strtok(user_in, " ");
			//	Direct = strtok(NULL, "\0");
			//	chdir(Direct);
			Sig_Status = 1;
		}
	}

	//If the user typed in something other than these arguments check to see if it is a valid file or directory.
	if(i == 1 && strcmp(First_Arg, "echo") != 0 && strcmp(First_Arg, "ls") != 0 && strcmp(First_Arg, "pwd") != 0 && strcmp(First_Arg, "ps") != 0 && strcmp(First_Arg, "cd") != 0)
	{
		//Here is where I check to see if it is a valid file or directory.
		if(access(First_Arg, F_OK) == -1)
		{
			//If it is invalid print this
			printf("%s: no such file or directory\n", From_Here);
			fflush(stdout);
			Sig_Status = 1;
		}

	}
	
	else{
	//Here I am going through each argument and looking for ceratin things.
	for(j = 0; j < i; j++)
	{
	//Set a char pointer to the argument so I can strcmp it.
	One_Arg = Args[j];
	
		//If the argumnet is equal to a greater than sign do this.
		if(strcmp(One_Arg, ">") == 0)
		{
			//Set char* to the arguments before and after the sign.
			From_Here = Args[j - 1];
			To_Here = Args[j + 1];
			//Check to see what the first argument was, if it is a file see if it exists.
				if(strcmp(From_Here, "ls") == 0 || strcmp(From_Here, "wc") == 0 || strcmp(From_Here, "cat") == 0 || access(From_Here, F_OK) != -1)
				{
				//This function correctly move around the contents from files.
				Input_From(From_Here, To_Here, First_Arg);
				}
				//If the file doesn't exist tell the user.
				else
				{
				printf("%s: no such file or directory\n", From_Here);
				fflush(stdout);
				}
		}
		//If the argument is a lesser than sign run this.
		else if(strcmp(One_Arg, "<") == 0 && i <= 3)
		{
		//Two char* that holds the arguments before and after the sign.
		From_Here = Args[j - 1];
		To_Here = Args[j + 1];
		//Check to see what the user wants to do with the argument and if the file exists.
			if(strcmp(From_Here, "ls") == 0 || strcmp(From_Here, "wc") == 0 || strcmp(From_Here, "cat") == 0 || access(From_Here, F_OK) != -1)
			{
			//This function correctly moves around the contents from files.
			Output_To(From_Here, To_Here);
			}
			//If the file doesn't exist tell the user.
			else
			{
			printf("%s: no such file or directory\n", From_Here);
			fflush(stdout);
			}

		}
		//If the first argument is to make a directory, then do this.
		else if(strcmp(One_Arg, "mkdir") == 0)
		{
		//Grab the name of the directory the user wants made.
		From_Here = strtok(If_money, " ");
		From_Here = strtok(NULL, "\0");
		//Here we are sending the directory name and mkder to our User_Ls function which will create the directory.
		User_Ls("mkdir", From_Here, 0);
		}
		//If the first argument is to cat, then do this.
		else if(strcmp(One_Arg, "cat") == 0)
		{
		//Grab the file that they would like to cat.
		From_Here = strtok(input2, " ");
		From_Here = strtok(NULL, "\0");
		//Send that file and cat to the User_Ls function which will cat the file.
		User_Ls("cat", From_Here, 0);
		}
		//If the user would like to echo a statement do this.
		else if(strcmp(One_Arg, "echo") == 0)
		{
		//Grab what the user would like to echo.
		From_Here = strtok(input2, " ");
		From_Here = strtok(NULL, "\0");
		//Send the statement and echo to the User_Ls function which will echo the statement.  
		User_Ls("echo", From_Here, 0);
		}
		
	}
	}
	
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function deals with the input redirection it was just poorly named because of many lost hours of sleep
int Output_To(char* From_Here, char* To_Here)
{
	int rand;
//Create a pid variable for our child process.
pid_t spawnPid = -5;


if(access(To_Here, F_OK) != -1)
{
//Fork off a child process
spawnPid = fork();

	switch (spawnPid)
	{
		//If there is an error.
		case -1:
		{
			perror("Hull Breach!\n"); exit(1); 
			break;
		}
		//Here is the child process
		case 0:
		{
		int result;
		//Open the target file for read only purposes
		int Target_File = open(To_Here, O_RDWR, 0644);
			if(strcmp(From_Here, "wc") == 0)
			{
			//Dup2 to standard in
			result = dup2(Target_File, 0);
			//Do whatever the first argument said to the open file.
			execlp(From_Here, From_Here, NULL);
			perror("Child: exec faliure!\n");
			break;	
			}
			else
			{
			//Dup2 to standard in
			result = dup2(Target_File, 0);
			//Cat the contents to the open file.
			execlp("cat", "cat", NULL);
			perror("Child: exec failure!\n");
			close(Target_File);
			break;
			}
		close(Target_File);
		}
		//Here I am waiting until the child process has completed.
		default:
		{
			//Ignore the SIGCHILD signal because it will print out the children ending.
			signal(SIGCHLD, SIG_IGN);
			//Wait for the process to end.
			pid_t actualPid = waitpid(spawnPid, &rand, 0);
			break;
		}
	}
}
else
{
//If the file can't be opened tell the user.
printf("cannot open %s for input\n", To_Here);
Sig_Status = 1;
}
return rand;
}
/***********************************************************************************************************************************************************/


/***********************************************************************************************************************************************************/
//This function simply kills the sleep process
void Proc_kill()
{
pid_t spawnPid = -5;
int rand;
//Fork off a child process.
spawnPid = fork();
	switch(spawnPid)
	{
		case -1:
		{
		//If there is an error print it.
			perror("Hull Breach!\n"); exit(1);
			break;
		}	
		case 0:
		{
		//Kill the sleep process using pkill
			execlp("pkill", "pkill", "sleep", NULL);
			break;
		}
		default:
		{
		//Wait until the child is done.
		pid_t pid = waitpid(spawnPid, &rand, 0);
		break;
		}
	}
}
/***********************************************************************************************************************************************************/



/***********************************************************************************************************************************************************/
//This process actually works with the command file as an output but it is poorly named because a lack of sleep.
int Input_From(char* From_Here, char* To_Here, char* First_Arg)
{
//Create a pid variable for our child process.
pid_t spawnPid = -5;
int rand;

//Fork off a child process
spawnPid = fork();

	switch (spawnPid)
	{
		//If there is an error.
		case -1:
		{
			perror("Hull Breach!\n"); exit(1); 
			break;
		}
		//Here is the child process
		case 0:
		{
		int result;	
		//Here I am opening the file the user wanted for reading.
		int Target_File = open(To_Here, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		//Here I am checking to see what the user wanted to do with that file.
			if(strcmp(From_Here, "ls") == 0 || strcmp(From_Here, "wc") == 0 || strcmp(From_Here, "cat") == 0)
			{
			result = dup2(Target_File, 1);
			execlp(From_Here, From_Here, NULL);
			}
		//Here I am creating a new file if the user wanted to send information to that file.
			else 
			{
			int Source_File = open(From_Here, O_RDONLY, 0644);
			//Here I am sending our target file to standard in.
			result = dup2(Source_File, 0);
			//Here I am sending our source file to standard out.
			result = dup2(Target_File, 1);
			//Here I am using the first argument to tell what should be sent from one file to another.
			execlp(First_Arg, First_Arg, NULL);	
			close(Target_File);
			}

		close(Target_File);
		exit(0);
		break;
		}
		//Here I am waiting until the child process has completed.
		default:
		{
			//Ignore the SIGCHLD signal because it will print out a statement otherwise.
			signal(SIGCHLD, SIG_IGN);
			pid_t actualPid = waitpid(spawnPid, &rand, 0);
			break;
		}
	
	}
return rand;
}
/***********************************************************************************************************************************************************/

/***********************************************************************************************************************************************************/
void main()
{
//This char holds the user input.
char user_in[2048];

char If_money[2048];
//This tells the program when to exit.
int exitor = 1;
//Holds the exit status
int money = 0;
int* Num_Args[512];
int* Space_Found;


	//While loop to run our command line.
while(1)
{
	user_in[0] = '1';
//These are my signal catches
  signal(SIGCHLD, SIGCHLD_Handler);
  fflush(stdout);
  signal(SIGINT, SignalTermination);
  signal(SIGTSTP, STP_Handler);

	fflush(stdout);
	//Here is our prompt.
	printf(": ");
	fflush(stdout);
	//Get the user input.
 fgets(user_in, sizeof(user_in), stdin);

 	 //Removing the new line character.
	user_in[strlen(user_in)-1] = '\0';
//If the user has actually input smething run this
 if(user_in[0] != '\0'){	
	//If the last two parts of the user input are $$
	if(user_in[strlen(user_in) -1] == '$' && user_in[strlen(user_in) -2] == '$')
	{
	//Get our pid and set it to the end of the user input with snprintf.
	int my_pid = getpid();
	user_in[strlen(user_in) - 2] = '\0';
	snprintf(If_money, 2048, "%s%d", user_in, my_pid);
	//This tells me if the user had $$ at the end.
	money = 1;
	}



	//Here I am checking to see if the user wanted to change directories
		if((user_in[0] == '#') || ((user_in[0] == 'c') && (user_in[1] == 'd')))
		{
			//Check to see if the $$ was added at the end of the user input
			if(money == 0)
			{
			Built_In_Comm(user_in);
			}
			else if(money == 1)
			{
			Built_In_Comm(If_money);
			}
		}


		//Here I am checking to see if the user wanted the exit or termination status.
		else if((strcmp(user_in, "status") == 0) || (strcmp(user_in, "status &") == 0))
		{	
			User_Status();
		}
		//Here we are checking to see if the user input exit.
		else if((strcmp(user_in, "exit") == 0) || (strcmp(user_in, "exit &") == 0))
		{
			//Break out of the loop.
			break;
		}
		//If the user wants to see the contents of the directory here run our User_ls function.
		else if ((strcmp(user_in, "ls") == 0))
		{
			User_Ls("ls", user_in, 1);
		}
		//If the user wants to see their directory run it through our User_ls function.
		else if ((strcmp(user_in, "pwd") == 0))
		{
			User_Ls("pwd", user_in, 1);
		}
		//If the user wants to see current process status'
		else if ((strcmp(user_in, "ps") == 0))
		{
			User_Ls("ps", user_in, 1);
		}
		//If the user wants to see the date.
		else if((strcmp(user_in, "date") == 0))
		{
			User_Ls("date", user_in, 1);
		}
		//If the user wants to kill the process.
		else if (user_in[0] == 'p' && user_in[1] == 'k' && user_in[2] == 'i' && user_in[3] == 'l' && user_in[4] == 'l' && user_in[5] == ' ')
		{
			Proc_kill();
			fflush(stdout);	
		}
		//If the user choses to enter kill -SIGSTP rather then entering control-z
		else if((strcmp(user_in, "kill -SIGTSTP ") == 0))
		{
			//If the process allows background don't
			if(Back_Check == 1)
			{
			//Print out the required sentence.
			printf("Entering foreground-only mode (& is now ignored)\n");
			fflush(stdout);
			fflush(stdout);
			//Set the process to foreground only.
			Back_Check = 0;
			}
			//Change the process back to background otherwise.
			else
			{
			printf("Exiting foreground-only mode\n");
			fflush(stdout);

			Back_Check = 1;
			}
				
		}
		//If the user enters sleep
		else if (user_in[0] == 's' && user_in[1] == 'l' && user_in[2] == 'e' && user_in[3] == 'e' && user_in[4] == 'p' && user_in[5] == ' ')
		{
			//Check if it is in background
			if(user_in[strlen(user_in) - 1] == '&' && Back_Check == 1)
			{
			Sleeper(user_in);
			}
			//Otherwise run the foreground function.
			else
			{
			Sleeper2(user_in);
			}
		}

		else
		{
		//Clone the array so I have two to work with (helpful when using strtok).
		char clone_arr[2048];
		int i;
		for(i = 0; i < sizeof(user_in); i++)
		{
		clone_arr[i] = user_in[i];
		}
		//Run my What_Input function to figure out what was input.	
		What_Input(user_in, clone_arr, If_money);		
		}

	}
	else
	{
	}
}
	
}
