#include "async_http_server.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio::ip;

using boost::asio::io_service;

http_connection::http_connection(io_service& ios)
: socket_(ios) {}

void http_connection::handle_write(const boost::system::error_code& /*error*/,
        size_t /*bytes_transferred*/){}

http_connection::pointer http_connection::create(io_service& ios)
{
    return pointer(new http_connection(ios));
}

tcp::socket& http_connection::socket()
{
    return socket_;
}

void http_connection::start()
{
    message_ = "HTTP/1.0 200 OK\r\n"
            "Date: Wed, 04 Jul 2018 12:52:38 GMT\r\n"
            "Last-Modified: Mon, 02 Jul 2018 02:11:02 GMT\r\n"
            "Server: Elvis0nA1r\r\n"
            "\r\n"
            "Fuck you bitch!!!!";
    
    boost::asio::async_write(
        socket_
        , boost::asio::buffer(message_)
        , boost::bind(
            &http_connection::handle_write
            , shared_from_this()
            , boost::asio::placeholders::error
            , boost::asio::placeholders::bytes_transferred
        )
    );
}


void http_server::start_accept()
{
    http_connection::pointer conn = http_connection::create(acceptor_.get_io_service());

    acceptor_.async_accept(
        conn->socket()
        , boost::bind(&http_server::handle_accept
            , this
            , conn
            , boost::asio::placeholders::error
        )
    );
}

void http_server::handle_accept(
    http_connection::pointer conn
    , const boost::system::error_code& error
)
{
    if (!error) {
        num_reqs++;
        if (num_reqs%1000 == 0) {
            cout << num_reqs << " requests!" << endl;
        }
        
        conn->start();
        start_accept(); // the reference to the previous tcp_connection turns to 0, so will be destroyed by the shared_ptr
    } else {
        cout << "Accept error!" << endl;
    }
}

http_server::http_server(io_service& ios)
: acceptor_(ios, tcp::endpoint(tcp::v4(), 10000))
{
    num_reqs = 0;
    start_accept();
}



int main()
{
    try {
        boost::asio::io_service io_service;
        http_server server(io_service);
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}