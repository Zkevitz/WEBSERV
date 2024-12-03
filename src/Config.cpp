#include "../include/Config.hpp"
#include "../include/Webserv.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

Config::Config() {}

Config::~Config() {}

bool Config::parseConfigFile(const std::string& filename) {
    std::ifstream configFile(filename.c_str());
    if (!configFile.is_open()) {
        std::cerr << "Error: Unable to open configuration file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        trim(line);
        if (line == "server {") {
            parseServerBlock(configFile);
        }
    }

    configFile.close();
    return true;
}

void Config::parseServerBlock(std::ifstream& file) {
    ServerConfig serverConfig;
    std::string line;

    while (std::getline(file, line)) {
        trim(line);  // Remove whitespace

        if (line == "}") {
            break;  // End of server block
        }

        if (line.find("listen") == 0) {
            serverConfig.port = extractPort(line);
        } else if (line.find("server_name") == 0) {
            serverConfig.hostname = extractServerName(line);
        } else if (line.find("root") == 0) {
            serverConfig.root = extractRoot(line);
        } else if (line.find("index") == 0) {
            serverConfig.index = extractIndex(line);
        } else if (line.find("error_pages") == 0) {
            std::map <int, std::string> err_page = extractErrPages(line);
            serverConfig.error_pages.insert(err_page.begin(), err_page.end());
        }
    }
    servers.push_back(serverConfig);
}
std::map <int, std::string> Config::extractErrPages(const std::string &line)
{
    std::map <int, std::string> node;
    std::istringstream iss(line);
    std::string token;
    std::string err_path;
    int err_code;


    iss >> token; // skip error_page 
    iss >> err_code >> err_path;
    node[err_code] = err_path;
    return node;
}

void Config::trim(std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    if (first != std::string::npos && last != std::string::npos) {
        str = str.substr(first, (last - first + 1));
    } else {
        str.clear(); // If the string is empty or consists only of whitespace
    }
}

int Config::extractPort(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip "listen" and get port
    return static_cast<int>(std::atoi(token.c_str())); // Convert to int
}

std::string Config::extractServerName(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip "server_name" and get name
    if (token == "localhost")
        token = "127.0.0.1";
    return token;
}

std::string Config::extractRoot(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip "root" and get path
    return token;
}

std::string Config::extractIndex(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip "index" and get index file
    return token;
}

const std::vector<ServerConfig>& Config::getServers() const {
    return servers;
}
