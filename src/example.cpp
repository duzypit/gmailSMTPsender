#include <iostream>
#include <string>

#include "../include/passwd.cpp"
#include "../include/smtpSender.cpp"


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

    SMTPSender sender;
    if (sender.verifyCreditenials(uname, password)){
        sender.sendSSL (
            uname, // userName
            password, // password
            from, // from
            recipient, // to
            "Testing Boost STMP", // subject
            "This message body" // message
        );
    } else
    {
        std::cout << "Login or password is not valid! Quit." << std::endl;
    }
    return 0;
}

