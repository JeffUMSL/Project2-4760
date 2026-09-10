#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>

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

			string arg0 = "./child";
			string arg1 = "Hello";
			string arg2 = "there";
			string arg3 = "exec";
			string arg4 = "is";
			string arg5 = "neat";
			execlp(arg0.c_str(),arg0.c_str(),arg1.c_str(),arg2.c_str(),arg3.c_str(),arg4.c_str(),arg5.c_str(),(char *)0);			

			fprintf(stderr,"Exec failed, terminating\n");
			exit(1);	

		} else {
			printf("I'm a parent! My pid is %d, and my child's pid is %d \n",
				getpid(), childPid);
			//sleep(1);
			wait(0);
		}
		printf("oss is now ending.\n");
		return EXIT_SUCCESS;
}

