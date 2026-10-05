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
    }
    cout << endl;


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

    int starting_sec = *sec;
    int last_sec = *sec;

    int interval_sec = stoi(argv[1]);
    int interval_nano = stoi(argv[2]);

    int target_sec = *sec + interval_sec;
    int target_nano = *nano + interval_nano;

    if (target_nano >= 1000000000)
    {
        target_sec++;
        target_nano -= 1000000000;
    }

    
    std::cout << "\nWorker, ";
    std::cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
    cout << "SysClockS: " << *sec << " SysClockNano: " << *nano << " TermTimeS: " << target_sec << " TermTimeNano: " << target_nano << endl;
    cout << "--Just Starting\n\n";

    while (*sec < target_sec || (*sec == target_sec && *nano < target_nano))
    {
	 if (*sec != last_sec)
         {
	     int seconds_passed = *sec - starting_sec;
             // check the shared system clock
             cout << "Worker, ";
             cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
             cout << "SysClockS: " << *sec << " SysClockNano: " << *nano << " TermTimeS: " 
		     << target_sec << " TermTimeNano: " << target_nano << endl;
	     cout << "--" << seconds_passed << " seconds have passed since starting" << endl << endl;

	     last_sec = *sec;
	 }
    }

    std::cout << "Worker, ";
    std::cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
    cout << "SysClockS: " << *sec << " SysClockNano: " << *nano << " TermTimeS: " << target_sec << " TermTimeNano: " << target_nano << endl;
    cout << "--Terminating\n\n";

    // Display the values set by OSS
    cout << "Child:\t sec " << *sec
         << " , nanosecond " << *nano << '\n';

    // Display the new values
    cout << "Child:\t sec " << *sec
         << " , nanosecond " << *nano << '\n';

    cout << "Child terminating\n";

    // Detach from shared memory
    shmdt(clock);
    clock = nullptr;

    std::cout << "\n\nworker is now ending.\n";

    //sleep(3);
    return EXIT_SUCCESS;
}
