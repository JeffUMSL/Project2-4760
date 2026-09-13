#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include <iterator>
#include<iostream>
using namespace std;

int main(int argc, char** argv) {
		printf("Hello from user.c, a new executable!\n");
		printf("My process id is: %d\n",getpid());
		printf(" I got %d arguments: \n", argc);

		int i;
		for (i =0; i < argc; i++)
				printf("|%s| ", argv[i]), "\n";


		//convert string to int
		int num_iter = stoi(argv[argc - 1]);
		for(int i = 0; i < num_iter; i++)
		{
			cout << "\nUSER PID: " << getpid() << " PPID: " << getppid() << " Iteration: " << i << " before sleeping" << endl;
			sleep(1);
			 cout << "USER PID: " << getpid() << " PPID: " << getppid() << " Iteration: " << i << " after sleeping" << endl;
		
		}


		printf("\nuser is now ending.\n");

		sleep(3);
		return EXIT_SUCCESS;
}

