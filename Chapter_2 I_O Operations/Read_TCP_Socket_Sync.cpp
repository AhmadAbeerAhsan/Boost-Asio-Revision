#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

std::string readFromSocket(asio::ip::tcp::socket& sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];
  std::size_t total_bytes_read = 0;

  while (total_bytes_read != MESSAGE_SIZE) {
    total_bytes_read += sock.read_some(
      asio::buffer(buf + total_bytes_read,
      MESSAGE_SIZE - total_bytes_read));
  }

  return std::string(buf, total_bytes_read);
}

std::string readFromSocketEnhanced(asio::ip::tcp::socket& sock) {
  const unsigned char MESSAGE_SIZE = 7;
  char buf[MESSAGE_SIZE];

  asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));

  return std::string(buf, MESSAGE_SIZE);
}

//asio::read_until() function provides a way to read data from a socket until a specified pattern is encountered in the data. There are eight overloads of this function
std::string readFromSocketDelim(asio::ip::tcp::socket& sock) {
  asio::streambuf buf;

  // Synchronously read data from the socket until
  // '\n' symbol is encountered.  
  asio::read_until(sock, buf, '\n');

  std::string message;

  // Because buffer 'buf' may contain some other data
  // after '\n' symbol, we have to parse the buffer and
  // extract only symbols before the delimiter. 
  
  std::istream input_stream(&buf);
  std::getline(input_stream, message);
  return message;
}

//The asio::read_at() function provides a way to read data from a socket, starting at a particular offset.

int main()
{
  std::string raw_ip_address = "127.0.0.1";
  unsigned short port_num = 3333;

  try {
    asio::ip::address ip_address = asio::ip::make_address(raw_ip_address);
    asio::ip::tcp::endpoint ep(ip_address, port_num);

    asio::io_context ioc;

    asio::ip::tcp::socket sock(ioc, ep.protocol());

    sock.connect(ep);

    readFromSocket(sock);
  }
  catch (system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
      << ". Message: " << e.what();
    
    return e.code().value();
  }

  return 0;
}