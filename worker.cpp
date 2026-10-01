#include <iostream>
#include <string>
#include <unistd.h> // getpid(), getppid(), and sleep()
#include <cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

const int BUFF_SZ = sizeof(int) * 2;

int main(int argc, char** argv) {
// Generate the same shared memory key as oss.cpp
    key_t shm_key = ftok("oss.cpp", 0);

    if (shm_key == -1)
    {
        cerr << "Child: Error in ftok\n";
        return 1;
    }

    // Get the existing shared memory segment
    int shm_id = shmget(shm_key, BUFF_SZ, 0700);

    if (shm_id == -1)
    {
        cerr << "Child: Error in shmget\n";
        return 1;
    }

    // Attach to the shared memory
    int* clock = static_cast<int*>(shmat(shm_id, nullptr, 0));

    if (clock == reinterpret_cast<int*>(-1))
    {
        cerr << "Child: Error in shmat\n";
        return 1;
    }

    // Access the two parts of the shared clock
    int* sec = &clock[0];
    int* nano = &clock[1];

    // Display the values set by OSS
    cout << "Child:\t sec " << *sec
         << " , nanosecond " << *nano << '\n';

    // Change the shared clock
    cout << "Changing clock to 5 , 13\n";

    *sec = 5;
    *nano = 13;

    // Display the new values
    cout << "Child:\t sec " << *sec
         << " , nanosecond " << *nano << '\n';

    cout << "Child terminating\n";

    // Detach from shared memory
    shmdt(clock);
    clock = nullptr;


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

