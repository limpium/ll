#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

const int SIZE = 8;

pid_t pid_t_smoker, pid_p_smoker, pid_m_smoker;
int count = 0;

void smoker_t(sem_t* tobacco_sem, sem_t* barman_sem) {
    while(true){
        sem_wait(tobacco_sem);
        std::cout << "T" << std::endl;
        sem_post(barman_sem);
    }
}

void smoker_p(sem_t* paper_sem, sem_t* barman_sem) {
    while(true){
        sem_wait(paper_sem);
        std::cout << "P" << std::endl;
        sem_post(barman_sem);
    }
}

void smoker_m(sem_t* match_sem, sem_t* barman_sem) {
    while(true){
        sem_wait(match_sem);
        std::cout << "M" << std::endl;
        sem_post(barman_sem);
    }
}

void barman(sem_t* tobacco_sem, sem_t* paper_sem, sem_t* match_sem, sem_t* barman_sem) {
    while (true) {
        sem_wait(barman_sem);

        if(count == SIZE) {
            std::cout << "Closing" << std::endl;
            kill(pid_t_smoker, SIGTERM);
            kill(pid_p_smoker, SIGTERM);
            kill(pid_m_smoker, SIGTERM);
            kill(getpid(), SIGTERM);
        }

        char item;
        std::cin >> item;
        ++count;

        switch (item) {
            case 't':
                sem_post(tobacco_sem);
                break;

            case 'p':
                sem_post(paper_sem);
                break;

            case 'm':
                sem_post(match_sem);
                break;

            default:
                std::cout << "Closing!!!" << std::endl;
                kill(pid_t_smoker, SIGTERM);
                kill(pid_p_smoker, SIGTERM);
                kill(pid_m_smoker, SIGTERM);
                kill(getpid(), SIGTERM);
        }
    }
}

int main() {
    int tobacco_shm = shm_open("/tobacco_shm", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(tobacco_shm == -1) {
            std::cerr << "open" << std::endl;
            exit(EXIT_FAILURE);
    }

    int tobacco_trunc = ftruncate(tobacco_shm, sizeof(sem_t));
    if(tobacco_trunc == -1) {
            std::cerr << "ftrunc" << std::endl;
            exit(EXIT_FAILURE);
    }

    int paper_shm = shm_open("/paper_shm", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(paper_shm == -1) {
            std::cerr << "open" << std::endl;
            exit(EXIT_FAILURE);
    }

    int paper_trunc = ftruncate(paper_shm, sizeof(sem_t));
    if(paper_trunc == -1) {
            std::cerr << "ftrunc" << std::endl;
            exit(EXIT_FAILURE);
    }

    int match_shm = shm_open("/match_shm", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(match_shm == -1) {
            std::cerr << "open" << std::endl;
            exit(EXIT_FAILURE);
    }

    int match_trunc = ftruncate(match_shm, sizeof(sem_t));
    if(match_trunc == -1) {
            std::cerr << "ftrunc" << std::endl;
            exit(EXIT_FAILURE);
    }


    int barman_shm = shm_open("/barman_shm", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(barman_shm == -1) {
            std::cerr << "open" << std::endl;
            exit(EXIT_FAILURE);
    }

    int barman_trunc = ftruncate(barman_shm, sizeof(sem_t));
    if(barman_trunc == -1) {
            std::cerr << "ftrunc" << std::endl;
            exit(EXIT_FAILURE);
    }

    sem_t* tobacco_sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, tobacco_shm, 0);
    sem_t* paper_sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, paper_shm, 0);
    sem_t* match_sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, match_shm, 0);
    sem_t* barman_sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, barman_shm, 0);
if(sem_init(tobacco_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(paper_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(match_sem, 1, 0) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_init(barman_sem, 1, 1) < 0)
    {
        std::cerr << "sem_init" << std::endl;
        exit(EXIT_FAILURE);
    }

    pid_t_smoker = fork();
    if (pid_t_smoker == 0) {
        smoker_t(tobacco_sem, barman_sem);
        return 0;
    }

    pid_p_smoker = fork();
    if (pid_p_smoker == 0) {
        smoker_p(paper_sem, barman_sem);
        return 0;
    }

    pid_m_smoker = fork();
    if (pid_m_smoker == 0) {
        smoker_m(match_sem, barman_sem);
        return 0;
    }

    barman(tobacco_sem, paper_sem, match_sem, barman_sem);

    waitpid(pid_t_smoker, NULL, 0);
    waitpid(pid_p_smoker, NULL, 0);
    waitpid(pid_m_smoker, NULL, 0);

    if(sem_destroy(tobacco_sem) < 0)
    {
        std::cerr << "destroy" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(paper_sem) < 0)
    {
        std::cerr << "destroy" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(sem_destroy(match_sem) < 0)
    {
        std::cerr << "destroy" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(sem_destroy(barman_sem) < 0)
    {
        std::cerr << "destroy" << std::endl;
        exit(EXIT_FAILURE);
    }

    munmap(tobacco_sem, sizeof (sem_t));
    munmap(paper_sem, sizeof (sem_t));
    munmap(match_sem, sizeof (sem_t));
    munmap(barman_sem, sizeof (sem_t));


    sem_unlink("/tobacco_shm");
    sem_unlink("/paper_shm");
    sem_unlink("/match_shm");
    sem_unlink("/barman_shm");

    if(close(tobacco_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(paper_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(match_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }
    if(close(barman_shm) < 0)
    {
        std::perror("close");
        exit(errno);
    }

    return 0;
}

