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

void callback(const boost::system::error_code &ec, std::shared_ptr<asio::ip::tcp::socket> sock)
{
  // If asynchronous operation has been
  // cancelled or an error occured during
  // execution, ec contains corresponding
  // error code.
  if (ec)
  {
    if (ec == asio::error::operation_aborted)
    {
      std::cout << "Operation cancelled!";
    }
    else
    {
      std::cout << "Error occured!"
                << " Error code = "
                << ec.value()
                << ". Message: "
                << ec.message();
    }

    return;
  }
  // At this point the socket is connected and
  // can be used for communication with
  // remote application.
}

void run(asio::io_context &ioc)
{
  try
  {
    ioc.run();
  }
  catch (system::system_error &e)
  {
    std::cout << "Error occured!"
              << " Error code = " << e.code()
              << ". Message: " << e.what();
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

    std::shared_ptr<asio::ip::tcp::socket> sock{std::make_shared<asio::ip::tcp::socket>(ioc, ep.protocol())};

    const system::error_code ec;
    sock->async_connect(ep, std::bind(callback, std::placeholders::_1, sock));

    // Starting a thread, which will be used
    // to call the callback when asynchronous 
    // operation completes.
    std::thread worker_thread([&ioc](){
      run(ioc);
    });

    // Emulating delay.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Cancelling the initiated operation.
    sock->cancel();

    // Waiting for the worker thread to complete.
    worker_thread.join();
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
If the application is intended to run on Windows XP or Windows Server 2003, define flags that enable asynchronous operation canceling on these versions of Windows.
Allocate and open a TCP or UDP socket. It may be an active or passive (acceptor) socket in the client or server application.
Define a callback function or functor for an asynchronous operation. If needed, in this callback, implement a branch of code that handles the situation when the operation has been canceled.
Initiate one or more asynchronous operations and specify a function or an object defined in step 4 as a callback.
Spawn an additional thread and use it to run the Boost.Asio event loop.
Call the cancel() method on the socket object to cancel all the outstanding asynchronous operations associated with this socket.
*/