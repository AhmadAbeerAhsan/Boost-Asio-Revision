#include <boost/predef.h> // Tools to identify the OS.

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501

#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace boost;

void communicate(asio::ip::tcp::socket &sock)
{
    // Allocating and filling the buffer with
    // binary data.
    const char request_buf[] = {'h', 'a', 'l', 'l', 'o'};

    // Sending the request data.
    asio::write(sock, asio::buffer(request_buf));

    // Shutting down the socket to let the
    // server know that we've sent the whole
    // request.
    sock.shutdown(asio::socket_base::shutdown_send);

    // We use extensible buffer for response
    // because we don't know the size of the
    // response message.
    asio::streambuf response_buf;

    system::error_code ec;
    asio::read(sock, response_buf, ec);

    if (ec == asio::error::eof)
    {
        // Whole response message has been received.
        // Here we can handle it.
        std::istream is(&response_buf);
        std::string data;
        std::getline(is, data);
        std::cout << "Buffer contents: " << data << std::endl;
    }
    else
    {
        throw system::system_error(ec);
    }
}

int main()
{
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try
    {
        asio::ip::address ip_address = asio::ip::make_address(raw_ip_address);
        asio::ip::tcp::endpoint ep(ip_address, port_num);

        asio::io_context ioc;

        asio::ip::tcp::socket sock{ioc, ep.protocol()};

        sock.connect(ep);

        communicate(sock);
    }
    catch (system::system_error &e)
    {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/*
The purpose of the client application is to allocate the socket and connect it to the server application. After the connection is established, the application should prepare and send a request message notifying its boundary by shutting down the socket after writing the message to it.
After the request is sent, the client application should read the response. The size of the response is unknown; therefore, the reading should be performed until the server closes its socket to notify the response boundary.
*/