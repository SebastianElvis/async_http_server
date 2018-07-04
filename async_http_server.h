#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

class http_connection
  : public boost::enable_shared_from_this<http_connection>
{
public:
    typedef boost::shared_ptr<http_connection> pointer; // using shared_ptr in order to destroy the http_connection object automatically
    static pointer create(boost::asio::io_service& io_service);
    boost::asio::ip::tcp::socket& socket();
    void start();

private:
    http_connection(boost::asio::io_service& io_service);
    void handle_write(const boost::system::error_code& /*error*/,
        size_t /*bytes_transferred*/);
    boost::asio::ip::tcp::socket socket_;
    std::string message_;
};

class http_server
{
public:
    http_server();
    http_server(boost::asio::io_service& io_service);

private:
    void start_accept();
    void handle_accept(http_connection::pointer conn
        , const boost::system::error_code& error);
    boost::asio::ip::tcp::acceptor acceptor_;

    int num_reqs;
};