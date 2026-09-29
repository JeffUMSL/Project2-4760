#include <iostream>
#include <string>
#include <unistd.h> // getpid(), getppid(), and sleep()

int main(int argc, char** argv) {

    // Error checking 
    if (argc < 3) {
	std::cerr << "Missing arguments\n";
        return 1;
    }

    std::cout << "Worker starting, ";
    std::cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
    std::cout << "Called with: \n" << "Interval: ";

    for (int i = 1; i < argc; i++) {
	if(i == 1){ std::cout << argv[i] << " seconds, ";}
	else{ std::cout << argv[i] << " nanoseconds";}
        //std::cout << argv[i] << " ";
    }

    // Convert string to int using C++ stoi
    /*int num_iter = std::stoi(argv[1]);
    for (int i = 0; i < num_iter; i++) {
        std::cout << "\nUSER PID: " << getpid() 
                  << " PPID: " << getppid() 
                  << " Iteration: " << i << " before sleeping";
        
        sleep(1);
        
        std::cout << "\nUSER PID: " << getpid() 
                  << " PPID: " << getppid() 
                  << " Iteration: " << i << " after sleeping\n";
    }*/

    std::cout << "\n\nworker is now ending.\n";

    //sleep(3);
    return EXIT_SUCCESS;
}

