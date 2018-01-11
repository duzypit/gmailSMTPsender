#ifndef __SMTPSENDER_CPP__
#define __SMTPSENDER_CPP__
#include "openSSLBearer.cpp"
#include <iostream>
#include <string>
#include "smtpSocket.cpp"
#include "base64.h"
#include <map>
#include <string>

class SMTPSender
{
public:
    SMTPSender() : newline("\r\n"),  _server("smtp.gmail.com"), _port(465), socket(_server, _port), openSSL(socket.GetSocket()->nativeHandle())
    {
        fillReplyCodes();
    }

    bool verifyCreditenials(const std::string& userName, const std::string& password)
    {
        std::string msg;
        bool success = false;
        //220 mx. google.com ESMTP
        msg = openSSL.read();
        if (!validReturnMsg(220, msg))
        {
            SMTPReply(220, msg);
        }

        openSSL.write(std::string("EHLO ") + _server + newline);

        //250-mx.google.com at your service,
        msg = openSSL.read();
        if (!validReturnMsg(250, msg))
        {
            SMTPReply(250, msg);
        }

        openSSL.write (std::string("AUTH LOGIN") + newline);

        //334
        msg = openSSL.read();
        if (!validReturnMsg(334, msg))
        {
            SMTPReply(334, msg);
        }

        openSSL.write(EncodeBase64(userName) + newline);
        //334
        msg = openSSL.read();
        if (!validReturnMsg(334, msg))
        {
            SMTPReply(334, msg);
        }

        openSSL.write(EncodeBase64(password) + newline);

        //235 2.7.0 Accepted // 535 not accepted
        msg = openSSL.read();
        if (!validReturnMsg(235, msg))
        {
            SMTPReply(235, msg);
        } else
        {
            success = true;
        }

        openSSL.write(std::string("QUIT") + newline);

        //22l 2.0.0 closing connectioni
        msg = openSSL.read();
        if (validReturnMsg(221, msg))
        {
            SMTPReply(221, msg);
        }
        return success;
    }

    void sendSSL (const std::string& userName, const std::string& password, const std::string& from, const std::string& to, const std::string& subject, const std::string& message)
    {
        std::string msg;

        //220 mx. google.com ESMTP
        msg = openSSL.read();
        if (!validReturnMsg(220, msg))
        {
            SMTPReply(220, msg);
        }

        openSSL.write(std::string("EHLO ") + _server + newline);

        //250-mx.google.com at your service,
        msg = openSSL.read();
        if (!validReturnMsg(250, msg))
        {
            SMTPReply(250, msg);
        }

        openSSL.write (std::string("AUTH LOGIN") + newline);

        //334
        msg = openSSL.read();
        if (!validReturnMsg(334, msg))
        {
            SMTPReply(334, msg);
        }

        openSSL.write(EncodeBase64(userName) + newline);
        //334
        msg = openSSL.read();
        if (!validReturnMsg(334, msg))
        {
            SMTPReply(334, msg);
        }

        openSSL.write(EncodeBase64(password) + newline);

        //235 2.7.0 Accepted // 535 not accepted
        msg = openSSL.read();
        if (!validReturnMsg(235, msg))
        {
            SMTPReply(235, msg);
        }

        openSSL.write(std::string("MAIL FROM: <") + from + " >" + newline);

        //250 2.1.0 0K
        msg = openSSL.read();
        if (!validReturnMsg(250, msg))
        {
            SMTPReply(250, msg);
        }

        openSSL.write (std::string("RCPT To:<") + to + ">" + newline);

        //250 2.1.5 0K
        msg = openSSL.read();
        if (!validReturnMsg(250, msg))
        {
            SMTPReply(250, msg);
        }

        openSSL.write(std::string("DATA") + newline);

        //354 Go ahead
        msg = openSSL.read();
        if (!validReturnMsg(354, msg))
        {
            SMTPReply(354, msg);
        }

        openSSL.write(std::string("From: <") + from + ">" + newline);
        openSSL.write(std::string("To: <") + to + ">" + newline);
        openSSL.write(std::string("SUBJECT: ") + subject + newline);
        openSSL.write(newline + message + newline + "." + newline);

        //250 2.0.0 OK
        msg = openSSL.read();
        if (!validReturnMsg(250, msg))
        {
            SMTPReply(250, msg);
        }


        openSSL.write(std::string("QUIT") + newline);

        //22l 2.0.0 closing connectioni
        msg = openSSL.read();
        if (validReturnMsg(221, msg))
        {
            SMTPReply(221, msg);
        }

    }

private:

    std::string newline;
    std::string _server;    // server address
    int _port;              // server port

    SMTPSocket socket;
    OpenSSLBearer openSSL;

    std::map<int, std::string> _smtpReplyCodes;

    void SMTPReply(const int& code, const std::string msg, bool visible = true)
    {
        if (visible)
        {
            int recCode = std::stoi(msg.substr(0,3));
            auto recCodeMsg = _smtpReplyCodes.find(recCode);
            if(recCodeMsg == _smtpReplyCodes.end()){
                std::cout << "smtpsender: unknown code: " << recCode << std::endl;
            } else {
                std::cout << "smtpSender: code should be: " << code << ", is: " << recCode << ", msg:  " << recCodeMsg->second << std::endl;
            }

        }
    }

    void fillReplyCodes()
    {
        _smtpReplyCodes = {
            {200,	"Nonstandard success response"},
            {211,	"System status, or system help reply"},
            {214,	"Help message"},
            {220,	"<domain> Service ready"},
            {221,	"<domain> Service closing transmission channel"},
            {250,	"Requested mail action okay, completed"},
            {251,	"User not local; will forward to <forward-path>"},
            {252,	"Cannot VRFY user, but will accept message and attempt delivery"},
            {354,	"Start mail input; end with <CRLF>.<CRLF>"},
            {421,	"<domain> Service not available, closing transmission channel"},
            {450,	"Requested mail action not taken: mailbox unavailable"},
            {451,	"Requested action aborted: local error in processing"},
            {452,	"Requested action not taken: insufficient system storage"},
            {500,	"Syntax error, command unrecognised"},
            {501,	"Syntax error in parameters or arguments"},
            {502,	"Command not implemented"},
            {503,	"Bad sequence of commands"},
            {504,	"Command parameter not implemented"},
            {521,	"<domain> does not accept mail (see rfc1846)"},
            {530,	"Access denied"},
            {535,   "Username and password not accepted"},
            {550,	"Requested action not taken: mailbox unavailable"},
            {551,	"User not local; please try <forward-path>"},
            {552,	"Requested mail action aborted: exceeded storage allocation"},
            {553,	"Requested action not taken: mailbox name not allowed"},
            {554,	"Transaction failed"}
        };
    }



    bool validReturnMsg(const int& expectedCode, std::string msg)
    {
        std::size_t codeLength = 3;
        std::string code;
        code = std::to_string(expectedCode);
        bool repeat = true;
        if (code.length() != codeLength) throw std::runtime_error ("smtpSender: SMTP code must be three-digits.");

        while(repeat)
        {
            repeat = false;
            if (msg.length() < codeLength) return false;

            if (code != msg.substr(0, codeLength)) return false;

            std::size_t posNewline = msg.find_first_of("\n", codeLength);

            if (posNewline == std::string::npos) return false;
            if (msg.at(codeLength) == ' ') return true;
            if (msg.at(codeLength) == '-')
            {
                msg = msg.substr(posNewline + 1);
                repeat = true;
            }
        }

        return false;
    }

	static std::string EncodeBase64 (const std::string& data)
    {

        std::string result;
        Base64::Encode(data, &result);
        return result;
    }
};
#endif
