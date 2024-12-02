#ifndef SERVER_HPP
# define SERVER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"

extern bool running;
struct ServerConfig;

class Server {
public:
    Server();
    Server(const std::string& hostname, int port);
    ~Server();

    bool setup();
    void    add_serv(ServerConfig newServ);
    void start();

private:
    std::string hostname;
    int port;
    int biggest_fd;
    int server_fd; // File descriptor for the server socket
    size_t amount_of_serv;
    struct sockaddr_in address;

    std::vector <int> all_serv_fd;
    std::vector <int> all_client_fd;
    std::vector <pollfd> poll_fds;
    std::vector <int> all_port;
    std::vector <std::string> all_hostname;
    std::vector <sockaddr_in> all_sock_addr;
    std::map<int, Request> Reqmap;
    std::map<int, std::map<int, std::string> > err_pages;
    std::map<int, time_t> TimeOutMap;


    // New private methods for static file serving
    const std::string find_err_path(int serv_fd, int err_code);
    void Check_TimeOut();
    void close_connexion(int client_fd, size_t pos);
    void readrequest(int client_fd, size_t pos); // Handles incoming client requests
    std::string getFilePath(const std::string& request_path); // Gets the file path based on the request
    void serveFile(int client_fd, const std::string& file_path, size_t pos); // Serves the requested file
    std::string getContentType(const std::string& file_path); // Gets the content type based on the file extension
    void sendError(int client_fd, std::string err_code); // Sends all error_page
    void handlePost(int client_fd, const std::string& request, const std::string& path, size_t request_length, std::vector<unsigned char> buffer, size_t pos);
    void handleDelete(int client_fd, const std::string& file_path, size_t pos);
    void sendInvalidUploadResponse(int client_fd);
    // Existing methods
    bool createSocket();
    bool bindSocket();
    bool makeNonBlocking();
    void acceptConnections();
    void add_client_to_poll(int client_fd);
    void initializePollFds();
    std::string read_cgi_output(int client_fd, size_t i);
    std::string init_cgi_param(std::string str, Request Req);
    int compare_poll(size_t size);
};

#endif // SERVER_HPP