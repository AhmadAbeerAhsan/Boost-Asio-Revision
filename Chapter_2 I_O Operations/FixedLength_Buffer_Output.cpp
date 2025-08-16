#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
    // Step 1. Allocate a buffer. Note that this step does not involve any functionality or data types from Boost.Asio.
    // Step 2. Fill the buffer with the data that is to be used as the output.
    std::string buf{"Hello"};

    // Step 3. Creating buffer representation that satisfies
    // ConstBufferSequence concept requirements.
    asio::const_buffer output_buf = asio::buffer(buf);

    // Step 4. 'output_buf' is the representation of the
    // buffer 'buf' that can be used in Boost.Asio output
    // operations.

    //In order to use our std::string object with the send() method of the class that represents a TCP socket, we can do something like this:
    asio::const_buffer asio_buf(buf.c_str(), buf.length());
    std::vector<asio::const_buffer> buffers_sequence;
    buffers_sequence.push_back(asio_buf);

    return 0;
}