#ifndef __SOCKET_CPP__
#define __SOCKET_CPP__
#include <boost/asio.hpp>

namespace SocketNS
{
    // Only reason why there is a namespace here is to use "using" in a limited scope.
    using boost::asio::ip::tcp;
    //indentation is kind of ugly but it doesn't always help readibility to strictly follow the rule.
    class Socket
    {
        // All programs that use boost::asio need to have at least one io service object.
        boost::asio::io_service IOService_;
        tcp::socket socket_;
    public:
        Socket ( std::string server, std::uint16_t port ) : socket_ (IOService_ ) // We create and connect the socket.
        {
            // We need to turn the server name into a TCP endpoint.
            tcp::resolver resolver ( IOService_ );
            // A resolver takes a query object and turns it into a list of endpoints.
            // We construct a query using the name of the server and port number.
            tcp::resolver::query query( server, std::to_string( port ) );
            // The list of endpoints is returned using an iterator of type resolver::iterator.
            tcp::resolver::iterator endpoint_iterator = resolver.resolve( query );
            // The list of endpoints obtained above may contain both IPv4/6,
            // so we need to try each of them until we find one that works; it makes IP version independent.
            // boost::asio::conect() function does this for us automatically.
            boost::asio::connect (socket_, endpoint_iterator );
            // the connection is open and we can now read the response from the port.
        }

        tcp::socket *GetSocket() { return &socket_; }
    }; // class
} // namespace
using namespace SocketNS;
#endif
