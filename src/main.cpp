#include "../include/Webserv.hpp"
#include "../include/Config.hpp"
#include "../include/Server.hpp"
#include "../include/Msg.hpp"
#include <iostream>
#include <vector>
#include <csignal>

bool running = true;
ssize_t max_body;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        Msg::logMsg(RED, CONSOLE_OUTPUT, "Arrêt du serveur...");
        running = false;
    }
    if (signal == SIGPIPE) {
        Msg::logMsg(RED, CONSOLE_OUTPUT, "SIGPIPE detected !!!");
    }
}

void    print_location(std::vector<ServerConfig> servers)
{
    std::string prefix = "/";
    for(int i = 0; i < 1; i++)
    {
        //printf("prefix = %s\n", servers[i].location_rules[i].prefix.c_str());
       // printf("redirect = %s\n", servers[i].location_rules[prefix].redirect.c_str());
        //printf("root = %s\n", servers[i].location_rules[i].root.c_str());
       // printf("prefix = %s\n", servers[i].location_rules[prefix].prefix.c_str());
       // printf("method 1 = %s\n", servers[i].location_rules[prefix].allowed_methods[0].c_str());
       // printf("method 2 = %s\n", servers[i].location_rules[prefix].allowed_methods[1].c_str());
        //printf("method 3 = %s\n", servers[i].location_rules[prefix].allowed_methods[2].c_str());
        printf("autoindex CONFIRMER ? = %d\n", servers[i].location_rules["cgi-bin/"].autoindex);
    }
}
int main(int argc, char** argv) {

    (void)argc;
    Config config;
    std::signal(SIGINT, signalHandler);
    signal(SIGPIPE, signalHandler);

    if (!config.parseConfigFile(argv[1] ? argv[1] : "config/server.conf")) {
        Msg::logMsg(RED, CONSOLE_OUTPUT, "Config File error !");
        return 1;
    }

    const std::vector<ServerConfig>& servers = config.getServers();
    Server server;
    print_location(servers);
    for (size_t i = 0; i < servers.size(); ++i) {
        const ServerConfig& serverConfig = servers[i];

        server.add_serv(serverConfig);
        if (!server.setup()) {
            Msg::logMsg(RED, CONSOLE_OUTPUT, "Error: Failed to set up server on %d : %d", serverConfig.hostname.c_str(), serverConfig.port);
            return 1;
        }
        Msg::logMsg(LIGHT_BLUE, CONSOLE_OUTPUT, "Server %d : %d is starting...", serverConfig.hostname.c_str(), serverConfig.port);
    }
    server.start();

    return 0;
}