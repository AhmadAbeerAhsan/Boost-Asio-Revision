#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service
{
public:
    Service() {}

    void HandleClient(asio::ip::tcp::socket &sock)
    {
        try
        {
            asio::streambuf request;
            asio::read_until(sock, request, '\n');

            std::string req;
            std::istream strm(&request);
            std::getline(strm, req);
            std::cout << req << std::endl;

            // Emulate request processing.
            int i = 0;
            while (i != 1000000)
            {
                i++;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // Sending response.
            std::string response = "Response\n";
            asio::write(sock, asio::buffer(response));
        }
        catch (system::system_error &e)
        {
            std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
        }
    }
};

class Acceptor
{
public:
    Acceptor(asio::io_context &ioc, unsigned short port_num) : m_ioc(ioc),
                                                               m_acceptor(
                                                                   m_ioc,
                                                                   asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num))
    {
        m_acceptor.listen();
    }

    void Accept()
    {
        asio::ip::tcp::socket sock(m_ioc);

        m_acceptor.accept(sock);

        Service svc;
        svc.HandleClient(sock);
    }

private:
    asio::io_context &m_ioc;
    asio::ip::tcp::acceptor m_acceptor;
};

class Server
{
public:
    Server() : m_stop(false) {}

    void Start(unsigned short port_num)
    {
        m_thread.reset(new std::thread([this, port_num]() { Run(port_num); }));
    }

    void Stop()
    {
        m_stop.store(true);
        m_thread->join();
    }

private:
    void Run(unsigned short port_num)
    {
        Acceptor acc(m_ioc, port_num);

        while (!m_stop.load())
        {
            acc.Accept();
        }
    }

    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    asio::io_context m_ioc;
};

int main()
{
  unsigned short port_num = 3333;

  try {
    Server srv;
    srv.Start(port_num);

    std::this_thread::sleep_for(std::chrono::seconds(60));

    srv.Stop();
  }
  catch (system::system_error&e) {
        std::cout  << "Error occured! Error code = " 
                   <<e.code() << ". Message: "
                   <<e.what();
  }

  return 0;
}

/*
Allocate an acceptor socket and bind it to a particular TCP port.
Run a loop until the server is stopped:
    Wait for the connection request from a client.
    Accept the client's connection request when one arrives.
    Wait for the request message from the client.
    Read the request message.
    Process the request.
    Send the response message to the client.
    Close the connection with the client and deallocate the socket.
*/

/*
In the Server class's Stop() method, after setting the value of the m_stop atomic variable to true, 
we can create a dummy active socket, connect it to this same server, and send some dummy request. 
This will guarantee that the server thread will leave the accept() method of the acceptor socket and 
will eventually check the value of the m_stop atomic variable to find out that its value is equal to true, 
which will lead to termination of the loop and completion of the Acceptor::Accept() method.
*/