#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Keeps objects we need in a callback to
// identify whether all data has been read
// from the socket and to initiate next async
// reading operation if needed.
struct Session
{
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char[]> buf;
    std::size_t total_bytes_read;
    unsigned int buf_size;
};

// Function used as a callback for
// asynchronous reading operation.
// Checks if all data has been read
// from the socket and initiates
// new reading operation if needed.
void callback(const boost::system::error_code &ec,
              std::size_t bytes_transferred,
              std::shared_ptr<Session> s)
{
    if (ec)
    {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();
        return;
    }

    s->total_bytes_read += bytes_transferred;
    if (s->total_bytes_read == s->buf_size)
    {
        return;
    }

    s->sock->async_read_some(
        asio::buffer(
            s->buf.get() + s->total_bytes_read,
            s->buf_size - s->total_bytes_read),
        std::bind(
            callback, std::placeholders::_1,
            std::placeholders::_2,
            s));
}
void readFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    std::shared_ptr<Session> s{std::make_shared<Session>()};

    // Step 4. Allocating the buffer.
    const unsigned int MESSAGE_SIZE = 7;

    s->buf.reset(new char[MESSAGE_SIZE]);
    s->total_bytes_read = 0;
    s->sock = sock;
    s->buf_size = MESSAGE_SIZE;

    // Step 5. Initiating asynchronous reading operation.
    s->sock->async_read_some(
        asio::buffer(s->buf.get(), s->buf_size),
        std::bind(
            callback,
            std::placeholders::_1,
            std::placeholders::_2,
            s));
}

struct SessionBetter
{
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char[]> buf;
    unsigned int buf_size;
};

void callbackBetter(const boost::system::error_code &ec,
                    std::size_t bytes_transferred,
                    std::shared_ptr<SessionBetter> s)
{
    if (ec)
    {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();

        return;
    }

    // Here we know that the reading has completed
    // successfully and the buffer is full with
    // data read from the socket.
}

void readFromSocketBetter(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    std::shared_ptr<SessionBetter> s{std::make_shared<SessionBetter>()};

    // Step 4. Allocating the buffer.
    const unsigned int MESSAGE_SIZE = 7;

    s->buf.reset(new char[MESSAGE_SIZE]);
    s->sock = sock;
    s->buf_size = MESSAGE_SIZE;

    // Step 5. Initiating asynchronous reading operation.
    asio::async_read(
        *s->sock.get(),
        asio::buffer(s->buf.get(), s->buf_size),
        std::bind(
            callbackBetter,
            std::placeholders::_1,
            std::placeholders::_2,
            s
        )
    );
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

        // Step 3. Allocating, opening and connecting a socket.
        std::shared_ptr<asio::ip::tcp::socket> sock(
            new asio::ip::tcp::socket(ioc, ep.protocol()));

        sock->connect(ep);

        readFromSocket(sock);

        // Step 6.
        ioc.run();
    }
    catch (system::system_error &e)
    {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}