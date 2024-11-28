#ifndef REQUEST_HPP
# define REQUEST_HPP


#include "Webserv.hpp"

class Request
{
    public:
        std::string method;
        std::string request;
        std::string FilePath;
        std::string connexion;
        std::string content_type;
        std::string content_length;
        std::string body;
        bool cgi_state;
        size_t serv_fd;
        size_t client_fd;
		ssize_t bytes_read;
        std::vector <unsigned char> data;
		char *buffer;

        Request(std::string method, std::string request);
        void    setFilePath(std::string FilePath);
        void    setConnexion(std::string Connexion);
        ~Request();
        Request();
    private:

};

#endif