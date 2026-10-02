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

using namespace std;

const int BUFF_SZ = sizeof(int) * 2;

int main(int argc, char** argv) {
    int opt;

    // Variables to store parsed values
    int total_children = 0;
    int sim_limit = 0;
    //int pass_to_user = 0;
    float simulated_time = 0.0;
    float min_interval = 0.0;


    // Error checking
    if (argc < 4) {
        std::cerr << "Missing arguments\n";
       	return 1;
    }

    std::cout << "OSS starting, ";
    std::cout << "PID:" << getpid() << " PPID:" << getppid() << "\n";
    std::cout << "Called with: \n";

    	/*for (int i = 1; i < argc; i++) {
        	if(i == 1){ std::cout << "-n " << argv[i] << endl;}
		else if(i == 2){ std::cout << "-s " << argv[i] << endl;}
		else if(i == 3){ std::cout << "-t " << argv[i] << endl;}
        	else{ std::cout << "-i " << argv[i] << endl;}
    	}*/

	//The proc -n parameter stands for number of total children to launch,
        //iter -t is the
	//simul -s parameter indicates how many children to allow to run simultaneously.
        //-h parameter, it should simply output a help message (indicating how it is supposed to be run) and then terminating
	//-i

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
                 cout << "total children to launch " << total_children << endl;
                 break;

	     case 't':
	         simulated_time = atof(optarg);
		 cout << "amount of simulated time, not real time " << simulated_time << endl;
                 break;

	     case 'i':
		 min_interval = atof(optarg);
                 cout << "min interval between when you launch child processes " << min_interval << endl;
                 break;

             case 's':
	         sim_limit = atoi(optarg);
                 cout << "children to allow to run simultaneously " << sim_limit << endl;
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
    
    
    int c = 0;      // current number of workers running
int total = 0;      // total number of workers launced

    // Generate shared memory key
    key_t shm_key = ftok("oss.cpp", 0);

    if (shm_key == -1)
    {
        std::cerr << "Parent: Error in ftok\n";
        return 1;
    }

    // Create shared memory
    int shm_id = shmget(shm_key, BUFF_SZ, 0700 | IPC_CREAT);

    if (shm_id == -1)
    {
        std::cerr << "Parent: Error in shmget\n";
        return 1;
    }

    // Attach shared memory
    int* clock = static_cast<int*>(shmat(shm_id, nullptr, 0));

    if (clock == reinterpret_cast<int*>(-1))
    {
        std::cerr << "Parent: Error in shmat\n";
        return 1;
    }

    // First integer = seconds
    // Second integer = nanoseconds
    int* sec = &clock[0];
    int* nano = &clock[1];

    // Initialize clock
    *sec = 0;
    *nano = 0;


    // 3. MULTIPLE WORKER CODE
    // ========================================

    //int c = 0;
    //int total = 0;


    // Launch initial workers up to -s
    while (c < sim_limit && total < total_children)
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
            cout << "I'm a parent! My PID is "
                 << getpid()
                 << ", and my child's PID is "
                 << childPid << endl;

            c++;
            total++;
        }

        else
        {
            cout << "Error: Fork Failed\n";
            return EXIT_FAILURE;
        }
    }


    // ========================================
    // 4. LAUNCH REMAINING WORKERS
    // ========================================

    while (total < total_children)
    {
        int status;
        pid_t finishedPid = 0;

        // Keep clock moving while waiting
        // for a worker to finish
        while (finishedPid == 0)
        {
            *nano += 1000;

            if (*nano >= 1000000000)
            {
                (*sec)++;
                *nano -= 1000000000;
            }

            finishedPid = waitpid(-1, &status, WNOHANG);
        }

        // One worker finished
        c--;

        cout << "OSS: Worker PID "
             << finishedPid
             << " finished." << endl;


        // Launch replacement
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
            cout << "I'm a parent! My PID is "
                 << getpid()
                 << ", and my child's PID is "
                 << childPid << endl;

            c++;
            total++;
        }

        else
        {
            cout << "Error: Fork Failed\n";
            return EXIT_FAILURE;
        }
    }


    // ========================================
    // 5. FINISH REMAINING WORKERS
    // ========================================

    while (c > 0)
    {
        *nano += 1000;

        if (*nano >= 1000000000)
        {
            (*sec)++;
            *nano -= 1000000000;
        }

        int status;

        pid_t finishedPid =
            waitpid(-1, &status, WNOHANG);

        if (finishedPid > 0)
        {
            c--;

            cout << "OSS: Worker PID "
                 << finishedPid
                 << " finished." << endl;
        }
    }

    // CLEAN UP

    shmdt(clock);
    clock = nullptr;

    shmctl(shm_id, IPC_RMID, nullptr);

    cout << "oss is now ending." << endl;

    return EXIT_SUCCESS;
}

