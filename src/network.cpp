#include "network.h"
#include <iostream>

Network::Network() : sock(INVALID_SOCKET), client(INVALID_SOCKET), isHost(false)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Network::~Network()
{
    disconnect();
    WSACleanup();
}

bool Network::sendAll(SOCKET s, const char *data, int len)
{
    int sent = 0;
    while (sent < len)
    {
        int n = send(s, data + sent, len - sent, 0);
        if (n <= 0)
            return false;
        sent += n;
    }
    return true;
}

int Network::recvAll(SOCKET s, char *buf, int len)
{
    int received = 0;
    while (received < len)
    {
        int n = recv(s, buf + received, len - received, 0);
        if (n <= 0)
            return -1;
        received += n;
    }
    return received;
}

std::string Network::recvLine(SOCKET s)
{
    std::string line;
    char ch;
    while (true)
    {
        int n = recv(s, &ch, 1, 0);
        if (n <= 0)
            return "";
        if (ch == '\n')
            break;
        if (ch != '\r')
            line += ch;
    }
    return line;
}

bool Network::host(int port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return false;

    // Allow port reuse
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((u_short)port);

    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) != 0)
        return false;
    if (listen(sock, 1) != 0)
        return false;

    // Set to non-blocking for accept
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    isHost = true;
    return true;
}

bool Network::join(const char *ip, int port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons((u_short)port);

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) != 0)
        return false;

    client = sock;
    isHost = false;
    return true;
}

bool Network::hasData()
{
    if (client == INVALID_SOCKET)
    {
        // Host mode: check for pending client connection
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET newClient = accept(sock, (sockaddr *)&clientAddr, &addrLen);
        if (newClient != INVALID_SOCKET)
        {
            client = newClient;
            // Set client socket to non-blocking
            u_long mode = 1;
            ioctlsocket(client, FIONBIO, &mode);
            return false;
        }
        return false;
    }

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(client, &readSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return select(0, &readSet, NULL, NULL, &timeout) > 0;
}

std::string Network::recvMsg()
{
    if (client == INVALID_SOCKET)
        return "";
    return recvLine(client);
}

bool Network::sendMsg(const char *msg)
{
    if (client == INVALID_SOCKET)
        return false;
    std::string s(msg);
    s += '\n';
    return sendAll(client, s.c_str(), (int)s.size());
}

void Network::disconnect()
{
    if (client != INVALID_SOCKET && client != sock)
    {
        closesocket(client);
        client = INVALID_SOCKET;
    }
    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}