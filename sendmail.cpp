#include <iostream>
#include <string>
#include <stdint.h>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "socket.cpp"
#include "myopenssl.cpp"
#include "mysmtp.cpp"
int main ()
{
    // This needs to be called only once.
    // Don't let multi-threads run it several times.
    MyOpenSSL::StaticInitialize ssInitializer;

    // For gmail application specific password, check out help pages:
    // https://support.google.com/accounts/answer/185833
    // https://security.google.com/settings/security/apppasswords
    MySMTP::sendSSL (
        "smtp.gmail.com", // server address
        465, // port
        "your_user_name", // userName
        "pass", // password
        "your_user_name@gmail.com", // from
        "recipient@aa.com", // to
        "Testing Boost STMP", // subject
        "This message body" // message
    );
    return 0;
}

