#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;


int main(int argc, char** argv){

    char* args[4];
    for(int j = 0; j < 4; j++){
      
        args[j] = (char*) malloc(100);
                
    }

    
    char program2[100];
    
    sprintf(program2,"./%s",  argv[1]);

    int numPrograms = atoi(argv[2]);

    char* Y = argv[2];
    int X = 0;
    char strX[3];

    // int pids[numPrograms];

    // needs to be mmap-ed because the children are altering array
    int* terminate_children = (int*) mmap(NULL, sizeof(int) * numPrograms, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1,0);
    
    // printf("%d\n", numPrograms);

    for(int i = 0; i < numPrograms; i++){
        if(fork() == 0){

            // pids[X] = getpid();
            
            strcpy(args[0], argv[1]);
            sprintf(strX, "%d", X);
            strcpy(args[1], strX);
            strcpy(args[2], Y);
            args[3] = NULL;
            // cout << "program name is " << program2 << "args are " << args[0] << " " << args[1] << " " << args[2] << endl;

            terminate_children[i] = getpid();

            execv(program2, args);

            printf("failed at %d", i );


            return 0;
        }
  
        X++;
    }

  int status;
  int stillWorking = 1;
  while(stillWorking){
      stillWorking = 0;
      for(int i = 0; i < numPrograms; i++){
         int res = waitpid(terminate_children[i], &status, WNOHANG);
         if(res == 0){
             stillWorking = 1;
         }
      }
    sleep(2);
  }
 
 //clear free
 for(int j = 0; j < 4; j++){
    free(args[j]);         
}

munmap(terminate_children, sizeof(int) * numPrograms);

return 0;

}