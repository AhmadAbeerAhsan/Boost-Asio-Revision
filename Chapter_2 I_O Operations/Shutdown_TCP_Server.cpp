#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void processRequest(asio::ip::tcp::socket &sock)
{
    // We use extensible buffer because we don't
    // know the size of the request message.
    asio::streambuf request_buf;

    system::error_code ec;

    // Receiving the request.
    asio::read(sock, request_buf, ec);

    if (ec != asio::error::eof)
        throw system::system_error(ec);

    std::istream is(&request_buf);
    std::string data;
    std::getline(is, data);
    std::cout << "Buffer contents: " << data << std::endl;

    // Request received. Sending response.
    // Allocating and filling the buffer with
    // binary data.
    const char response_buf[] = {'w', 'e', 'l', 't'};

    // Sending the request data.
    asio::write(sock, asio::buffer(response_buf));

    // Shutting down the socket to let the
    // client know that we've sent the whole
    // response.
    sock.shutdown(asio::socket_base::shutdown_send);
}

int main()
{
    // The size of the queue containing the pending connection
    // requests.
    const int BACKLOG_SIZE = 30;

    // Step 1. Here we assume that the server application has
    // already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Creating a server endpoint.
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(),
                               port_num);

    asio::io_context ioc;

    try
    {
        asio::ip::tcp::acceptor acceptor(ioc, ep);

        // acceptor.listen();

        asio::ip::tcp::socket sock(ioc);

        acceptor.accept(sock);

        processRequest(sock);
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
The server application is intended to allocate an acceptor socket and passively wait for a connection request. When the connection request arrives, it should accept it and read the data from the socket connected to the client until the client application shuts down the socket on its side. Having received the request message, the server application should send the response message notifying its boundary by shutting down the socket.
*/