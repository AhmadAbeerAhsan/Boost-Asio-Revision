#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
  // Step 1. Assume that the client application has already
  // obtained the IP address and protocol port number of the
  // target server.
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    // Step 2. Creating an endpoint designating 
    // a target server application.
    asio::ip::address ip_address = asio::ip::make_address(raw_ip_address);
    asio::ip::tcp::endpoint ep(ip_address, port_num);

    asio::io_context ioc;

    // Step 3. Creating and opening a socket.
    asio::ip::tcp::socket sock(ioc, ep.protocol());

    // Step 4. Connecting a socket.
    sock.connect(ep);
    // connect() method will bind the socket to the endpoint consisting of an IP address and a protocol port number chosen by the operating system.

    // At this point socket 'sock' is connected to 
    // the server application and can be used
    // to send data to or receive data from it.
  }
  // Overloads of asio::ip::make_address() and 
  // asio::ip::tcp::socket::connect() used here throw
  // exceptions in case of error condition.
  catch (system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
      << ". Message: " << e.what();
    
    return e.code().value();
  }

  return 0;
}