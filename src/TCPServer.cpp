#include <thread>
#include <netinet/tcp.h> // ‘TCP_NODELAY’

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "TCPServer.h" 

TCPServer::TCPServer(int port) {
    struct sockaddr_in serverAddress;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);
    int optValue = 1; // DO NOT make this a "bool" type! I have seen it not working then.
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof (optValue));
    bind(sockfd, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
    listen(sockfd, 5);
}

TCPServer::~TCPServer() {
    std::cout << "TCPServer::~TCPServer called: " << std::this_thread::get_id() << std::endl;
    close(sockfd);
    sockfd = -1;
}

void TCPServer::Task(void * This, int newsockfd) {
    std::cout << "TCPServer::Task start: " << std::this_thread::get_id() << std::endl;
    while (1) {
        int n = ((TCPServer *) This)->handleMessage(newsockfd);
        if (n <= 0) {
            close(newsockfd);
            break;
        }
    }
    std::cout << " TCPServer::Task stop: " << std::this_thread::get_id() << std::endl;
}

void TCPServer::run() {

    class MessageServer {
    public:

        static void accept_conn(void * This, int sockfd) {
            std::thread accept_thread([This, sockfd] {
                while (1) {
                    struct sockaddr_in clientAddress;
                    std::string str;
                    socklen_t sosize = sizeof (clientAddress);
                    int newsockfd = accept(sockfd, (struct sockaddr*) &clientAddress, &sosize);
                    if (newsockfd <= 0) {
                        break;
                    }
                    std::thread t(Task, This, newsockfd);
                    t.detach();
                    str = inet_ntoa(clientAddress.sin_addr);
                    std::cout << "TCPServer::run MessageServer::accept_conn: " << str << std::endl;
                }
            });
            accept_thread.detach();
        }
    };
    MessageServer::accept_conn(this, sockfd);
}

int TCPServer::handleMessage(int newsockfd) {
    int n;
    char msg[MAXPACKETSIZE];

    struct timeval tv;
    tv.tv_sec = 8; /* Secs Timeout */
    tv.tv_usec = 0; // Not init'ing this can cause strange errors
    setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (struct timeval));
    int flag = 1;
    if (setsockopt(newsockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof (int)) < 0) {
        std::cout << "TCPServer::handleMessage Could not disable Nagle's algorithm." << std::endl;
    }

    n = recv(newsockfd, msg, MAXPACKETSIZE, 0);
    if (n <= 0) {
        return n;
    }
    msg[n] = 0;
    std::string sendstr = "sending TCPServer";
    send(newsockfd, sendstr.c_str(), sendstr.length(), 0);
    return n;
}
