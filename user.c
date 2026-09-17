#include <iostream>
#include <string>
#include <unistd.h> // getpid(), getppid(), and sleep()

int main(int argc, char** argv) {
    std::cout << "Hello from user.c, a new executable!\n";
    std::cout << "My process id is: " << getpid() << "\n";
    std::cout << " I got " << argc << " arguments: \n";

    for (int i = 0; i < argc; i++) {
        std::cout << "|" << argv[i] << "| \n";
    }

    // Error checking
    if (argc < 2) {
        std::cerr << "user needs another argument\n";
        return 1;
    }

    // Convert string to int using C++ stoi
    int num_iter = std::stoi(argv[1]);
    for (int i = 0; i < num_iter; i++) {
        std::cout << "\nUSER PID: " << getpid() 
                  << " PPID: " << getppid() 
                  << " Iteration: " << i << " before sleeping";
        
        sleep(1);
        
        std::cout << "\nUSER PID: " << getpid() 
                  << " PPID: " << getppid() 
                  << " Iteration: " << i << " after sleeping\n";
    }

    std::cout << "\nuser is now ending.\n";

    sleep(3);
    return EXIT_SUCCESS;
}



/*#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<iterator>
#include<iostream>

int main(int argc, char** argv) {
	printf("Hello from user.c, a new executable!\n");
	printf("My process id is: %d\n",getpid());
	printf(" I got %d arguments: \n", argc);

	int i;
	for (i =0; i < argc; i++)
		printf("|%s| ", argv[i]), "\n";
		
	//error checking
	if(argc < 2)
	{
		fprintf(stderr, "user needs another argument\n");
		return 1;
	}


	//convert string to int
	int num_iter = stoi(argv[1]);
	for(int i = 0; i < num_iter; i++)
	{
		printf("\nUSER PID: %d PPID: %d Iteration: %d before sleeping\n", getpid(), getppid(), i);
		sleep(1);
		printf("\nUSER PID: %d PPID: %d Iteration: %d after sleeping\n", getpid(), getppid(), i);
	}


	printf("\nuser is now ending.\n");

	sleep(3);
	return EXIT_SUCCESS;
}*/

