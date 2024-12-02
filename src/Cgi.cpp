#include "../include/Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

Cgi::Cgi(std::string path, std::string method, Request Req)
{
    int i = 0;
    this->post_body = "";
    this->client_fd = Req.client_fd;
    if(path.find_first_of('?') == std::string::npos)
        this->exec_path = path;
    else
    {
        i = path.find('?');
        this->exec_path = path.substr(0, i);
        this->query_string = path.c_str() + i + 1;
    }
    this->env["REQUEST_METHOD"] = method;
    this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->env["PATH_INFO"] = this->exec_path;
    std::cout << "voila la method " << method << std::endl;
    if(method == "GET" && i > 0)
        this->env["QUERY_STRING"] = this->query_string;
    else if(method == "POST")
    {
        this->env["CONTENT_LENGTH"] = Req.content_length;
        this->env["CONTENT_TYPE"] = Req.content_type;
        this->post_body = Req.body;
    }

    this->char_env = (char **)calloc(sizeof(char *), this->env.size() + 1);
    std::map<std::string, std::string>::const_iterator it = this->env.begin();
    for (size_t i = 0; i < this->env.size(); i++, it++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->char_env[i] = strdup(tmp.c_str());
	}
}
int Cgi::get_pipe_fd(int side)
{
    if(side == 0)
        return(this->pipe_fd[0]);
    else if (side == 1)
        return(this->pipe_fd[1]);
    else
        return(this->pipe_fd[0]);
}

int Cgi::get_pid()
{
    return(this->pid);
}

std::string Cgi::exec_cgi()
{
    std::string content = "";
    //int child_status;

    if (pipe(this->pipe_fd) == -1) {
        std::cerr << "Error: Unable to create pipe in." << std::endl;
        return ""; // implementer une sortie
    }
    this->pid = fork();
    if (pid == -1) {
        std::cerr << "Error: Unable to fork." << std::endl;
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return ""; // implementer une sortie 
    }
    if (pid == 0) // processus enfant
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        if(this->post_body.size() > 0)
        {
            int post_pipe[2];
            pipe(post_pipe);

            dup2(post_pipe[0], 0);
            write(post_pipe[1], this->post_body.c_str(), this->post_body.size());
            close(post_pipe[1]);
        }
        std::vector<char*> args;
        args.push_back(const_cast<char*>(this->exec_path.c_str()));
        args.push_back(nullptr);
        if (access(this->exec_path.c_str(), X_OK) != 0) {
            std::cerr << "Error: exec_path is not executable." << std::endl;
            perror("access");
            exit(1);
        }

        if (execve(this->exec_path.c_str(), args.data(), this->char_env) == -1) {
            std::cerr << "Error: execve failed." << std::endl;
            exit(1);
        }
    }
    else // processus parent
    {   
        // close(pipe_fd[1]);
        // char cgi_buffer[1024];
        // ssize_t bytes_read;
        // while ((bytes_read = read(pipe_fd[0], cgi_buffer, sizeof(cgi_buffer) - 1)) > 0) {
        //     cgi_buffer[bytes_read] = '\0'; // Null-terminate the string
        //     std::cout << cgi_buffer << std::endl;
        //     content += cgi_buffer;
        // }
        // wait(&child_status);
    }
    //close(pipe_fd[0]); 
    // int status;
    // waitpid(this->pid, &status, 0);
    return(content);
}