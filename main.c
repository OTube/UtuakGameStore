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
    room *myroom;
    int streame;
    BYTE noinfo;
    BYTE timerem;
	bool playing;
	bool finding;
	bool first;
};

struct roomstruct{
	user* p1;
	user* p2;
	bool drs;
	BYTE state; // 0-none 1-free 2-full
};

room *rooms;
user *users;
int wroom;
//room *vroom;
bool freeroom;
BYTE *buffe;

int main(void) {
    rooms = (room*)malloc(sizeof(room) * 64);
    users = (user*)malloc(sizeof(user) * 128);
    buffe = (char*)malloc(sizeof(char) * 8);
    for(int u = 0; u < 64; ++u){
        (rooms + u)->state = 0;
        (rooms + u)->drs = false;
    }
    for(int u = 0; u < 128; ++u){
        (users + u)->streame = -1;
        (users + u)->finding = false;
        (users + u)->playing = false;
        (users + u)->noinfo = 0;
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
        if((client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)) > 0){
            /*int flags = fcntl(client_sock, F_GETFL);
            if(flags < 0) return 0;
            fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);*/
            printf("%d\n", client_sock);
            for(int p = 0; p < 128; ++p){
                if((users + p)->streame < 0){
                    (users + p)->streame = client_sock;
                    /*
                    for(int r = 0; r < 128; ++r){
                        switch(rooms[r].state){
                            case 0:
                                break;
                            case 1:
                                rooms[r].p1 = users + p;
                                break;
                            case 2:
                                rooms[r].p2 = users + p;
                                break;
                        }
                    }*/
                    break;
                }
            }
        }
        for(int p = 0; p < 128; ++p){
            int sock = (users + p)->streame;
            if(sock > 0){
                user *player = users + p;
                int code = recv(sock, buffe, BUFFER_SIZE, NULL);
                if(code > 0){
                    /*for(int k = 0; k < 8; ++k) printf("%d-", buffe[k]);
                    printf("\n");*/
                    player->noinfo = 0;
                    room *proom = player->myroom;
                    switch(buffe[0]){
                        case 1:{
                            if(player->playing) break;
                            if(!freeroom){
                                player->finding = true;
                                freeroom = true;
                                player->first = true;
                                int r = 0;
                                while((rooms + r)->state != 0) ++r;
                                wroom = r;
                                proom = rooms + r;
                                proom->p1 = player;
                            }
                            else if(player->finding){
                                player->finding = false;
                                freeroom = false;
                                player->first = false;
                            }
                            else{
                                freeroom = false;
                                player->first = false;
                                proom = rooms + wroom;
                                proom->state = 2;
                                proom->p2 = player;
                                proom->p1->playing = true;
                                proom->p1->finding = false;
                                proom->p1->timerem = 120;
                                proom->p2->playing = true;
                                proom->p2->finding = false;
                                proom->p2->timerem = 120;
                                bool w = rand() > RAND_HALF;
                                BYTE mess1[BUFFER_SIZE];
                                mess1[0] = 1;
                                mess1[1] = w;
                                send(proom->p1->streame, mess1, BUFFER_SIZE, NULL);
                                mess1[1] = !w;
                                send(proom->p2->streame, mess1, BUFFER_SIZE, NULL);
                                //vroom = malloc(sizeof(room));
                                proom->drs = false;
                                wroom = -1;
                            }
                            break;
                        }
                        case 2:{
                            send(proom->p1->streame, buffe, BUFFER_SIZE, NULL);
                            send(proom->p2->streame, buffe, BUFFER_SIZE, NULL);
                            if(proom->drs){
                                proom->drs = false;
                            }
                            break;
                        }
                        case 3:{
                            if(!player->playing) break;
                            proom->p1->playing = false;
                            proom->p2->playing = false;
                            BYTE mess3[BUFFER_SIZE];
                            mess3[0] = 3;
                            mess3[1] = !player->first;
                            send(proom->p1->streame, mess3, BUFFER_SIZE, NULL);
                            mess3[1] = player->first;
                            send(proom->p2->streame, mess3, BUFFER_SIZE, NULL);
                            proom->state = 0;
                            break;
                        }
                        case 4:{
                            if(!player->playing) break;
                            BYTE mess4[BUFFER_SIZE];
                            mess4[0] = 4;
                            mess4[1] = 0;
                            if(!proom->drs){
                                proom->drs = true;
                                if(player->first) send(proom->p2->streame, mess4, BUFFER_SIZE, NULL);
                                else send(proom->p1->streame, mess4, BUFFER_SIZE, NULL);
                            }
                            else{
                                mess4[1] = 1;
                                proom->p1->playing = false;
                                proom->p2->playing = false;
                                send(proom->p1->streame, mess4, BUFFER_SIZE, NULL);
                                send(proom->p2->streame, mess4, BUFFER_SIZE, NULL);
                                proom->state = 0;
                            }
                            break;
                        }
                        case 5:{
                            break;
                        }
                    }
                }else if(code == 0){
                    /*
                    if(player->finding) freeroom = false;
                    else if(player->playing){
                        BYTE messq[2];
                        messq[0] = 3;
                        messq[1] = 1;
                        if(first){
                            myroom->p2->playing = false;
                            send(myroom->p2->streame, messq, 2, NULL);
                        }
                        else{
                            myroom->p1->playing = false;
                            send(myroom->p1->streame, messq, 2, NULL);
                        }
                        free(myroom);
                    }
                    */
                    player->streame = 0;
                    room *proom = player->myroom;
                    if(player->playing){
                        if(player->finding) freeroom = false;
                        else if(player->playing){
                            BYTE messq[BUFFER_SIZE];
                            messq[0] = 3;
                            messq[1] = 1;
                            if(player->first){
                                proom->p2->playing = false;
                                send(proom->p2->streame, messq, BUFFER_SIZE, NULL);
                            }
                            else{
                                proom->p1->playing = false;
                                send(proom->p1->streame, messq, BUFFER_SIZE, NULL);
                            }
                            proom->state = 0;
                        }
                    }
                }else{
                    if(++player->noinfo == 20){
                        BYTE empty[8];
                        empty[0] = 5;
                        send(player->streame, empty, BUFFER_SIZE, NULL);
                    }else if(player->noinfo == 30){
                        close(player->streame);
                        player->streame == -1;
                        room *proom = player->myroom;
                        if(player->playing){
                            proom->p1->playing = false;
                            proom->p2->playing = false;
                            BYTE mess3[BUFFER_SIZE];
                            mess3[0] = 3;
                            mess3[1] = !player->first;
                            send(proom->p1->streame, mess3, BUFFER_SIZE, NULL);
                            mess3[1] = player->first;
                            send(proom->p2->streame, mess3, BUFFER_SIZE, NULL);
                            proom->state = 0;
                        }
                    }
                    if(player->playing && (--player->timerem > 120)){
                        room *proom = player->myroom;
                        proom->p1->playing = false;
                        proom->p2->playing = false;
                        BYTE mess3[BUFFER_SIZE];
                        mess3[0] = 3;
                        mess3[1] = !player->first;
                        send(proom->p1->streame, mess3, BUFFER_SIZE, NULL);
                        mess3[1] = player->first;
                        send(proom->p2->streame, mess3, BUFFER_SIZE, NULL);
                        proom->state = 0;
                    }
                    printf(":%d\n", code);
                    puts("none");
                }
            }
        }
        sleep(1);
    }
}
