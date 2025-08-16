#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Keeps objects we need in a callback to
// identify whether all data has been written
// to the socket and to initiate next async
// writing operation if needed.
struct Session
{
  std::shared_ptr<asio::ip::tcp::socket> sock;
  std::string buf;
  std::size_t total_bytes_written;
};

// Function used as a callback for
// asynchronous writing operation.
// Checks if all data from the buffer has
// been written to the socket and initiates
// new asynchronous writing operation if needed.
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

  s->total_bytes_written += bytes_transferred;

  if (s->total_bytes_written == s->buf.length())
  {
    return;
  }

  s->sock->async_write_some // void async_write_some( const ConstBufferSequence & buffers, WriteHandler handler);
      (
          asio::buffer(
              s->buf.c_str() +
                  s->total_bytes_written,
              s->buf.length() -
                  s->total_bytes_written),
          std::bind(
              callback,
              std::placeholders::_1,
              std::placeholders::_2,
              s));
}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock)
{

  std::shared_ptr<Session> s{std::make_shared<Session>()};

  // Step 4. Allocating and filling the buffer.
  s->buf = std::string("Hello");
  s->total_bytes_written = 0;
  s->sock = sock;

  // Step 5. Initiating asynchronous write operation.
  s->sock->async_write_some(
      asio::buffer(s->buf),
      std::bind(
          callback,
          std::placeholders::_1,
          std::placeholders::_2,
          s));
}

struct SessionBetter {
  std::shared_ptr<asio::ip::tcp::socket> sock;
  std::string buf;
};

void callbackBetter(const boost::system::error_code& ec,
  std::size_t bytes_transferred,
  std::shared_ptr<SessionBetter> s)
{
  if (ec) {
    std::cout << "Error occured! Error code = "
      << ec.value()
      << ". Message: " << ec.message();

    return;
  }

  // Here we know that all the data has
  // been written to the socket.
}

void writeToSocketBetter(std::shared_ptr<asio::ip::tcp::socket> sock)
{

  std::shared_ptr<SessionBetter> s{std::make_shared<SessionBetter>()};

  // Step 4. Allocating and filling the buffer.
  s->buf = std::string("Hello");
  s->sock = sock;

  // Step 5. Initiating asynchronous write operation.
  asio::async_write(*s->sock.get(),
      asio::buffer(s->buf),
      std::bind(
          callbackBetter,
          std::placeholders::_1,
          std::placeholders::_2,
          s));
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
    std::shared_ptr<asio::ip::tcp::socket> sock{std::make_shared<asio::ip::tcp::socket>(ioc, ep.protocol())};

    sock->connect(ep);

    writeToSocket(sock);

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

/*
Define a data structure that contains a pointer to a socket object, a buffer, and a variable used as a counter of bytes written.
Define a callback function that will be called when the asynchronous writing operation is completed.
In a client application, allocate and open an active TCP socket and connect it to a remote application. In a server application, obtain a connected active TCP socket by accepting a connection request.
Allocate a buffer and fill it with data that is to be written to the socket.
Initiate an asynchronous writing operation by calling the socket's async_write_some() method. Specify a function defined in step 2 as a callback.
Call the run() method on an object of the asio::io_service class.
In a callback, increase the counter of bytes written. If the number of bytes written is less than the total amount of bytes to be written, initiate a new asynchronous writing operation to write the next portion of the data.
*/