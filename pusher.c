#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
    srand(time(NULL));
    char *data = malloc(1000000);
    for(unsigned int i = 0; i < 1000000; ++i){
        data[i] = i%2==0 ? (char)(rand() / (float)RAND_MAX * 95 + 32) : '\n';
    }
    data[999999] = '\0';
    FILE *fl = fopen("/home/oleg/Main/Github/file.txt", "w");
    fwrite(data, 1000000, 1, fl);
    fclose(fl);
    free(data);
    system("/home/oleg/Main/Github/pusher.sh");
    return 0;
}
