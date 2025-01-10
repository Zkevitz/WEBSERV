#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include "Webserv.hpp"
#include "Request.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>

class Cgi {
    private :
        std::map<std::string, std::string>	env;
        std::string exec_path;
        std::string post_body;
        std::string query_string;
        pid_t   pid;
        char **char_env;
        int pipe_fd[2];
    public:
        Cgi();
        Cgi(std::string path, std::string method, Request Req);
        ~Cgi();
        
        int client_fd;
        int exit_code;
        int get_pipe_fd(int side);
        int get_pid();
        std::string    exec_cgi();

};

#endif