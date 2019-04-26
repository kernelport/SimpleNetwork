#include <iostream>
#include <sys/socket.h>
#include "TCPServer.h"

using namespace std;

class RpcTCPServer:  public TCPServer {
public:
    RpcTCPServer(int port) : TCPServer(port) {}
    int handleMessage(int newsockfd);
};

int RpcTCPServer::handleMessage(int newsockfd) {
    int n;
    char msg[MAXPACKETSIZE];
    n = recv(newsockfd, msg, MAXPACKETSIZE, 0);
    cout << " recv: " << msg << endl;
    if (n <= 0) {
        cout << " handleMessage: " << n << endl;
        return n;
    }
    msg[n] = 0;
    //Message = string(msg);
    string sendstr = "sending RpcTCPServer " + string(msg) ;
    send(newsockfd, sendstr.c_str(), sendstr.length(), 0);
    return n;
}

int main()
{
	RpcTCPServer tcp(11999);
        tcp.run();
        while (1);
	return 0;
}
