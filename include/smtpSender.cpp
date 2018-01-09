#ifndef __MYSMTP_CPP__
#define  __MYSMTP_CPP__
#include "openSSLBearer.cpp"
#include <iostream>
#include <string>
#include "smtpSocket.cpp"
#include "base64.h"

class SMTPSender
{
public:
    SMTPSender() : newline("\r\n"),  _server("smtp.gmail.com"), _port(465){}

    void verifyCreditenials(const std::string& userName, const std::string& password)
    {
        SMTPSocket socket(_server, _port);
        OpenSSLBearer openSSL(socket.GetSocket()->nativeHandle());

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


    }

    void sendSSL (const std::string& userName, const std::string& password, const std::string& from, const std::string& to, const std::string& subject, const std::string& message)
    {
        SMTPSocket socket(_server, _port);
        OpenSSLBearer openSSL(socket.GetSocket()->nativeHandle());

        std::cout << openSSL.read(ReceiveFunctor(220));// 220 mx. google.com ESMTP

        openSSL.write(std::string("EHLO ") + _server + newline);
        std::cout << openSSL.read(ReceiveFunctor(250));	// 250-mx.google.com at your service,

        openSSL.write (std::string("AUTH LOGIN") + newline);
        std::cout << openSSL.read(ReceiveFunctor(334)); // 334

        openSSL.write(EncodeBase64(userName) + newline);
        std::cout << openSSL.read(ReceiveFunctor(334)); // 334

        openSSL.write(EncodeBase64(password) + newline);
        std::cout << openSSL.read(ReceiveFunctor(235)); // 235 2.7.0 Accepted // 535 not accepted

        openSSL.write(std::string("MAIL FROM: <") + from + " >" + newline);
        std::cout << openSSL.read(ReceiveFunctor (250));	// 250 2.1.0 0K

        openSSL.write (std::string("RCPT To:<") + to + ">" + newline);
        std::cout << openSSL.read (ReceiveFunctor (250)); // 250 2.1.5 0K

        openSSL.write(std::string("DATA") + newline);
        std::cout << openSSL.read(ReceiveFunctor(354)); // 354 Go ahead

        openSSL.write(std::string("From: <") + from + ">" + newline);
        openSSL.write(std::string("To: <") + to + ">" + newline);
        openSSL.write(std::string("SUBJECT: ") + subject + newline);
        openSSL.write(newline + message + newline + "." + newline);

        std::cout << openSSL.read(ReceiveFunctor(250)) << std::endl; // 250 2.0.0 OK
        openSSL.write(std::string("QUIT") + newline);
        std::cout << openSSL.read(ReceiveFunctor (221));	// 22l 2.0.0 closing connection
    }

private:
    std::string newline;
    std::string _server;    // server address
    int _port;

    struct ReceiveFunctor
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
            if (msg.length () < codeLength) return false;

            if (code != msg.substr(0, codeLength)) throw std::runtime_error ("smtpSender: SMTP code was not received.");

            const size_t posNewline = msg.find_first_of("\n", codeLength);

            if (posNewline == std::string::npos) return false;
            if (msg.at(codeLength) == ' ') return true;
            if (msg.at(codeLength) == '-') return this->operator()(msg.substr(posNewline + 1));
            throw std::runtime_error ("smtpSender: Unexpected return code recieved.");
		}
    };

	static std::string EncodeBase64 (const std::string& data)
    {

        std::string result;
        Base64::Encode(data, &result);
        return result;
    }
};
#endif
