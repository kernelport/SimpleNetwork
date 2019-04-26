#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define MAXPACKETSIZE 4096

class TCPServer {
public:
    TCPServer(int port);
    virtual ~TCPServer();
    void run();
    virtual int handleMessage(int newsockfd);

private:
    int sockfd;
    static void Task(void * This, int newsockfd);
};

#endif
