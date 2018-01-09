#ifndef __SOCKET_CPP__
#define __SOCKET_CPP__
#include "practicalSocket.h"
#include "practicalSocket.cpp"


class SMTPSocket
{
    public:

    SMTPSocket(const std::string& address, const int& port)
    {
        socket_.connect(address, port);
    }

    Socket *GetSocket() { return &socket_; }

    private:
    TCPSocket socket_;
}; // class

#endif
