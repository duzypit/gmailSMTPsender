#ifndef __MYSMTP_CPP__
#define  __MYSMTP_CPP__
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "myopenssl.cpp"
#include <iostream>
#include <string>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include "socket.cpp"

class MySMTP
{
public:
    static void sendSSL (const std::string& server,
                        uint16_t port,
                        const std::string& userName,
                        const std::string& password,
                        const std::string& from,
                        const std::string& to,
                        const std::string& subject,
                        const std::string& message)
    {
        static const std::string newline = "\r\n";
        Socket socket(server, port);
        MyOpenSSL openSSL (socket.GetSocket()->native ());
        // http:en.wikipedia.org/wiki/Simple Mail Transfer ProtocolitSMTP transport example
        std::cout << openSSL.Read (ReceiveFunctor (220));// 220 mx. google.com ESMTP
        openSSL.Write(std::string( "EHLO " ) + server + newline );
        std::cout << openSSL.Read (ReceiveFunctor (250));	// 250-mx.google.com at your service,
        openSSL.Write (std::string ("AUTH LOGIN" ) + newline );
        std::cout << openSSL.Read ( ReceiveFunctor (334) ); // 334
        openSSL.Write (EncodeBase64(userName) + newline );
        std::cout << openSSL.Read (ReceiveFunctor (334) ); // 334
        openSSL.Write ( EncodeBase64 ( password ) + newline );
        std::cout << openSSL.Read ( ReceiveFunctor ( 235 ) ); // 235 2.7.0 Accepted
        openSSL.Write(std::string ("MAIL FROM: <" ) + from + " >" + newline);
        std::cout << openSSL.Read (ReceiveFunctor (250));	// 250 2.1.0 0K
        openSSL.Write (std::string( "RCPT To:<") + to + ">" + newline);
        std::cout << openSSL.Read (ReceiveFunctor (250)); // 250 2.1.5 0K
        openSSL.Write (std::string( "DATA" ) + newline );
        std::cout << openSSL.Read ( ReceiveFunctor (354) ); // 354 Go ahead
        openSSL.Write( std::string ("From: <" ) + from + ">" + newline );
        openSSL.Write ( std::string( "To: <" ) + to + ">" + newline );
        openSSL.Write(std::string( "SUBJECT: " ) + subject + newline );
        openSSL.Write ( newline + message + newline + "." + newline );
        std::cout << openSSL.Read ( ReceiveFunctor (250) ) << std::endl; // 250 2.0.0 OK
        openSSL.Write ( std::string ( "QUIT" ) + newline );
        std::cout << openSSL.Read (ReceiveFunctor (221));	// 22l 2.0.0 closing connection
    }

private:
    struct ReceiveFunctor
    {
        enum { codeLength = 3 };
        const std::string code;

        ReceiveFunctor(int expectingCode) : code (std::to_string (expectingCode))
        {
            if (code.length() != codeLength) throw std::runtime_error ("SMTP code must be three-digits.");
		}

        // return true if msg contains expected code and there is a newline character following.
        // return false if msg didn' t contatin the newline character yet; we need to receive more.
        // throw if msg doesn 't contain the expected code.

        bool operator () ( const std::string &msg) const
        {
            if ( msg.length () < codeLength ) return false;
            //std::cout << "Return Code: " << msg << std::endl;

            if ( code != msg.substr ( 0, codeLength ) ) throw std::runtime_error ( "SMTP code is not received." );

            const size_t posNewline = msg.find_first_of( "\n", codeLength );

            if (posNewline == std::string::npos ) return false;
            if ( msg.at(codeLength) == ' ') return true;
            if ( msg.at(codeLength) == '-' ) return this->operator() ( msg.substr( posNewline + 1) );
            //std::cout << "Return Code: " << msg << std::endl;
            throw std::runtime_error (" Unexpected return code recieved." );
		}
    };

	static std::string EncodeBase64 (const std::string & data)
    {
        //boost involved
		typedef boost::archive::iterators::base64_from_binary<	// convert binary values to base64 characters
            boost::archive::iterators::transform_width<const char *, 6, 8> // get 6bit from 8bit
            > Base64;
        return std::string ( Base64( data.c_str()), Base64(data.c_str() + data.length()));
    }
};
#endif
