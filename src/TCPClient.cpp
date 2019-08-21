#include "TCPClient.h"
#include <netinet/tcp.h> // ‘TCP_NODELAY’

TCPClient::TCPClient()
{
	sock = -1;
	port = 0;
	address = "";
}

bool TCPClient::setup()
{
         return setup(address , port);
}

bool TCPClient::setup(string address , int port)
{
        this->address = address;
        this->port = port; 
	if(sock == -1)
	{
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
      			cout << "Could not create socket" << endl;
    		}
        }
  	if(inet_addr(address.c_str()) == INADDR_NONE)
  	{
    		struct hostent *he;
    		struct in_addr **addr_list;
    		if ( (he = gethostbyname( address.c_str() ) ) == NULL)
    		{
		      herror("gethostbyname");
      		      cout<<"Failed to resolve hostname\n";
		      return false;
    		}
	   	addr_list = (struct in_addr **) he->h_addr_list;
    		for(int i = 0; addr_list[i] != NULL; i++)
    		{
      		      server.sin_addr = *addr_list[i];
		      break;
    		}
  	}
  	else
  	{
    		server.sin_addr.s_addr = inet_addr( address.c_str() );
  	}


    struct timeval tv;
    tv.tv_sec = 8; /* Secs Timeout */
    tv.tv_usec = 0; // Not init'ing this can cause strange errors
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (struct timeval));
    int flag = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof (int)) < 0) {
        std::cout << "TCPServer::handleMessage Could not disable Nagle's algorithm." << std::endl;
    }

  	server.sin_family = AF_INET;
  	server.sin_port = htons( port );
  	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  	{
    		perror("connect failed. Error");
    		return false;
  	}
  	return true;
}

bool TCPClient::Send(string data)
{
	if(sock != -1) 
	{
		if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
		{
			cout << "Send failed : " << data << endl;
                        exit();
			return false;
		}
	}
	else
		return false;
	return true;
}

string TCPClient::receive(int size)
{
  	char buffer[size];
	memset(&buffer[0], 0, sizeof(buffer));

  	string reply;
	if( recv(sock , buffer , size, 0) <= 0)
  	{
	    	cout << "receive failed!" << endl;
		exit();
		return "";
  	}
	buffer[size-1]='\0';
  	reply = buffer;
  	return reply;
}

string TCPClient::read()
{
  	char buffer[1] = {};
  	string reply;
  	while (buffer[0] != '\n') {
    		if( recv(sock , buffer , sizeof(buffer) , 0) <= 0)
    		{
      			cout << "receive failed!" << endl;
                        exit();
			return "";
    		}
		reply += buffer[0];
	}
	return reply;
}

void TCPClient::exit()
{
    close( sock );
    sock = -1;
		std::quick_exit( 3 );
}
