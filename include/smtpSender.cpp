#ifndef __MYSMTP_CPP__
#define  __MYSMTP_CPP__
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
    SMTPSender() : newline("\r\n"),  _server("smtp.gmail.com"), _port(465){
        fillReplyCodes();
    }

    void verifyCreditenials(const std::string& userName, const std::string& password)
    {
        SMTPSocket socket(_server, _port);
        OpenSSLBearer openSSL(socket.GetSocket()->nativeHandle());
/*
        std::cout << openSSL.read(ReceiveFunctor(220));// 220 mx. google.com ESMTP

        openSSL.write(std::string("EHLO ") + _server + newline);
        std::cout << openSSL.read(ReceiveFunctor(250));	// 250-mx.google.com at your service,

        openSSL.write (std::string("AUTH LOGIN") + newline);
        std::cout << openSSL.read(ReceiveFunctor(334)); // 334

        openSSL.write(EncodeBase64(userName) + newline);
        std::cout << openSSL.read(ReceiveFunctor(334)); // 334

        openSSL.write(EncodeBase64(password) + newline);
        std::cout << openSSL.read(ReceiveFunctor(235)); // 235 2.7.0 Accepted // 535 not accepted

        openSSL.write(std::string("QUIT") + newline);
        std::cout << openSSL.read(ReceiveFunctor (221));	// 22l 2.0.0 closing connection
*/

    }

    void sendSSL (const std::string& userName, const std::string& password, const std::string& from, const std::string& to, const std::string& subject, const std::string& message)
    {
        SMTPSocket socket(_server, _port);
        OpenSSLBearer openSSL(socket.GetSocket()->nativeHandle());
        std::string msg;

        msg = openSSL.read();
        if (validReturnMsg(220, msg)) {
            std::cout << msg << std::endl;
        }

        //std::cout << openSSL.read(ReceiveFunctor(220));// 220 mx. google.com ESMTP

        openSSL.write(std::string("EHLO ") + _server + newline);
        //std::cout << openSSL.read(ReceiveFunctor(250));	// 250-mx.google.com at your service,
        msg = openSSL.read();
        if (validReturnMsg(250, msg)) {
            std::cout << msg << std::endl;
        }
        //std::cout << "missing msg: " << std::endl;
        //std::cout << msg <<std::endl;

        openSSL.write (std::string("AUTH LOGIN") + newline);
        //std::cout << openSSL.read(ReceiveFunctor(334)); // 334
        msg = openSSL.read();
        if (validReturnMsg(334, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write(EncodeBase64(userName) + newline);
        //std::cout << openSSL.read(ReceiveFunctor(334)); // 334
        msg = openSSL.read();
        if (validReturnMsg(334, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write(EncodeBase64(password) + newline);
        //std::cout << openSSL.read(ReceiveFunctor(235)); // 235 2.7.0 Accepted // 535 not accepted
        msg = openSSL.read();
        if (validReturnMsg(235, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write(std::string("MAIL FROM: <") + from + " >" + newline);
        //std::cout << openSSL.read(ReceiveFunctor (250));	// 250 2.1.0 0K
        msg = openSSL.read();
        if (validReturnMsg(250, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write (std::string("RCPT To:<") + to + ">" + newline);
        //std::cout << openSSL.read (ReceiveFunctor (250)); // 250 2.1.5 0K
        msg = openSSL.read();
        if (validReturnMsg(250, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write(std::string("DATA") + newline);
        //std::cout << openSSL.read(ReceiveFunctor(354)); // 354 Go ahead
        msg = openSSL.read();
        if (validReturnMsg(354, msg)) {
            std::cout << msg << std::endl;
        }

        openSSL.write(std::string("From: <") + from + ">" + newline);
        openSSL.write(std::string("To: <") + to + ">" + newline);
        openSSL.write(std::string("SUBJECT: ") + subject + newline);
        openSSL.write(newline + message + newline + "." + newline);
        msg = openSSL.read();
        if (validReturnMsg(250, msg)) {
            std::cout << msg << std::endl;
        }

        //std::cout << openSSL.read(ReceiveFunctor(250)) << std::endl; // 250 2.0.0 OK
        openSSL.write(std::string("QUIT") + newline);
        //std::cout << openSSL.read(ReceiveFunctor (221));	// 22l 2.0.0 closing connectioni
        msg = openSSL.read();
        if (validReturnMsg(221, msg)) {
            std::cout << msg << std::endl;
        }

    }

private:
    std::string newline;
    std::string _server;    // server address
    int _port;              // server port
    std::map<int, std::string> _smtpReplyCodes;

    void fillReplyCodes()
    {
//        std::map<int, std::string> m i

        _smtpReplyCodes = {
            {200,	"(nonstandard success response, see rfc876)"},
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
            {530,	"Access denied (???a Sendmailism)"},
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

    /*struct ReceiveFunctor
    {
        enum { codeLength = 3 };
        const std::string code;

        ReceiveFunctor(int expectedCode) : code(std::to_string(expectedCode))
        {
            if (code.length() != codeLength) throw std::runtime_error ("smtpSender: SMTP code must be three-digits.");
		}

        // return true if msg contains expected code and there is a newline character following.
        // return false if msg didn' t contatin the newline character yet; we need to receive more.
        // throw if msg doesn 't contain the expected code.

        bool operator()(const std::string &msg) const
        {
            if (msg.length() < codeLength) return false;

            if (code != msg.substr(0, codeLength)) throw std::runtime_error ("smtpSender: SMTP code was not received.");
            *if(!_smtpReplyCodes.find(msg.substr(0, codeLength)))
            {
                throw std::runtime_error ("smtpSender: SMTP code was not received.");
            }*/
/*            const size_t posNewline = msg.find_first_of("\n", codeLength);

            if (posNewline == std::string::npos) return false;
            if (msg.at(codeLength) == ' ') return true;
            if (msg.at(codeLength) == '-') return this->operator()(msg.substr(posNewline + 1));
            throw std::runtime_error ("smtpSender: Unexpected return code recieved.");
		}
    };
*/
	static std::string EncodeBase64 (const std::string& data)
    {

        std::string result;
        Base64::Encode(data, &result);
        return result;
    }
};
#endif
