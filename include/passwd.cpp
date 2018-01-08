#ifndef __PASSWD_CPP__
#define __PASSWD_CPP__
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <string>

class Passwd
{
public:
    Passwd(){}
    ~Passwd(){}

    void setStdinEcho(bool enable = true)
    {
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        if( !enable )
            tty.c_lflag &= ~ECHO;
        else
            tty.c_lflag |= ECHO;

        (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    std::string getPassword()
    {
        std::string pass;
        setStdinEcho(false);
        std::cin >> pass;
        setStdinEcho();
        return pass;
    }

private:

};


#endif
