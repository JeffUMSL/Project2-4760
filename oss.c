#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
	int opt;

	// Variables to store parsed values
	int total_children = 0;
	int sim_limit = 0;
	int pass_to_user = 0;

	//The proc -n parameter stands for number of total children to launch,
        //iter -t is the number to pass to the user process
	//simul -s parameter indicates how many children to allow to run simultaneously.
        //-h parameter, it should simply output a help message (indicating how it is supposed to be run) and then terminating

	const char optstr[] = "hn:s:t:";
	
	while((opt= getopt(argc, argv, optstr)) != -1)
	{
		switch(opt){
			case 'h':
				cout << "Usage: " << argv[0] << " -n [children] -s [simultaneous] -t [value]\n";
                                cout << "  -n : Total children to launch\n";
                                cout << "  -s : Max simultaneous children allowed to run\n";
                                cout << "  -t : Number to pass to each user process\n";
                                return EXIT_SUCCESS;


			case 'n':
				total_children = atoi(optarg);
                                cout << "total children to launch " << total_children << endl;
                                break;

			case 't':
				pass_to_user = atoi(optarg);
                                cout << "number to pass to user process " << pass_to_user << endl;
                                break;

                        case 's':
				sim_limit = atoi(optarg);
                                cout << "how many children to allow to run simultaneously " << sim_limit << endl;
                                break;

			default:/* '?' */
				cout << "Invalid option" << endl;
                                return EXIT_FAILURE ;


		}
	}
	cout << endl;

	
	// Given n is max process to launch
	// simul_limit is processes runnning simultaneously
	// c is current processes running, starting at 0
	// total is total processes that have launched, starting at 0
	
	int c = 0;    
	int total = 0;

	while(c < sim_limit && total < total_children)
	{
		//restrict running more than 3 processes simul
		if(sim_limit > 3) 
		{ 
			cout << "sim_limit is > 3\n";
			break;
		}

		/*Launch new child process
		oss outputs message of new child launched
		c++;
		total++;*/
		
		// This is where the child process splits from the parent
		pid_t childPid = fork();
        	if (childPid == 0) {
			printf("I am a child but a copy of parent! My parent's PID is %d, and my PID is %d\n",getppid(), getpid());

                	// Following code launches the child process with some arguments
                	// I show some different ways to set up your strings

                string iter_str = to_string(pass_to_user);

                char arg0[] = "./user";

                // Allocate a buffer large enough for your number, then copy the string value into it
                char arg1[16];
                snprintf(arg1, sizeof(arg1), "%s", iter_str.c_str());

                // Bundle their pointers together into the final arguments array
                char* args[] = { arg0, arg1, NULL };

                execvp(args[0], args);

                fprintf(stderr,"Exec failed, terminating\n");
                exit(1);
        }
        else if(childPid > 0) {
                printf("I'm a parent! My pid is %d, and my child's pid is %d \n",getpid(), childPid);
                c++;
                total++;
        }
	else{
		cout << "Error: Fork Failed";
		return EXIT_FAILURE;
	
	}

}

	while ( total < total_children) {
		/*wait();
		Launch new child process
		total++;*/

		//restrict running more than 3 processes simul
                if(sim_limit > 3)
                {
                        break;
                }
		
		wait(NULL);
		
		// This is where the child process splits from the parent
		pid_t childPid = fork();
        	if (childPid == 0) {
                	printf("I am a child but a copy of parent! My parent's PID is %d, and my PID is %d\n",getppid(), getpid());

                	// Following code launches the child process with some arguments
                	// I show some different ways to set up your strings

                	string iter_str = to_string(pass_to_user);

                	char arg0[] = "./user";

                	// Allocate a buffer large enough for your number, then copy the string value into it
                	char arg1[16];
               		snprintf(arg1, sizeof(arg1), "%s", iter_str.c_str());

                	// Bundle their pointers together into the final arguments array
                	char* args[] = { arg0, arg1, NULL };

                	execvp(args[0], args);



                	fprintf(stderr,"Exec failed, terminating\n");
                	exit(1);
        	}

        	else if(childPid > 0) {
                	printf("I'm a parent! My pid is %d, and my child's pid is %d \n",getpid(), childPid);
                	total++;
        	}
}

	// chatgpt, run into error when using only wait(NULL)
	while(wait(NULL) > 0) {}

	printf("oss is now ending.\n");
	return EXIT_SUCCESS;
}

