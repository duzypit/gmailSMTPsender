#ifndef __MYOPENSSL_CPP__
#define __MYOPENSSL_CPP__

//first sudo apt-get install libssl-dev
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <memory>

class MyOpenSSL
{
    std::unique_ptr<SSL_CTX, decltype (SSL_CTX_free)*> ctx_;
    std::unique_ptr<SSL, decltype (SSL_free)*> ssl_;
    enum { errorBufSize = 256, readBufSize = 256};	// may need to increase these sizes.

public:
    // method can be one of SSLv2 method, SSLv3 method, TLSvl method or SSLv23 method.
    MyOpenSSL(int socket, const SSL_METHOD *method = SSLv23_method() ) : ctx_ (nullptr, SSL_CTX_free), ssl_ (nullptr, SSL_free)
    {
        char errorBuf[errorBufSize];
        // Then an SSL CTX object is created as a framwork to establish TLS/SSL enabled connections.
        ctx_ = decltype(ctx_) (SSL_CTX_new(method), SSL_CTX_free);
        if (nullptr == ctx_) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));
        //Create SSL struct that hold data for the connection.ssl = declitype
        ssl_ = decltype(ssl_)(SSL_new(ctx_.get()), SSL_free);
        if (nullptr == ssl_) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));
        // When a network connection has been created, it can be assigned to an SSL object.
        // After the SSL object has been created using SSL new,
        // SSL set fd or SSL set bio can be used to associate the network connection with the object.

        const int rstSetFd = SSL_set_fd( ssl_.get(), socket);
        if (rstSetFd == 0 ) throw std::runtime_error (ERR_error_string(ERR_get_error(), errorBuf));


        // Then TLS/SSL handshake is performed using SSL accept or SSL connect respectively.
        const int rstConnect = SSL_connect(ssl_.get() );
        if ( 0 == rstConnect )
        {
            throw std::runtime_error ( "handshake failed.");
        }
        else if ( 0 > rstConnect )
        {
            throw std::runtime_error ( "handshake and shutdown failed.");
        }
    }

    void Write(const std::string& msg)
    {
        // SSL read and SSL write are used to read and write data on the TLS/SSL connection.
        const int rstWrite = SSL_write(ssl_.get(), msg.c_str(), msg.length() );
        if ( 0 == rstWrite )
        {
            throw std::runtime_error("socket write failed due to lose connection." );
        }
        else if ( 0 > rstWrite )
        {
            throw std::runtime_error ( "socket write failed due to unknown reason." );
        }
    }


    template <typename IsDoneReceivingFunctorType>
    std::string Read (IsDoneReceivingFunctorType isDoneReceiving)
    {
        char buf[readBufSize];
        std::string read;
        while (true)
        {
	        const int rstRead = SSL_read (ssl_.get(), buf, readBufSize);
	        if ( 0 == rstRead) throw std::runtime_error("Connection lost while read.");
	        if ( 0 > rstRead && SSL_ERROR_WANT_READ == SSL_get_error(ssl_.get(), rstRead)) continue;
	        read += std::string(buf, buf + rstRead);
        	if (isDoneReceiving (read)) return read;
        }
    }

    ~MyOpenSSL ()
	{
        // SSL shutdown can be used to shut down the TLS/SSL connection.
        int rstShutdown = SSL_shutdown(ssl_.get());
        if ( 0 == rstShutdown )
        {
            rstShutdown = SSL_shutdown(ssl_.get());
        }
        else if ( -1 == rstShutdown && SSL_RECEIVED_SHUTDOWN == SSL_get_shutdown(ssl_.get()))
        {
            //uh strange thing below ;(
            //throw std::runtime_error( "shutdown failed." );

	    }
    }

    struct StaticInitialize
    {
        StaticInitialize()
	    {
            // At first the library must be initialized
            ERR_load_crypto_strings();
            SSL_load_error_strings();
	        SSL_library_init();
        }
	    ~StaticInitialize()
        {
            ERR_free_strings ();
        }
    };
};
#endif
