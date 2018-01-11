#ifndef __MYOPENSSL_CPP__
#define __MYOPENSSL_CPP__

//first sudo apt-get install libssl-dev
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <memory>
#include <iostream>


class OpenSSLBearer
{
public:
    // method can be one of SSLv2 method, SSLv3 method, TLSvl method or SSLv23 method.
    OpenSSLBearer(int socket, const SSL_METHOD *method = SSLv23_method() ) : ctx_ (nullptr, SSL_CTX_free), ssl_ (nullptr, SSL_free)
    {
        initialize();
       // std::cout << "OpenSSLBearer initialized" << std::endl;
        char errorBuf[errorBufSize];
        // Then an SSL CTX object is created as a framwork to establish TLS/SSL enabled connections.
        ctx_ = decltype(ctx_) (SSL_CTX_new(method), SSL_CTX_free);
        if (ctx_ == nullptr) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));
        //Create SSL struct that hold data for the connection.ssl = declitype
        ssl_ = decltype(ssl_)(SSL_new(ctx_.get()), SSL_free);
        if (ssl_ == nullptr) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));
        // When a network connection has been created, it can be assigned to an SSL object.
        // After the SSL object has been created using SSL new,
        // SSL set fd or SSL set bio can be used to associate the network connection with the object.

        const int rstSetFd = SSL_set_fd( ssl_.get(), socket);
        if (rstSetFd == 0 ) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));


        // Then TLS/SSL handshake is performed using SSL accept or SSL connect respectively.
        const int rstConnect = SSL_connect(ssl_.get() );
        if (rstConnect == 0)
        {
            throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));
//("handshake failed.");
        }
        else if (rstConnect < 0)
        {
            throw std::runtime_error ("OpenSSLBearer: handshake and shutdown failed.");
        }
    }

    void write(const std::string& msg)
    {
        // SSL read and SSL write are used to read and write data on the TLS/SSL connection.
        const int rstWrite = SSL_write(ssl_.get(), msg.c_str(), msg.length() );
        if (rstWrite == 0)
        {
            throw std::runtime_error("OpenSSLBearer: socket write failed due to lose connection.");
        }
        else if ( rstWrite < 0)
        {
            throw std::runtime_error ("OpenSSLBearer: socket write failed due to unknown reason.");
        }
    }


    //template <typename IsDoneReceivingFunctorType>
    std::string read (/*IsDoneReceivingFunctorType isDoneReceiving*/)
    {
        char buf[readBufSize];
        std::string read;
        bool waitForRead;
        while (true)
        {
            waitForRead = false;
	        const int rstRead = SSL_read (ssl_.get(), buf, readBufSize);
	        if (rstRead == 0)
            {
                throw std::runtime_error("OpenSSLBearer: Connection lost while read.");
                //std::cout << "openSSLBearer: connection lost whie read." <<std::endl;
            }

            //wait for the socket to be readable, then call this fucn again
	        if (rstRead < 0 && SSL_ERROR_WANT_READ == SSL_get_error(ssl_.get(), rstRead))
            {
                waitForRead = true;
                continue;
            }

	        read += std::string(buf, buf + rstRead);
        	if (!waitForRead) return read;
        }
    }

    ~OpenSSLBearer()
	{
        // SSL shutdown can be used to shut down the TLS/SSL connection.
        SSL_shutdown(ssl_.get());
        ERR_free_strings();

    }

private:
    void initialize()
    {
        ERR_load_crypto_strings();
        SSL_load_error_strings();
        SSL_library_init();
    }

    std::unique_ptr<SSL_CTX, decltype (SSL_CTX_free)*> ctx_;
    std::unique_ptr<SSL, decltype (SSL_free)*> ssl_;
    enum
    {
        errorBufSize = 256,
        readBufSize = 256
    };	// may need to increase these sizes.
};
#endif
