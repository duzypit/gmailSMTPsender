#include <iostream>
#include <string>
#include <stdint.h>
#include <stdexcept>
//#include <boost/asio.hpp>
//#include <boost/archive/iterators/base64_from_binary.hpp>
//#include <boost/archive/iterators/transform_width.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "include/socket.cpp"
#include "include/myopenssl.cpp"
#include "include/mysmtp.cpp"
#include "include/passwd.cpp"


int main ()
{
    //get username
    std::cout << "Gmail username: ";
    std::string uname;
    std::cin >> uname;
    //get pass
    std::cout << "password: ";
    Passwd p;
    std::string password;
    password = p.getPassword();
    std::cout << std::endl;
    //get from
    std::string from(uname);
    from += "@gmail.com";

    //get to
    std::cout << "Recipient: ";
    std::string recipient;
    std::cin >> recipient;

    // This needs to be called only once.
    // Don't let multi-threads run it several times.
    MyOpenSSL::StaticInitialize ssInitializer;

    // For gmail application specific password, check out help pages:
    // https://support.google.com/accounts/answer/185833
    // https://security.google.com/settings/security/apppasswords
    MySMTP::sendSSL (
        "smtp.gmail.com", // server address
        465, // port
        uname, // userName
        password, // password
        from, // from
        recipient, // to
        "Testing Boost STMP", // subject
        "This message body" // message
    );
    return 0;
}

