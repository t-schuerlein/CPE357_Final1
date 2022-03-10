#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

// Include statements I added
#include <sys/shm.h>
#include <time.h>

void synch(int par_id, int par_count, int *ready)
{
    int synchid = ready[par_count] + 1;
    ready[par_id] = synchid;
    int breakout = 0;
    while (1)
    {
        breakout = 1;
        for (int i = 0; i < par_count; i++)
        {
            if (ready[i] < synchid)
            {
                breakout = 0;
                break;
            }
        }
        if (breakout == 1)
        {
            ready[par_count] = synchid;
            break;
        }
    }
}

void sort_pair(int *arr, int par_id, int par_count, int isEven)
{

    for (int i = (2 * par_id) + isEven; i < 31; i += (2 * par_count))
    {
        int left = arr[i];
        int right = arr[i + 1];

        if (left > right)
        {
            arr[i] = right;
            arr[i + 1] = left;
        }
    }
}

//************************************************************************************************************************
int main(int argc, char *argv[])
{

    printf("hi from program %s\n", argv[1]);

    int par_id = 0;    // the parallel ID of this process
    int par_count = 1; // the amount of processes
    int *arr;  // matrices A,B and C
    int *ready;        // needed for synch

    if (argc != 3)
    {
        printf("no shared\n");
    }
    else
    {
        par_id = atoi(argv[1]);
        par_count = atoi(argv[2]);
        // strcpy(shared_mem_matrix,argv[3]);
    }
    if (par_count == 1)
    {
        printf("only one process\n");
    }

    int fd[2];

    if (par_id == 0)
    {

        fd[0] = shm_open("numArray", O_RDWR | O_CREAT, 0777);
        ftruncate(fd[0], 32 * sizeof(int));
        arr = (int *)mmap(NULL, 32 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd[0], 0);

        // fd[1] =  shm_open("matrixB", O_RDWR|O_CREAT, 0777);
        // ftruncate(fd[1], 10*10*sizeof(float));
        // B = (float*) mmap(NULL, 100*sizeof(float), PROT_WRITE|PROT_READ, MAP_SHARED, fd[1], 0);

        // fd[2] =  shm_open("matrixC", O_RDWR|O_CREAT, 0777);
        // ftruncate(fd[2], 10*10*sizeof(float));
        // C = (float*) mmap(NULL, 100*sizeof(float), PROT_WRITE|PROT_READ, MAP_SHARED, fd[2], 0);

        fd[1] = shm_open("synchobject", O_RDWR | O_CREAT, 0777);
        ftruncate(fd[1], 1 * sizeof(int));
        // TODO statically inputted 4
        ready = (int *)mmap(NULL, 4 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd[1], 0);
    }
    else
    {
        sleep(2);
        // TODO: init the shared memory for A,B,C, ready. shm_open withOUT C_CREAT here! NO ftruncate! but yes to mmap
        fd[0] = shm_open("numArray", O_RDWR, 0777);
        arr = (int *)mmap(NULL, 32 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        // fd[1] = shm_open("matrixB", O_RDWR, 0777);
        // B = (float*) mmap(NULL, 100*sizeof(float), PROT_WRITE|PROT_READ, MAP_SHARED, fd[1], 0);
        // fd[2] = shm_open("matrixC", O_RDWR, 0777);
        // C = (float*) mmap(NULL, 100*sizeof(float), PROT_WRITE|PROT_READ, MAP_SHARED, fd[2], 0);
        fd[1] = shm_open("synchobject", O_RDWR, 0777);
        ready = (int *)mmap(NULL, 4 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd[1], 0);
        sleep(2); // needed for initalizing synch
    }

    // printf("before #1: %d\n", par_id);

    // synch(par_id, par_count, ready);
    // sleep(1);
    // printf("after #1: %d\n", par_id);

    synch(par_id, par_count, ready);

    if (par_id == 0)
    {
        srand(time(NULL));
        // TODO: initialize the matrices A and B
        for (int i = 0; i < 32; i++)
        {
            // FIXME range needs to be 0 - 1000
            arr[i] = rand() % 1001;
        }

        printf("\n\nUnsorted Array: ");
        // TODO print out unsorted array
        for (int i = 0; i < 32; i++)
        {
            printf("[%3d] ", arr[i]);
            if(i == 18){
                printf("\n"         );
            }
        }
        printf("\n\n\n");
    }

    for (int i = 0; i < 32; i++)
    {
        synch(par_id, par_count, ready);
        sort_pair(arr, par_id, par_count, 0);

        synch(par_id, par_count, ready);
        sort_pair(arr, par_id, par_count, 1);
    }

    synch(par_id, par_count, ready);

    // sleep(3);

    if (par_id == 0)
    {

        printf("Sorted!: ");
        // TODO print out unsorted array
        for (int i = 0; i < 32; i++)
        {
            printf("[%3d] ", arr[i]);
            if(i == 18){
                printf("\n");
            }
        }
        printf("\n\n\n");
    }

    close(fd[0]);
    close(fd[1]);
    shm_unlink("numArray");
    shm_unlink("synchobject");

    return 0;
}