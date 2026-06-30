#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // Create a duplicate process

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } 
    else if (pid == 0) { 
        // --- CHILD PROCESS ---
        // We use execlp to replace the current process with the script.
        // "sh" is the executable, "-c" tells sh to read the following string.
        char *args[] = {"/bin/sh", "-c", "/home/delta/src/flaresolverr/run", NULL};
        
        if (execvp(args[0], args) == -1) {
            perror("Exec failed");
            exit(1);
        }
    } 
    else { 
        // --- PARENT PROCESS ---
        int status;
        waitpid(pid, &status, 0); // Wait for the script to finish
        printf("Child process finished\n");
    }

    return 0;
}
