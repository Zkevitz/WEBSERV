#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <fstream>
#include "Webserv.hpp"

struct rules {
    bool state;
    std::string prefix;
    std::string redirect;
    std::string root;
    std::string index;
    std::vector<std::string> allowed_methods;
    std::map <int, std::string> error_pages;
    bool autoindex;
    std::string _clientMaxBodySize;
    std::string upload_path;
};

struct ServerConfig {
    int port;
    std::vector<int> listen_ports;
    std::string hostname;
    std::string root;
    std::string index;
    std::map<int, ssize_t> max_body;
    std::map <int, std::string> error_pages;
    std::map<std::string, rules> location_rules;
};

class Config {
public:
    Config();
    ~Config();

    bool parseConfigFile(const std::string& filename);
    const std::vector<ServerConfig>& getServers() const; // New getter for server configurations

private:
    std::vector<ServerConfig> servers; // Store multiple server configurations

    // Helper functions
    void parseLine(const std::string& line);
    void parseServerBlock(std::ifstream& file); // New method for parsing server blocks
    std::map<std::string, rules> parseLocationBlock(std::ifstream& file, std::string line);
    void trim(std::string& str); // New method to trim whitespace
    int extractPort(const std::string& line); // New method to extract port
    bool extractAutoIndex(const std::string& line);
    std::vector <std::string> extractMethod(const std::string &line);
    std::string extractServerName(const std::string& line); // New method to extract server name
    std::string extractRoot(const std::string& line); // New method to extract root
    std::string extractRedirect(const std::string& line);
    std::string extractIndex(const std::string& line); // New method to extract index
    std::map <int, std::string> extractErrPages(const std::string &line);
    std::string extractValue(const std::string& line);
    int extractMaxBody(const std::string &line);
};

#endif

