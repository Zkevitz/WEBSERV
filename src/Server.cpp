#include "../include/Server.hpp"
#include "../include/Webserv.hpp"
#include "../include/Cgi.hpp"
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <poll.h>

Server::Server(const std::string& hostname, int port) : hostname(hostname), port(port), server_fd(-1) {}

Server::Server():amount_of_serv(0){}

Server::~Server() {
    if (server_fd != -1)
        close(server_fd);
}

bool Server::setup() {
    return createSocket() && bindSocket() && makeNonBlocking();
}

bool Server::createSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error: Failed to create socket." << std::endl;
        return false;
    }
    all_serv_fd.push_back(server_fd);
    return true;
}

bool Server::bindSocket() {
   static int i = 0;
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = inet_addr(all_hostname[i].c_str()); // Use the hostname from config
    address.sin_port = htons(all_port[i]); // Set the port
    printf("%s\n", all_hostname[i].c_str());
    all_sock_addr.push_back(address);
    if (bind(all_serv_fd[i], (struct sockaddr*)&all_sock_addr[i], sizeof(all_sock_addr[i])) < 0) {
        perror("bind failed");
        return false;
    }
    i++;
    return true;
}

void    Server::add_serv(ServerConfig newServ)
{
    (void)port;
    static int i = 0;


    all_hostname.push_back(newServ.hostname);
    all_hostname_str.push_back(newServ.hostname.c_str());
    all_port.push_back(newServ.port);

    location_rules[amount_of_serv + 3] = newServ.location_rules;

    Body_size[amount_of_serv + 3] = newServ.max_body;
    if(Body_size[amount_of_serv + 3] == 0)
        Body_size[amount_of_serv + 3] = 10000;


    if(newServ.error_pages.size() > 0)
        err_pages[amount_of_serv + 3] = newServ.error_pages;
    i++;
    amount_of_serv++;
}

void    Server::Check_TimeOut()
{
    for(std::map<int, time_t>::iterator it = TimeOutMap.begin() ; it != TimeOutMap.end(); ++it)
    {
        if ((time(NULL) - it->second) > 60)
        {
            size_t position = std::distance(TimeOutMap.begin(), it);
            position += all_serv_fd.size();
            Msg::logMsg(YELLOW, CONSOLE_OUTPUT, "Client %d Timeout, Closing Connection..", it->first);
            close_connexion(it->first, position);
            return;
        }
    }
}

void    Server::close_connexion(int client_fd, size_t pos)
{
    if(std::find(all_client_fd.begin(), all_client_fd.end(), client_fd) != all_client_fd.end())
    {
        Msg::logMsg(YELLOW, CONSOLE_OUTPUT, "Connexion with client : [%d] closed", client_fd);


        std::vector<int>::iterator it = all_client_fd.begin();
        (void) it;
        all_client_fd.erase(all_client_fd.begin() + (pos - all_serv_fd.size()));
        poll_fds.erase(poll_fds.begin() + pos);
        Reqmap.erase(client_fd);
        TimeOutMap.erase(client_fd);
        close(client_fd);
    }
    
}
std::string Server::read_cgi_output(int client_fd, size_t i)
{
    int child_status;
    std::string content;
    int read_fd = Reqmap[client_fd].cgi_->get_pipe_fd(0);
    int write_fd = Reqmap[client_fd].cgi_->get_pipe_fd(1);
    int send_fd = Reqmap[client_fd].cgi_->client_fd;
    char cgi_buffer[1024];
    ssize_t bytes_read;

    close(write_fd);
    while ((bytes_read = read(read_fd, cgi_buffer, sizeof(cgi_buffer) - 1)) > 0) {
        cgi_buffer[bytes_read] = '\0'; // Null-terminate the string
        //std::cout << cgi_buffer << std::endl;
        content += cgi_buffer;
    }
    if (bytes_read == - 1)
    {
        wait(&child_status);
        close(read_fd);
        close_connexion(client_fd, i);
        return ("");
    }
    wait(&child_status);
    close(read_fd);
    int status;
    waitpid(Reqmap[client_fd].cgi_->get_pid(), &status, 0);
    std::string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(content.size()) + "\r\n"
        "\r\n" + 
        content;
    send(send_fd, http_response.c_str(), http_response.size(), 0);
    Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "client %d reponse envoyer with HTTP code : %s", client_fd, Reqmap[client_fd].http_code.c_str());
    close_connexion(client_fd, i);
    return(content);
}
std::string Server::init_cgi_param(std::string str, Request& Req)
{
    Req.cgi_ = new Cgi(str, Req.method, Req);

    Req.cgi_->exec_cgi();
    int read_fd = Req.cgi_->get_pipe_fd(0);
    add_client_to_poll(read_fd);
    Reqmap[read_fd].cgi_state = 2;
    Reqmap[read_fd].cgi_ = Req.cgi_;
    return("");
}

bool Server::makeNonBlocking() {
    //muavais flag
    int flags = fcntl(server_fd, FD_CLOEXEC, 0);
    if (flags == -1) {
        std::cerr << "Error: Failed to get socket flags." << std::endl;
        return false;
    }

    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error: Failed to make socket non-blocking." << std::endl;
        return false;
    }

    return true;
}

void Server::start() {
     // << "AMOUNT OF SERV = " << amount_of_serv << std::endl;
    for(size_t i = 0; i < amount_of_serv ; i++)
    {
        if (listen(all_serv_fd[i], SOMAXCONN) < 0) {
            std::cerr << "Error: Failed to listen on socket." << std::endl;
            return;
        }
        Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "Server listening on %s : %d", this->getHostname(i), all_port[i]);
    }
    acceptConnections();
}

void Server::initializePollFds()
{
    this->poll_fds.clear(); 
    for (size_t i = 0; i < this->all_serv_fd.size(); i++)
    {
        pollfd pfd;
        pfd.fd = all_serv_fd[i];
        pfd.events = POLLIN;  // Surveillez les lectures pour les serveurs
        pfd.revents = 0;
        poll_fds.push_back(pfd);
    }
    for (size_t i = 0; i < this->all_client_fd.size(); i++)
    {
        pollfd pfd;
        pfd.fd = all_client_fd[i];
        pfd.events = POLLIN | POLLOUT; // Surveillez les lectures et écritures pour les clients
        pfd.revents = 0;
        poll_fds.push_back(pfd);
    }
    biggest_fd = all_serv_fd[all_serv_fd.size() - 1];
}
void    Server::add_client_to_poll(int client_fd)
{
    all_client_fd.push_back(client_fd);
    TimeOutMap[client_fd] = time(NULL);
    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
    poll_fds.push_back(pfd);
    if(client_fd > biggest_fd)
        biggest_fd = client_fd;
    Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "New Client Connection on fd %d len of poll struct is now %d", client_fd, poll_fds.size());
}


int Server::compare_poll(size_t size)
{
   for(size_t i = 0; i < poll_fds.size(); i++)
   {
        if ((int)size == poll_fds[i].fd)
            return 1;
   }
   return 0;
}

const char* Server::getHostname(int Vecpos)
{
    return(all_hostname[Vecpos].c_str());
}

int Server::getPort(int Vecpos)
{
    return(all_port[Vecpos]);
}

void    Server::close_all_fd()
{
    std::cout << "yo la vie" << std::endl;
    size_t i = 0;
    size_t j = 0;
    while(i < all_serv_fd.size())
    {
        close(this->all_serv_fd[i]);
        i++;
    }
    while(j < all_client_fd.size())
    {
        close_connexion(all_client_fd[j], i);
        j++;
        i++;
    }
}
void Server::acceptConnections() {

    initializePollFds();
    while (true) {
        int poll_ret = poll(poll_fds.data(), poll_fds.size(), 1000);  // Timeout en millisecondes
        if (poll_ret < 0)
        {
            if(!running)
                Msg::logMsg(RED, CONSOLE_OUTPUT, "Webserv closing poll...");
            else
                std::cerr << "webserv: poll error   Closing ...." << std::endl;
            close_all_fd();
            exit(1);
        }
        for(size_t i = 0; i < poll_fds.size(); i++)
        {
            int fd = poll_fds[i].fd;
            if(!compare_poll(fd))
                continue;
            Msg::logMsg(RED, FILE_OUTPUT, "Revent for fd : %d  = %d and ERRNO = %d ", fd, poll_fds[i].revents, strerror(errno));
            if (poll_fds[i].revents & POLLHUP)
            {
                Msg::logMsg(YELLOW, CONSOLE_OUTPUT, "Poll close connexion event on fd [%d]...", fd);
                close_connexion(fd, i);
            }
            else if (poll_fds[i].revents & POLLIN)
            {
                if(std::find(all_serv_fd.begin(), all_serv_fd.end(), fd) != all_serv_fd.end())
                {
                    struct sockaddr_in client_address;
                    socklen_t client_addr_len = sizeof(client_address);
                    int client_fd = accept(fd, (struct sockaddr*)&client_address, &client_addr_len);
                    Reqmap[client_fd].serv_fd = fd;
                    
                    if (errno == EWOULDBLOCK || errno == EAGAIN)
                            continue;
                    if (client_fd < 0) { 
                            Msg::logMsg(RED, CONSOLE_OUTPUT, "Error: Failed to accept connection. Errno: %s", strerror(errno));
                            exit(1);
                    }
                    add_client_to_poll(client_fd);
                }
                else if(std::find(all_client_fd.begin(), all_client_fd.end(), fd) != all_client_fd.end())
                {
                    if(Reqmap[fd].cgi_state == 2)
                        read_cgi_output(fd, i);
                    else
                        readrequest(fd, i);
                }
            }
            else if (poll_fds[i].revents & POLLOUT)
            {
                if(std::find(all_client_fd.begin(), all_client_fd.end(), fd) != all_client_fd.end())
                {
                    std::string method = Reqmap[fd].method;
                    if (method == "GET" || Reqmap[fd].cgi_state == 1) {
                        serveFile(fd, Reqmap[fd].FilePath, i);
                    } else if (method == "POST") {
                        handlePost(fd, Reqmap[fd].request, Reqmap[fd].FilePath, Reqmap[fd].bytes_read, Reqmap[fd].data, i); // New function for handling POST requests
                    } else if (method == "DELETE") {
                        handleDelete(fd, Reqmap[fd].FilePath, i); // New function for handling DELETE requests
                    }
                }
            }
        }
        Check_TimeOut();
    }
}
// void Server::look_for_rules(std::string FilePath)
// {
//     if(FilePath.find())
// }
void Server::readrequest(int client_fd, size_t pos) {

    (void)pos;
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "Request from client %d : we get %d bytes_read\n", client_fd, bytes_read);
    if (bytes_read <= 0) {
        if(bytes_read == 0)
            Msg::logMsg(YELLOW, CONSOLE_OUTPUT, "Connexion closed by read end on client : [%d]", client_fd);
        else if(bytes_read < 0)
            Msg::logMsg(YELLOW, CONSOLE_OUTPUT, "Connexion closed by read error on client : [%d]", client_fd);
        close_connexion(client_fd, pos);
        return;
    }
    else if(bytes_read == 9999)
    {
        std::string request(buffer);
        Reqmap[client_fd].data.insert(Reqmap[client_fd].data.end(), buffer, buffer + bytes_read);
        return;
    }
    else if(bytes_read > 0 && bytes_read < 9999)
    {
        std::string request(buffer);
        if(Reqmap[client_fd].data.size() > 0)
        {
            printf("je suis une requetes fragmenter et je termine mon job la taille de ma requete = %lu\n", Reqmap[client_fd].data.size());
            Reqmap[client_fd].request += request;
            Reqmap[client_fd].data.insert(Reqmap[client_fd].data.end(), buffer, buffer + bytes_read);
            std::string request(Reqmap[client_fd].data.begin(), Reqmap[client_fd].data.end());
        }
        else
        {
            Reqmap[client_fd].request = request;
            Reqmap[client_fd].data.insert(Reqmap[client_fd].data.end(), buffer, buffer + bytes_read);
        }
        std::vector<unsigned char> fileContent(Reqmap[client_fd].data.begin(), Reqmap[client_fd].data.end());
        std::string fileContentAsString(fileContent.begin(), fileContent.end());
        TimeOutMap[client_fd] = time(NULL);
        Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "------------------------------------------------------------");
        Msg::logMsg(DARK_GREY, CONSOLE_OUTPUT, "Request : the complete request = %s", fileContentAsString.c_str());
        Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "------------------------------------------------------------");
        std::string method, path, connexion;
        size_t method_end = fileContentAsString.find(' ');
        if (method_end != std::string::npos) {
            method = fileContentAsString.substr(0, method_end);
            size_t path_end = fileContentAsString.find(' ', method_end + 1);
            if (path_end != std::string::npos) {
                path = fileContentAsString.substr(method_end + 1, path_end - method_end - 1);
            }
        }
        size_t connexion_pos = fileContentAsString.find("Connection:");
        size_t Referer_pos = fileContentAsString.find("Referer");
        size_t Host_pos = fileContentAsString.find("Host:");
        size_t User_Agent_pos = fileContentAsString.find("User-Agent:");
        Reqmap[client_fd].hostname = fileContentAsString.substr(Host_pos + 6, User_Agent_pos - (Host_pos + 6));
        size_t double_point = Reqmap[client_fd].hostname.find(":");
        Reqmap[client_fd].hostname = Reqmap[client_fd].hostname.substr(0, double_point);
        if(CheckValidHost(Reqmap[client_fd].hostname) == 1)
        {
            Reqmap[client_fd].cgi_state = 0;
            sendError(client_fd, "400 Bad Request", pos);
            return;
        }
        connexion = fileContentAsString.substr(connexion_pos + 11 , Referer_pos - connexion_pos);

        Reqmap[client_fd].setConnexion(connexion);
        Reqmap[client_fd].method = method;
        Reqmap[client_fd].request = fileContentAsString;
        Reqmap[client_fd].data = fileContent;
        Reqmap[client_fd].client_fd = client_fd;
        Reqmap[client_fd].bytes_read = fileContent.size();
        std::string FilePath;
        printf("Body_size : %lu\n", Body_size[Reqmap[client_fd].serv_fd]);
        if (Reqmap[client_fd].bytes_read >  Body_size[Reqmap[client_fd].serv_fd])
        {
                Reqmap[client_fd].cgi_state = 0;
                sendError(client_fd, "413 Entity Too Large", pos);
                return;
        }
        if (method == "GET")
        {
            FilePath = getFilePath(path);
            Reqmap[client_fd].http_code = "200 OK";
        } 
        else if (method == "POST")
        {
            size_t Content_type_pos = fileContentAsString.find("Content-Type");
            size_t Content_length_pos = fileContentAsString.find("Content-Length: ");
            size_t Origin_pos = fileContentAsString.find("Origin");
            size_t param_pos = fileContentAsString.find("param");
            Reqmap[client_fd].body = fileContentAsString.substr(param_pos + 6, fileContentAsString.size() - param_pos);
            Reqmap[client_fd].content_type = fileContentAsString.substr(Content_type_pos + 12, Content_length_pos - Content_type_pos);
            Reqmap[client_fd].content_length = fileContentAsString.substr(Content_length_pos + 15, Origin_pos - (Content_length_pos + 15));
            Reqmap[client_fd].http_code = "201 Created";
            FilePath = getFilePath(path);
        } 
        else if (method == "DELETE") 
        {
            size_t filename_begin = fileContentAsString.find("delete/");
            filename_begin += 7;
            size_t end_file_name = fileContentAsString.find("HTTP/1.1", filename_begin);
            std::string file_name = fileContentAsString.substr(filename_begin, end_file_name - filename_begin - 1);
            FilePath = "./uploads/" +  file_name;
        } 
        else 
        {
            Reqmap[client_fd].http_code = "501 Not Implemente";
            Reqmap[client_fd].cgi_state = 0;
            sendError(client_fd, "501 Not Implemented", pos);
            this->poll_fds[pos].events = POLLIN;
            return;
        }
        if(FilePath.find("cgi-bin") != std::string::npos)
            Reqmap[client_fd].cgi_state = 1;
        else
            Reqmap[client_fd].cgi_state = 0;
        //look_for_rules(FilePath);
        Reqmap[client_fd].setFilePath(FilePath);
        this->poll_fds[pos].events = POLLOUT;
    }
}

std::string Server::getFilePath(const std::string& request_path) {
    std::string base_directory = "./www";  // Define the base directory for static files
    std::string file_path = base_directory + request_path;
    if (file_path.back() == '/') file_path += "index.html";
    return file_path;
}
std::string trim_cgi_param(std::string str)
{
    if(str.find_first_of('?') == std::string::npos)
        return str;
    else
    {
        int i = str.find('?');
        return str.substr(0, i);
    }
}
void Server::serveFile(int client_fd, const std::string& file_path, size_t pos) {
    std::string real_file_path = trim_cgi_param(file_path);
    std::ifstream file(real_file_path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        sendError(client_fd, "404", pos);
        return;
    }
    std::string file_content;
    file.close();
    if(Reqmap[client_fd].cgi_state == 1)
    {
            file_content = init_cgi_param(file_path, Reqmap[client_fd]);
            Reqmap[client_fd].request = "";
            Reqmap[client_fd].data.clear();
            this->poll_fds[client_fd - all_serv_fd.size() - 1].events = POLLIN;
            return;
    }
    else
    {
        std::ifstream file(real_file_path);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open HTML file." << std::endl;
            return;
        }
        file_content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

    }
    std::string content_type = Reqmap[client_fd].cgi_state ? "text/html; charset=UTF-8" : getContentType(real_file_path);
    std::string http_code = Reqmap[client_fd].http_code;
    if(http_code == "501 Not Implemented")
        content_type = "application/x-www-form-urlencoded";
    std::cout << "cgi_state = " << Reqmap[client_fd].cgi_state << std::endl;
    std::cout << "HTTP CODE = " << http_code << std::endl;
    std::cout << "HTTP CODE2 = " << content_type << std::endl;
    std::string http_response =
        "HTTP/1.1 " + http_code + " \r\n"
        "Content-Type: " + content_type + "\r\n"
        "Content-Length: " + std::to_string(file_content.size()) + "\r\n"
        "\r\n" + 
        file_content;
    send(client_fd, http_response.c_str(), http_response.size(), 0);
    Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "client %d reponse envoyer with HTTP code : %s", client_fd, Reqmap[client_fd].http_code.c_str());
    Reqmap[client_fd].request = "";
    Reqmap[client_fd].data.clear();
    this->poll_fds[pos].events = POLLIN;
}

std::string Server::getContentType(const std::string& file_path) {
    if (file_path.substr(file_path.size() - 5) == ".html") return "text/html; charset=UTF-8";
    if (file_path.substr(file_path.size() - 3) == ".py") return "text/html";
    if (file_path.substr(file_path.size() - 4) == ".css") return "text/css";
    if (file_path.substr(file_path.size() - 3) == ".js") return "application/javascript";
    if (file_path.substr(file_path.size() - 4) == ".jpg" || file_path.substr(file_path.size() - 5) == ".jpeg") return "image/jpeg";
    if (file_path.substr(file_path.size() - 4) == ".png") return "image/png";
    return "multipart/form-data"; // ANCIENNEMENT text/html;
}

void Server::sendInvalidUploadResponse(int client_fd) {
    std::string response = "HTTP/1.1 400 Bad Request\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: 40\r\n"
                           "\r\n"
                           "Invalid file upload";
    send(client_fd, response.c_str(), response.size(), 0);
}

const std::string Server::find_err_path(int serv_fd, int err_code)
{
    if(err_pages.find(serv_fd) != err_pages.end())
    {
        if(err_pages[serv_fd].find(err_code) != err_pages[serv_fd].end())
        {
            return(err_pages[serv_fd][err_code]);
        }
    }
    std::string err_path = "./Www/error_pages/error" + myItoa(err_code) + ".html";
    return(err_path);
}

void Server::sendError(int client_fd, std::string err_code, size_t pos) 
{
    int int_err_code = std::atoi(err_code.c_str());
    std::string err_page_path = find_err_path(Reqmap[client_fd].serv_fd, int_err_code);
    Reqmap[client_fd].http_code = err_code;
    serveFile(client_fd, err_page_path, pos);
}
int Server::CheckValidHost(std::string host)
{
    if(host == "localhost")
        return(0);
    for(size_t i = 0; i < all_hostname_str.size(); i++)
    {
        printf("string = %s\n", all_hostname_str[i].c_str());
        if(all_hostname_str[i] == host)
            return(0);
    }
    return(1);
}
void Server::handlePost(int client_fd, const std::string& request, const std::string& path, size_t request_length, std::vector<unsigned char> data, size_t pos)
{
    (void) path;
    (void) request_length;
    int write_rtn;
    size_t boundary_start = request.find("boundary=") + 9;
    size_t boundary_end = request.find("C", boundary_start) - 1; // Find the end of the boundary
    std::string boundary;
    if (boundary_end != std::string::npos) {
        boundary = "--" + request.substr(boundary_start, boundary_end - boundary_start);
    }
    size_t start = request.find(boundary);
    if (start == std::string::npos) {
        std::cerr << "Error: Boundary not found." << std::endl;
        sendInvalidUploadResponse(client_fd);
        return;
    }
    start += boundary.length();
    while (start != std::string::npos) {
        size_t end = request.find(boundary, start);
        size_t content_disposition_start = request.find("Content-Disposition:", 0);
        if (content_disposition_start == std::string::npos || content_disposition_start >= end) {
            std::cerr << "Error: Content-Disposition not found." << std::endl;
            sendInvalidUploadResponse(client_fd);
            return;
        }
        size_t content_disposition_end = request.find("\r\n\r\n", content_disposition_start);
        std::string content_disposition = request.substr(content_disposition_start, content_disposition_end - content_disposition_start);
        size_t filename_begin = request.find("filename=");
        filename_begin += 10; // Length of "Content-Length: "
        size_t end_file_name = request.find("\r\n", filename_begin);
        std::string file_name = request.substr(filename_begin, end_file_name - filename_begin - 1);

        // Check if it's a file part
        if (filename_begin != std::string::npos) {
            // Get the file content
            size_t file_start = content_disposition_end + 4; // Skip "\r\n"
            std::string file_path = "./uploads/" +  file_name; // Adjust path as necessary
            int f = open(file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
           
            data[data.size() -1] = 0;
            
            unsigned char *l = reinterpret_cast<unsigned char*>(&data[file_start]);
            for (size_t i = 0; i < data.size() - file_start - (boundary.size() + 4); i++)
                write_rtn = write(f, &l[i], 1);
            // Respond back to the client
            if(write_rtn <= 0)
            {
                close_connexion(client_fd, pos);
                return;
            }
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                                   "File uploaded successfully";
       
            send(client_fd, response.c_str(), response.size(), 0);
           
            std::cout << this->poll_fds[pos].fd << std::endl;
            
            Reqmap[client_fd].request = "";
            
            Reqmap[client_fd].data.clear();
            this->poll_fds[pos].events = POLLIN;
            return;
        }
        start = end;
    }
    sendInvalidUploadResponse(client_fd);
}




void Server::handleDelete(int client_fd, const std::string& file_path, size_t pos)
{
    std::string response;
    if (fileExists(file_path))
    {
        if (std::remove(file_path.c_str()) == 0)
        {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                       "File deleted successfully";
        }
        else
        {
            response = "HTTP/1.1 500 Not Found\r\nContent-Type: text/html\r\n\r\n"
                   "Internal serveur error";
        }
    }
    else
    {
        response = "HTTP/1.1 404 Server Error\r\nContent-Type: text/html\r\n\r\n"
                       "FILE NOT FOUND";
    }
    send(client_fd, response.c_str(), response.size(), 0);
    std::cout << this->poll_fds[pos].fd << std::endl;
    Reqmap[client_fd].request = "";
    Reqmap[client_fd].data.clear();
    this->poll_fds[pos].events = POLLIN;
}
