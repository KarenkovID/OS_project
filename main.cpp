#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include <sys/epoll.h>
#include<arpa/inet.h> //inet_addr
#include <array>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <thread>

using namespace ::std;

int set_non_blocking(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}
//


int socket_count = 100;
int epoll_size = socket_count;
int epfd = -1;

void send_requests() {
    struct sockaddr_in server;
    //server.sin_addr.s_addr = inet_addr(" 95.213.157.142");
    server.sin_family = AF_INET;
    //IP
    server.sin_addr.s_addr = inet_addr("64.233.163.94");
    //Port
    server.sin_port = htons(80);

    //Create socket
    vector<int> socket_pool;
    char *message = "GET /\n\n";
    struct epoll_event ev[socket_count];
    for (int i = 0; i < socket_count; i++) {
        int tmp = socket(AF_INET, SOCK_STREAM, 0);
        socket_pool.push_back(tmp);
        cout << tmp << ' ';
        //connecting
        if (connect(socket_pool[i], (struct sockaddr *) &server, sizeof(server)) < 0) {
            puts("connect error");
            return;
        }
//        set_non_blocking(socket_pool[i]);
        ev[i].events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP | EPOLLET;
        ev[i].data.fd = socket_pool[i];

        epoll_ctl(epfd, EPOLL_CTL_ADD, socket_pool[i], &ev[i]);

        if (send(socket_pool[i], message, strlen(message), 0) < 0) {
            puts("Send failed");
            return;
        }
        set_non_blocking(socket_pool[i]);
    }
    cout << endl;
    //Send some data
    //Receive a reply from the server

    for (int i = 0; i < socket_count; i++) {

    }
}

//epoll is here
void resive_response() {
    struct epoll_event events[socket_count];
    char server_reply[8];

    int open_connections = epoll_size;

    while (open_connections > 0) {
        // ожидаем момента, когда надо будет работать...
        int nfds = epoll_wait(epfd, events,
                              open_connections,
                              10000);
        open_connections -= nfds;
        cout << "Nfds " << nfds << endl;
        // для каждого готового сокета
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            ssize_t respLen;
            //в данном месте используется блокуруемая версия чтения в буфер
            while ((respLen = recv(fd, server_reply, 8, 0)) > 0) {
                printf("%.*s", respLen, server_reply);
            }
            cout << i << ' ' << fd << ' ' << strlen(server_reply) << endl;
        }
    };
}
/**
 * This app do several requests to the given server
 * @param argc is size of argv
 * @param argv
 * argv[1] = IP address
 * argv[2] = port
 * optional [argv[3] = ]
 * @return
 */
//
int main(int argc, char *argv[]) {
    /*if (client_sock == -1)
    {
        printf("Could not create socket");
    }*/

    int t = 0;


    //Короче, Стас, тут почти всё готовл
    //Осталось только немного доделать
    //Предлагаю сразу делать n запросов и смотреть врямя ответа
    //Сразу = просте делать их, там ничего не блокируется, кроме connect
    epfd = epoll_create(epoll_size);
    std::thread sendThread(send_requests);
    std::thread epollThread(resive_response);
    sendThread.join();
    epollThread.join();

    return 0;
}