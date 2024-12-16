#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <fstream>
#include "Webserv.hpp"

struct ServerConfig {
    int port;
    std::string hostname;
    std::string hostname_str;
    std::string root;
    std::string index;
    std::map<int, ssize_t> max_body;
    std::map <int, std::string> error_pages;
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
    void trim(std::string& str); // New method to trim whitespace
    int extractPort(const std::string& line); // New method to extract port
    std::string extractServerName(const std::string& line); // New method to extract server name
    std::string extractRoot(const std::string& line); // New method to extract root
    std::string extractIndex(const std::string& line); // New method to extract index
    std::map <int, std::string> extractErrPages(const std::string &line);
    std::string extractValue(const std::string& line);
};

#endif

