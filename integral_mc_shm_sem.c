#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>


double get_wtime(void) 
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

inline double f(double x) 
{
    return sin(cos(x));
}

int main(int argc, char *argv[]) 
{
    double a = 0.0;
    double b = 1.0;
    unsigned long n = 24e7; // or e8
    const double h = (b-a)/n;
    const double ref = 0.73864299803689018;
    double t0, t1;
    int num_procs = 4; // Default number of processes

    if (argc == 3) 
    {
        n = atol(argv[1]);
        num_procs = atoi(argv[2]);
    }

    long tseed[num_procs];
    for(int i=0; i<num_procs; i++)
    {
        tseed[i]=rand()%100;
    }

    // Allocate shared memory for result and semaphore
    double *shared_res = mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize shared memory and semaphore
    *shared_res = 0.0;
    sem_init(sem, 1, 1); // Initialize semaphore for shared resource

    t0 = get_wtime();

    for (int i = 0; i < num_procs; i++) 
    {
        if (fork() == 0) 
        {
            // Child process
            srand48(tseed[i]);
            double partial_sum = 0.0;
            unsigned long start = i * (n / num_procs);
            unsigned long end = (i + 1) * (n / num_procs);
            for (unsigned long j = start; j < end; j++) 
            {
                double xi=drand48();
                partial_sum += f(xi);
            }

            sem_wait(sem); // Lock the semaphore
            *shared_res += partial_sum * h;
            sem_post(sem); // Unlock the semaphore

            exit(0);
        }
    }

    // Wait for child processes to finish
    for (int i = 0; i < num_procs; i++) 
    {
        wait(NULL);
    }

    t1 = get_wtime();

    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds\n",
           *shared_res, fabs(*shared_res-ref), fabs(*shared_res-ref)/ref, t1-t0);

    // Cleanup
    sem_destroy(sem);
    munmap(sem, sizeof(sem_t));
    munmap(shared_res, sizeof(double));
    return 0;
}
