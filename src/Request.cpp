#include "../include/Webserv.hpp"

Request::Request(){
}
Request::Request(std::string method, std::string request): method(method), request(request)
{
}
Request::~Request(){};

void    Request::setFilePath(std::string FilePath)
{
    this->FilePath = FilePath;
}
void    Request::setConnexion(std::string Connexion)
{
    size_t check = Connexion.find("keep-alive");
    if(check != std::string::npos)
        this->connexion = "keep-alive";
    else
        this->connexion = "close";
}
