#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
		pid_t childPid = fork(); // This is where the child process splits from the parent
		if (childPid == 0) {
			printf("I am a child but a copy of parent! My parent's PID is %d, and my PID is %d\n",
				getppid(), getpid());

			// Following code launches the child process with some arguments
			// I show some different ways to set up your strings

			/* ways to do it in C
			char* args[] = {"./child", "Hello", 
						"there", "exec", "is", "neat"};
			execvp(args[0], args);
    		*/

			/* Another way to do it in C
			execlp(args[0],args[0],args[1],args[2],args[3],args[4],args[5],(char *)0);
			*/

			/*string arg0 = "./user";
			string arg1 = "Hello";
			string arg2 = "there";
			string arg3 = "exec";
			string arg4 = "is";
			string arg5 = "neat";
			execlp(arg0.c_str(),arg0.c_str(),arg1.c_str(),arg2.c_str(),arg3.c_str(),arg4.c_str(),arg5.c_str(),(char *)0);*/

			fprintf(stderr,"Exec failed, terminating\n");
			exit(1);

		} else {
			printf("I'm a parent! My pid is %d, and my child's pid is %d \n",
				getpid(), childPid);
			//sleep(1);
			wait(0);
		
		}

		int opt;

		// Variables to store your parsed values
   int total_children = 0;
   int sim_limit = 0;
   int pass_to_user = 0;

		//The proc -n parameter stands for number of total children to launch, 
		//iter -t is the number to pass to the user process and the simul -s
		//parameter indicates how many children to allow to run simultaneously.
		//-h parameter, it should simply output a help message (indicating how it is supposed to be run) and then terminating
		
		const char optstr[] = "hn:s:t:";
		while((opt= getopt(argc, argv, optstr)) != -1)
		{
			switch(opt)
			{
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





		printf("oss is now ending.\n");
		return EXIT_SUCCESS;
}

