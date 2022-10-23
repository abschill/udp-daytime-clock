#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class udp_server {
public:
  udp_server(boost::asio::io_context& io_context)
    : socket_(io_context, udp::endpoint(udp::v4(), 13)) {
    start_receive(); // call start_receive when we get a call
  }

private:
  void start_receive() {
    /*
     * The function ip::udp::socket::async_receive_from() will cause the application to listen in the background for a new request. 
     When such a request is received, the io_context object will invoke the handle_receive() function with two arguments: a value 
     of type boost::system::error_code indicating whether the operation succeeded or failed, and a size_t value bytes_transferred 
     specifying the number of bytes received.
    */
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this, // bind handler to pass the call
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/) {
        /*
         * The error parameter contains the result of the asynchronous operation. 
         Since we only provide the 1-byte recv_buffer_ to contain the client's request, 
         the io_context object would return an error if the client sent anything larger. 
         We can ignore such an error if it comes up.
        */
    if (!error) {
      boost::shared_ptr<std::string> message(
          new std::string(make_daytime_string()));

      /*
       * service the client request 
      */
      socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
          boost::bind(&udp_server::handle_send, this, message,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      // Start listening for the next client request.
      start_receive();
    }
  }
  /*
   * invoked after the service request has been completed.
   * you can see we pass the reference operator of this function into the socket method above
   * so thats where it knows to cleanup with this
   * just like the tcp one, we didn't actually have it do anything other than demonstrate the concept 
  */
  void handle_send(boost::shared_ptr<std::string> /*message*/,
      const boost::system::error_code& /*error*/,
      std::size_t /*bytes_transferred*/) {
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1> recv_buffer_;
};

int main() {
  try {
    boost::asio::io_context io_context;
    udp_server server(io_context);
    io_context.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}