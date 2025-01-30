#include "../include/Server.hpp"

void    Server::sendCgiResponse(int client_fd, int pos)
{
    if (send(client_fd, Reqmap[client_fd].cgi_content.c_str(), Reqmap[client_fd].cgi_content.size(), 0) <= 0)
        close_connexion(client_fd, pos);
    Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "client %d reponse envoyer with HTTP code : %s", client_fd, Reqmap[client_fd].http_code.c_str());
    Reqmap[client_fd].request = "";
    Reqmap[client_fd].method = "";
    Reqmap[client_fd].cgi_state = 0;
    Reqmap[client_fd].cgi_content = "";
    Reqmap[client_fd].connexion = "";
    Reqmap[client_fd].content_length = "";
    Reqmap[client_fd].content_type = "";
    Reqmap[client_fd].body = "";
    Reqmap[client_fd].content_type = "";
    Reqmap[client_fd].FilePath = "";
    Reqmap[client_fd].data.clear();
    close_connexion(client_fd, pos);
}