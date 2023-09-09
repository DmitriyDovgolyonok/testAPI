#include <boost/asio.hpp>
#include <iostream>

#include "WindowsUpdate.h"

using boost::asio::ip::tcp;

void handleRequest(tcp::socket& socket, const std::string& json)
{
	try
	{
        std::ostringstream response_stream;
        response_stream << "HTTP/1.1 200 OK\r\n";
        response_stream << "Content-Type: application/json\r\n";
        response_stream << "Content-Length: " << json.length() << "\r\n\r\n";
        response_stream << json;

        boost::asio::write(socket, boost::asio::buffer(response_stream.str()));
	}
	catch (std::exception& e)
	{
        std::cerr << "Exception: " << e.what() << std::endl;
	}
}
int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server started. Listening on port 8080..." << std::endl;

        SetConsoleOutputCP(CP_UTF8);
        WindowsUpdate windowsUpdate;
        if (!windowsUpdate.Initialize())
            return 1;

        const char* criteria = "";
        long startIndex = 0;
        long count = 10; // Можно изменить для желаемого количества записей 

        if (windowsUpdate.GetUpdateHistory(criteria, startIndex, count))
            windowsUpdate.DisplayUpdateHistory();
        while(true)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            std::string json = windowsUpdate.GetUpdateJSON();
            handleRequest(socket, json);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

