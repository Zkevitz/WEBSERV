#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Webserv.hpp"
# include "Cgi.hpp"
# include <map>
class Cgi;

class Request
{
    public:
        std::string method;
        std::string request;
        std::string FilePath;
        std::string connexion;
        std::string content_type;
        std::string content_length;
        std::string http_code;
        std::string cgi_content;
        std::string hostname;
        std::string body;
        std::map<int, Cgi> cgi_map;
        Cgi *cgi_;
        int cgi_state;
        size_t serv_fd;
        size_t client_fd;
		ssize_t bytes_read;
        ssize_t max_body_size;
        std::vector <unsigned char> data;
		char *buffer;
        bool error;
        bool autoindex;

        Request(std::string method, std::string request);
        void    setFilePath(std::string FilePath);
        void    setConnexion(std::string Connexion);
        ~Request();
        Request();
    private:

};

#endif