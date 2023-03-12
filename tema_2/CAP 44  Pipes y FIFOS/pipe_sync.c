#include <stdio.h>



int main(int argc, char *argv[]) {
    int pfd[2]; /* Sincronización de Procesos con pipe */
    int j, dummy;

    if (argc < 2 || strcmp(argv[1], "--help") == 0){
        printf("%s sleep-time...\n",argv[0]);
        exit(1);
    }
    printf("%s  Parent started\n", currTime("%T"));
    if (pipe(pfd) == -1){
        perror("pipe");
        exit(1);
    }
    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(1);
        case 0: /* Hijo */
            if (close(pfd[0]) == -1)        /* Read end is unused */
                errExit("close");
            /* Child does some work, and lets parent know it's done */
            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                                            /* Simulate processing */
            printf("%s  Child %d (PID=%ld) closing pipe\n",
                    currTime("%T"), j, (long) getpid());
e             if (close(pfd[1]) == -1)
                errExit("close");
            /* Child now carries on to do other things... */
            _exit(EXIT_SUCCESS);
        default: /* Parent loops to create next child */
            break;
        }
    }
    /* Parent comes here; close write end of pipe so we can see EOF */
r     if (close(pfd[1]) == -1)                /* Write end is unused */
        errExit("close");
    /* Parent may do other work, then synchronizes with children */
t     if (read(pfd[0], &dummy, 1) != 0)
        fatal("parent didn't get EOF");
    printf("%s  Parent ready to go\n", currTime("%T"));
    /* Parent can now carry on to do other things... */
    exit(EXIT_SUCCESS);
}



}