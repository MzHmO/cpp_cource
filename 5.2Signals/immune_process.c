#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    signal(SIGTERM, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    
    FILE *fp = fopen("/home/box/pid", "w");
    if (fp) {
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }
    
    while(1) {
        sleep(1);
    }
    
    return 0;
}
