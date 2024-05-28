#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include <vector>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <semaphore.h>

const int phil_num = 5;

void philosoph(std::size_t philNum, sem_t* sems) {
    while(true) {
        int priorFork = philNum == 4 ? 0 : philNum;
        int nonPriorFork = philNum == 4 ? 4 : philNum + 1;
        std::string eating = std::to_string(philNum) + ": i am eating\n";
        std::string done = std::to_string(philNum) + ": i am done\n";

        sleep(6);

        if (sem_wait(&sems[priorFork]) < 0) {
            std::cerr << "wait";
            exit(EXIT_FAILURE);
        }

        if (sem_trywait(&sems[nonPriorFork]) == -1) {
            if (sem_post(&sems[priorFork]) < 0) {
                std::cerr << "post";
                exit(EXIT_FAILURE);
            }
        } else {
            std::cout << eating;
            sleep(3);

            if (sem_post(&sems[priorFork]) < 0) {
                std::cerr << "post";
                exit(EXIT_FAILURE);
            }
            if (sem_post(&sems[nonPriorFork]) < 0) {
                std::cerr << "post";
                exit(EXIT_FAILURE);
            }

            std::cout << done;
        }

    }
}

int main() {
    sem_t* sems = (sem_t*)mmap(NULL, phil_num * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    std::vector<int> pids(phil_num);

    for(int i = 0; i < phil_num; ++i) {
        if(sem_init(&sems[i],1, 1) < 0) {
            std::cerr << "init";
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < pids.size(); ++i) {
        pids[i] = fork();

        if(pids[i] == 0) {
            philosoph(i, sems);
            exit(0);
        }
    }

    for(int x : pids) {
        if(waitpid(x,nullptr,0) == -1) {
            std::cerr << "wait";
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < phil_num; ++i) {
        if(sem_destroy(&sems[i]) < 0) {
            std::cerr << "destroy";
            exit(EXIT_FAILURE);
        }
    }

    munmap(sems, phil_num * sizeof(sem_t));

    return 0;
}
