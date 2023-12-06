#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

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

    // Allocate shared memory
    double *shared_res = mmap(NULL, num_procs * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    t0 = get_wtime();

    for (int i = 0; i < num_procs; i++) 
    {
        if (fork() == 0) 
        {
            // Child process
            double partial_sum = 0.0;
            unsigned long start = i * (n / num_procs);
            unsigned long end = (i + 1) * (n / num_procs);
            for (unsigned long j = start; j < end; j++) 
            {
                double xi = a + j * h;
                partial_sum += f(xi);
            }
            shared_res[i] = partial_sum * h;
            exit(0);
        }
    }

    // Wait for child processes to finish
    for (int i = 0; i < num_procs; i++) 
    {
        wait(NULL);
    }

    // Sum up the results
    double res = 0.0;
    for (int i = 0; i < num_procs; i++) 
    {
        res += shared_res[i];
    }

    t1 = get_wtime();

    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds\n",
           res, fabs(res-ref), fabs(res-ref)/ref, t1-t0);

    // Cleanup
    munmap(shared_res, num_procs * sizeof(double));
    return 0;
}