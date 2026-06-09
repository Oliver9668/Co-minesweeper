#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <string>

class Network
{
private:
    SOCKET sock;
    SOCKET client;
    bool isHost;

    bool sendAll(SOCKET s, const char *data, int len);
    int  recvAll(SOCKET s, char *buf, int len);
    std::string recvLine(SOCKET s);

public:
    Network();
    ~Network();

    bool host(int port);
    bool join(const char *ip, int port);
    bool isHosting() const { return isHost; }
    bool isConnected() const { return client != INVALID_SOCKET; }

    bool sendMsg(const char *msg);

    bool hasData();
    std::string recvMsg();

    void disconnect();
};

#endif