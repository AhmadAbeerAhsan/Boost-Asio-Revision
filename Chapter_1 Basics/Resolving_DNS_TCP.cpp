#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
  // Step 1. Assume that the client application has already
  // obtained the DNS name and protocol port number and 
  // represented them as strings.
  std::string host = "samplehost.com";
  std::string port_num = "3333";

  // Step 2.
  asio::io_context ioc;

  // Step 4. Creating a resolver.
  asio::ip::tcp::resolver resolver(ioc);

  // Used to store information about error that happens
  // during the resolution process.
  system::error_code ec;

  // Step 5. + // Step 3. Creating a query.
  asio::ip::tcp::resolver::results_type endpoints =
    resolver.resolve(host, port_num, asio::ip::tcp::resolver::numeric_service, ec);

  // Handling errors if any.
  if (ec) {
    // Failed to resolve the DNS name. Breaking execution.
    std::cout << "Failed to resolve a DNS name."
      << "Error code = " << ec.value() 
      << ". Message = " << ec.message();

    return ec.value();
  }

  // Iterate directly over the results
    for (const auto& entry : endpoints) {
        asio::ip::tcp::endpoint ep = entry.endpoint();
        std::cout << "Resolved: " << ep << "\n";
    }
  
  return 0;
}