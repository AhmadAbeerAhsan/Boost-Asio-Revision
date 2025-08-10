#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
  // Step 1. Assume that the client application has already
  // obtained the IP address and protocol port number of the
  // target server.
  std::string host = "samplehost.com";
  std::string port_num = "3333";

// Used by a 'resolver' and a 'socket'.
  asio::io_context ioc;
  
// Creating a resolver.
  asio::ip::tcp::resolver resolver(ioc);

  try {

    // Step 2. Resolving a DNS name.
    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(host, port_num, asio::ip::tcp::resolver::numeric_service);

    // Step 3. Creating and opening a socket.
    asio::ip::tcp::socket sock(ioc);

    // Step 4. asio::connect() method iterates over
    // each endpoint until successfully connects to one
    // of them. It will throw an exception if it fails
    // to connect to all the endpoints or if other
    // error occurs.
    asio::connect(sock, endpoints);
    // connect() method will bind the socket to the endpoint consisting of an IP address and a protocol port number chosen by the operating system.
    // Note that in step 3, we don't open the socket when we create it. This is because we don't know the version of IP addresses to which the provided DNS name will resolve. The asio::connect() function opens the socket before connecting it to each endpoint specifying proper protocol object and closes it if the connection fails.

    // At this point socket 'sock' is connected to 
    // the server application and can be used
    // to send data to or receive data from it.
  }
  // Overloads of asio::ip::tcp::resolver::resolve and 
  // asio::connect() used here throw
  // exceptions in case of error condition.
  catch (system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
      << ". Message: " << e.what();
    
    return e.code().value();
  }

  return 0;
}