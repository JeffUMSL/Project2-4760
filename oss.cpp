#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<iostream>
#include<string>
#include<cstdlib>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <signal.h>

using namespace std;

const int BUFF_SZ = sizeof(int) * 2;

struct PCB
{
    int occupied;             // either true or false
    pid_t pid;                // PID of child
    int startSeconds;         // simulated start time
    int startNano;
    int endingTimeSeconds;    // estimated time it should end
    int endingTimeNano;       // estimated time it should end
};

PCB processTable[20];

int shm_id = -1;
int* sharedClock = nullptr;


volatile sig_atomic_t signalReceived = 0;

void signalHandler(int sig)
{
    signalReceived = sig;

    // kill signal to all children based on their PIDs in process table
    for (int i = 0; i < 20; i++)
    {
        if (processTable[i].occupied == 1)
        {
            kill(processTable[i].pid, SIGTERM);
	}
    }

    // free up shared memory
    if(sharedClock != nullptr)
    {
        shmdt(sharedClock);
	sharedClock = nullptr;
    }
    if(shm_id != -1)
    {
        shmctl(shm_id, IPC_RMID, nullptr);
	shm_id = -1;
    }

    exit(1);
}

int main(int argc, char** argv) {
    int opt;

    // Variables to store parsed values
    int total_children = 0;
    int sim_limit = 0;
    float simulated_time = 0.0;
    float min_interval = 0.0;

    signal(SIGALRM, signalHandler);
    signal(SIGINT, signalHandler);

    alarm(60);

    std::cout << "OSS starting, ";
    std::cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
    std::cout << "Called with: \n";

    const char optstr[] = "hn:s:t:i:";
	
    while((opt= getopt(argc, argv, optstr)) != -1)
    {
         switch(opt){
	     case 'h':
	         cout << "Usage: " << argv[0] << " -n [children] -s [simultaneous] -t [value]\n";
                 cout << "  -n : Total children to launch\n";
                 cout << "  -s : Max simultaneous children allowed to run\n";
                 cout << "  -t : amount of simulated time, not real time (int or float)\n";
		 cout << "  -i : minimum interval between when you launch child processes (Ex: 0.1)\n"; 
                 return EXIT_SUCCESS;


	     case 'n':
	         total_children = atoi(optarg);
                 cout << "-n " << total_children << endl;
                 break;

	     case 't':
	         simulated_time = atof(optarg);
		 cout << "-t " << simulated_time << endl;
                 break;

	     case 'i':
		 min_interval = atof(optarg);
                 cout << "-i " << min_interval << endl;
                 break;

             case 's':
	         sim_limit = atoi(optarg);
                 cout << "-s " << sim_limit << endl;
                 break;

	     default:/* '?' */
	         cout << "Invalid option" << endl;
                 return EXIT_FAILURE ;


	}
}
	
    cout << endl;

    // simul_limit is processes runnning simultaneously
    // c is current processes running, starting at 0
    // total is total processes that have launched, starting at 0
    
    int c = 0;      // current number of workers running
    int total = 0;      // total number of workers launched

    for (int i = 0; i < 20; i++)
    {
        processTable[i].occupied = 0;
        processTable[i].pid = 0;
        processTable[i].startSeconds = 0;
        processTable[i].startNano = 0;
        processTable[i].endingTimeSeconds = 0;
        processTable[i].endingTimeNano = 0;
    }

    // Generate shared memory key
    key_t shm_key = ftok("oss.cpp", 0);

    if (shm_key == -1)
    {
        std::cerr << "Parent: Error in ftok\n";
        return 1;
    }

    // Create shared memory
    shm_id = shmget(shm_key, BUFF_SZ, 0700 | IPC_CREAT);

    if (shm_id == -1)
    {
        std::cerr << "Parent: Error in shmget\n";
        return 1;
    }

    // Attach shared memory
    int* sharedClock = static_cast<int*>(shmat(shm_id, nullptr, 0));

    if (sharedClock == reinterpret_cast<int*>(-1))
    {
        std::cerr << "Parent: Error in shmat\n";
        return 1;
    }

    // First integer = seconds
    // Second integer = nanoseconds
    int* sec = &sharedClock[0];
    int* nano = &sharedClock[1];

    // Initialize clock
    *sec = 0;
    *nano = 0;

    // convert -i into sec and nano
    int interval_sec = static_cast<int>(min_interval);

    int interval_nano =
    static_cast<int>((min_interval - interval_sec) * 1000000000);

    int next_launch_sec = 0;
    int next_launch_nano = 0;

    // process table should first print at 0.5 seconds
    int next_print_sec = 0;
    int next_print_nano = 500000000;

    int totalRunSec = 0;
    int totalRunNano = 0;


    // Run OSS until all children have been launched
    // and all launched children have finished
    while ((total < total_children || c > 0) && signalReceived == 0)
    {
	if (sim_limit < 1 || sim_limit > 3)
	{
    	    cout << "Error: -s must be between 1 and 3." << endl;
    	    return 1;
	}

        // Advance simulated system clock
        *nano += 1000000; // 1 millisecond


        if (*nano >= 1000000000)
        {
            (*sec)++;
            *nano -= 1000000000;
        }

        // Print process table every 0.5 simulated seconds
        bool print_time =
        (*sec > next_print_sec) ||
        (*sec == next_print_sec &&
         *nano >= next_print_nano);

        if (print_time)
        {
            cout << "\nOSS PID:" << getpid() << " SysClockS: " << *sec
             << " SysClockNano: " << *nano << endl;

            cout << "Process Table:" << endl;

            cout << "Entry\tOccupied\tPID\tStartS\tStartN\t"
             << "EndingTimeS\tEndingTimeNano" << endl;

            for (int i = 0; i < 20; i++)
            {
                cout << i << "\t"
                 << processTable[i].occupied << "\t"
                 << processTable[i].pid << "\t"
                 << processTable[i].startSeconds << "\t"
                 << processTable[i].startNano << "\t"
                 << processTable[i].endingTimeSeconds << "\t"
                 << processTable[i].endingTimeNano
                 << endl;
            }

            // Schedule next print 0.5 simulated seconds later
            next_print_nano += 500000000;

            if (next_print_nano >= 1000000000)
            {
                next_print_sec++;
                next_print_nano -= 1000000000;
            }
}


    // Check if any worker has finished
    int status;

    pid_t finishedPid =
        waitpid(-1, &status, WNOHANG);

    // Check whether enough simulated time has
    // passed since the previous launch
    bool interval_passed = (*sec > next_launch_sec) ||
        (*sec == next_launch_sec &&
         *nano >= next_launch_nano);

    int tableIndex = -1;

    for (int i = 0; i < 20; i++)
    {
        if (processTable[i].occupied == 0)
        {
            tableIndex = i;
            break;
        }
    }

    int childStartSec = *sec;
    int childStartNano = *nano;

    // Launch a worker only if:
    // We still have workers to launch, less than simultaneous limit, parameter -i has passed
    if (total < total_children &&
        c < sim_limit &&
        interval_passed)
    {
        pid_t childPid = fork();

        if (childPid == 0)
        {
            execlp("./worker",
                   "./worker",
                   "5",
                   "500000",
                   static_cast<char*>(nullptr));

            cerr << "Exec failed, terminating\n";
            exit(1);
        }

        else if (childPid > 0)
        {
            /*cout << "I'm a parent! My PID is "
                 << getpid()
                 << ", and my child's PID is "
                 << childPid << endl;*/

            c++;
            total++;

	    processTable[tableIndex].occupied = 1;
            processTable[tableIndex].pid = childPid;

            processTable[tableIndex].startSeconds = childStartSec;
            processTable[tableIndex].startNano = childStartNano;

	    processTable[tableIndex].endingTimeSeconds =
            childStartSec + 5;

            processTable[tableIndex].endingTimeNano =
            childStartNano + 500000;

            if (processTable[tableIndex].endingTimeNano >= 1000000000)
            {
                processTable[tableIndex].endingTimeSeconds++;
                processTable[tableIndex].endingTimeNano -= 1000000000;
            }

            // Determine earliest simulated time
            // at which another worker may launch
            next_launch_sec =
                *sec + interval_sec;

            next_launch_nano =
                *nano + interval_nano;

            if (next_launch_nano >= 1000000000)
            {
                next_launch_sec++;
                next_launch_nano -= 1000000000;
            }
        }

        else
        {
            cout << "Error: Fork Failed\n";
            return EXIT_FAILURE;
        }
    }


if (finishedPid > 0)
{
    c--;

    for (int i = 0; i < 20; i++)
    {
        if (processTable[i].occupied == 1 &&
            processTable[i].pid == finishedPid)
        {
            // Calculate this worker's runtime
            int runSec = *sec - processTable[i].startSeconds;
            int runNano = *nano - processTable[i].startNano;

            if (runNano < 0)
            {
                runSec--;
                runNano += 1000000000;
            }

            // Add it to combined worker runtime
            totalRunSec += runSec;
            totalRunNano += runNano;

            if (totalRunNano >= 1000000000)
            {
                totalRunSec += totalRunNano / 1000000000;
                totalRunNano %= 1000000000;
            }

            // Clear PCB
            processTable[i].occupied = 0;
            processTable[i].pid = 0;
            processTable[i].startSeconds = 0;
            processTable[i].startNano = 0;
            processTable[i].endingTimeSeconds = 0;
            processTable[i].endingTimeNano = 0;

            break;
        }
    }

    cout << "OSS: Worker PID "
         << finishedPid
         << " finished." << endl;
}
}

    if (signalReceived != 0)
    {
        if (signalReceived == SIGALRM)
        {
            cout << "\nOSS: 60 second timeout reached." << endl;
        }
        else if (signalReceived == SIGINT)
        {
            cout << "\nOSS: Ctrl+C received." << endl;
        }

        cout << "OSS: Terminating all child processes." << endl;

        // Reap the terminated children
        while (waitpid(-1, nullptr, 0) > 0)
        {
        }
}

    cout << "\nOSS PID:" << getpid() << " Terminating" << endl;

    cout << total << " workers were launched and terminated" << endl;

    cout << "Workers ran for a combined time of "
    << totalRunSec << " seconds "
    << totalRunNano << " nanoseconds." << endl;


    return EXIT_SUCCESS;
}
