#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BYTE unsigned char
#define SERVER_PORT_STANDART 47821
#define RAND_HALF 1073741823
#define BUFFER_SIZE 8

typedef struct userstruct user;
typedef struct roomstruct room;

struct userstruct{
    //room *myroom;
    int myroom;
    int streame;
    BYTE noinfo;
    //BYTE timerem;
	bool playing;
	bool finding;
	bool first;
};

struct roomstruct{
	user* p1;
	user* p2;
	bool drs;
	bool state; // 0-none 1-free 2-full
};

room *rooms;
user *users;
int wroom;
//room *vroom;
bool freeroom;
BYTE *buffe;
const struct timespec tpc1 = {0, 100000000};
const struct timespec tpc2 = {0, 100000000};

int main(void) {
    rooms = (room*)malloc(sizeof(room) * 64);
    users = (user*)malloc(sizeof(user) * 128);
    buffe = (char*)malloc(sizeof(char) * 8);
    for(int u = 0; u < 64; ++u){
        (rooms + u)->state = false;
        (rooms + u)->drs = false;
    }
    for(int u = 0; u < 128; ++u){
        (users + u)->streame = -1;
    }
    wroom = -1;
    freeroom = false;
    //rooms->drs = false;
	int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    socket_desc = socket(AF_INET, SOCK_STREAM, NULL);
    if (socket_desc == -1) return 1;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT_STANDART);
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) return 1;
    listen(socket_desc, 4);
    c = sizeof(struct sockaddr_in);
    pthread_t threa;
    fcntl(socket_desc, F_SETFL, fcntl(socket_desc, F_GETFL, NULL) | O_NONBLOCK);
	while(true) {
        //printf("frame. last sock: %d\n", client_sock);
        if((client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)) > 0){
            fcntl(client_sock, F_SETFL, fcntl(client_sock, F_GETFL) | O_NONBLOCK);
            //printf("%d\n", client_sock);
            for(int p = 0; p < 128; ++p){
                if((users + p)->streame < 0){
                    (users + p)->streame = client_sock;
                    (users + p)->finding = false;
                    (users + p)->playing = false;
                    (users + p)->noinfo = 0;
                    break;
                }
            }
        }
        for(int p = 0; p < 128; ++p){
            int sock = (users + p)->streame;
            if(sock > 0){
                user *player = users + p;
                //printf("%d\n", player->streame);
                int code = recv(sock, buffe, BUFFER_SIZE, NULL);
                if(code > 0){
                    //printf("%d-%d-%d-%d-%d-%d-%d-%d\n", buffe[0], buffe[1], buffe[2], buffe[3], buffe[4], buffe[5], buffe[6], buffe[7]);
                    player->noinfo = 0;
                    switch(buffe[0]){
                        case 1:{
                            if(player->playing) break;
                            if(!freeroom){
                                player->finding = true;
                                freeroom = true;
                                player->first = true;
                                int r = 0;
                                while((rooms + r)->state) ++r;
                                wroom = r;
                                player->myroom = r;
                                (rooms + r)->p1 = player;
                            }
                            else if(player->finding){
                                player->finding = false;
                                freeroom = false;
                                player->first = false;
                                wroom = -1;
                            }
                            else{
                                freeroom = false;
                                player->first = false;
                                player->myroom = wroom;
                                room *proom = rooms + wroom;
                                proom->state = true;
                                proom->p2 = player;
                                proom->p1->playing = true;
                                proom->p1->finding = false;
                                proom->p2->playing = true;
                                proom->p2->finding = false;
                                //bool w = rand() > RAND_HALF;
                                bool w = true;
                                buffe[0] = 1;
                                buffe[1] = w;
                                send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                                buffe[1] = !w;
                                send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                                //vroom = malloc(sizeof(room));
                                proom->drs = false;
                                wroom = -1;
                            }
                            break;
                        }
                        case 2:{
                            //BYTE ii[8] = {2, 9, 12, 0, 0, 0, 0, 0};
                            room *proom = rooms + player->myroom;
                            send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                            /*if(proom->drs){
                                proom->drs = false;
                            }*/
                            break;
                        }
                        case 3:{
                            if(!player->playing) break;
                            room *proom = rooms + player->myroom;
                            proom->p1->playing = false;
                            proom->p2->playing = false;
                            buffe[0] = 3;
                            buffe[1] = !player->first;
                            send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            buffe[1] = player->first;
                            send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                            proom->state = false;
                            break;
                        }
                        case 4:{
                            if(!player->playing) break;
                            room *proom = rooms + player->myroom;
                            buffe[0] = 4;
                            buffe[1] = 0;
                            if(!proom->drs){
                                proom->drs = true;
                                if(player->first) send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                                else send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            }
                            else{
                                buffe[1] = 1;
                                proom->p1->playing = false;
                                proom->p2->playing = false;
                                send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                                send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                                proom->state = false;
                            }
                            break;
                        }
                        case 5:{
                            break;
                        }
                    }
                }else if(code == 0){
                    player->streame = -1;
                    room *proom = rooms + player->myroom;
                    if(player->playing){
                        if(player->finding) freeroom = false;
                        else if(player->playing){
                            buffe[0] = 3;
                            buffe[1] = 1;
                            if(player->first){
                                proom->p2->playing = false;
                                send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                            }
                            else{
                                proom->p1->playing = false;
                                send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            }
                            proom->state = false;
                        }
                    }
                }else{
                    if(++player->noinfo == 200){
                        BYTE empty[8];
                        empty[0] = 5;
                        send(player->streame, empty, BUFFER_SIZE, NULL);
                    }else if(player->noinfo == 250){
                        close(player->streame);
                        player->streame = -1;
                        room *proom = rooms + player->myroom;
                        if(player->playing){
                            proom->p1->playing = false;
                            proom->p2->playing = false;
                            buffe[0] = 3;
                            buffe[1] = !player->first;
                            send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            buffe[1] = player->first;
                            send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                            proom->state = false;
                        }
                    }
                    /*printf("%d\n", player->noinfo);
                    if(player->playing && (--player->timerem > 120) && false){
                        room *proom = rooms + player->myroom;
                        proom->p1->playing = false;
                        proom->p2->playing = false;
                        BYTE mess3[BUFFER_SIZE];
                        mess3[0] = 3;
                        mess3[1] = !player->first;
                        send(proom->p1->streame, mess3, BUFFER_SIZE, NULL);
                        mess3[1] = player->first;
                        send(proom->p2->streame, mess3, BUFFER_SIZE, NULL);
                        proom->state = 0;
                    }*/
                }
            }
        }
        nanosleep(&tpc1, &tpc2);
    }
}
