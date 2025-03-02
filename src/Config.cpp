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

void addListenPort(int port, ServerConfig *serverConfig) {
     serverConfig->listen_ports.push_back(port);
     }

void Config::parseServerBlock(std::ifstream& file) {
    ServerConfig serverConfig;
    static int i = 3;
    std::string line;

    while (std::getline(file, line)) {
        trim(line);


        if (line == "}")
        {
            i++;
            break;
        }
        else if (line.rfind("listen", 0) == 0 && line.size() > 6) {
            addListenPort(std::stoi(line.substr(7)), &serverConfig);
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
        else if (line.find("max_body") == 0){
            serverConfig.max_body[i] =  atol(extractValue(line).c_str());
        }
        else if (line.find("location") == 0)
        {
            std::map <std::string, rules> tmp = parseLocationBlock(file, line);
            serverConfig.location_rules.insert(tmp.begin(), tmp.end());
        }
    }
    servers.push_back(serverConfig);
}

std::map<std::string, rules>   Config::parseLocationBlock(std::ifstream& file, std::string name) {
    std::map <std::string, rules>   location_rules;
    std::string line;
    std::string prefix;
    prefix = extractRoot(name);
    location_rules[prefix].prefix = prefix;
    location_rules[prefix].state = 1;
    while (std::getline(file, line))
    {
        trim(line);

        if (line == "}")
            break;
        if (line.find("index") == 0){
            location_rules[prefix].index = extractIndex(line);
        }
        if (line.find("return") == 0) {
            location_rules[prefix].redirect = extractRedirect(line);
        } else if (line.find("root") == 0) {
            location_rules[prefix].root = extractRoot(line);
        } else if (line.find("autoindex") == 0) {
            location_rules[prefix].autoindex = extractAutoIndex(line);
        } else if (line.find("allowed_methods") == 0) {
            location_rules[prefix].allowed_methods = extractMethod(line);
        }
    }

    return (location_rules);
}
std::string Config::extractIndex(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;

    iss >> key >> value; // Récupère les deux premiers tokens de la ligne

    if (key == "index") { // Vérifie si le premier token est "index"
        return value;     // Retourne la valeur associée
    }

    return ""; // Retourne une chaîne vide si "index" n'est pas trouvé
}

std::string Config::extractValue(const std::string& line){
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip max_body and getv alue
    
    return token;
}
int Config::extractMaxBody(const std::string &line)
{
    std::istringstream iss(line);
    std::string token;
    int max_body = 0;
    iss >> token >> token;
    max_body = std::atoi(token.c_str());
    if(max_body <= 0)
        max_body = 10000;
    return(max_body);
}
std::string Config::extractRedirect(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token;
    return token;
}

bool Config::extractAutoIndex(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token;
    if (token == "on")
        return 1;
    else
        return 0;
}

std::vector <std::string> Config::extractMethod(const std::string &line)
{
    std::vector <std::string> node;
    std::istringstream iss(line);
    std::string token;
    std::string method;

    iss >> token; // skip
    while (iss >> method)
    {
        node.push_back(method);
    }   
    return node;
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
    } else
        str.clear();
}

int Config::extractPort(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token >> token; // Skip "listen" and get port
    return static_cast<int>(std::atoi(token.c_str()));
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

const std::vector<ServerConfig>& Config::getServers() const {
    return servers;
}
