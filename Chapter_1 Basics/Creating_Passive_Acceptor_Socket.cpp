#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
  // Step 1. An instance of 'io_context' class is required by
  // socket constructor. 
  asio::io_context ioc;

  // Step 2. Creating an object of 'tcp' class representing
  // a TCP protocol with IPv4 as underlying protocol.
  asio::ip::tcp protocol = asio::ip::tcp::v4();

  // Step 3. Instantiating an acceptor socket(passive socket) object.
  asio::ip::tcp::acceptor acceptor(ioc);

  // Used to store information about error that happens
  // while opening the socket.
  system::error_code ec;

  // Step 4. Opening the acceptor socket.
  acceptor.open(protocol, ec);

  if (ec.value() != 0) {
    // Failed to open the acceptor socket.
    std::cout
      << "Failed to open the socket! Error code = "
      << ec.value() << ". Message: " << ec.message();
      return ec.value();
  }

  return 0;
}

/*
try {
  // Step 3 + 4 in single call. May throw.
  asio::ip::tcp::socket sock(ios, protocol);
} catch (boost::system::system_error & e) {
  std::cout << "Error occured! Error code = " << e.code()
    << ". Message: "<< e.what();
}
*/