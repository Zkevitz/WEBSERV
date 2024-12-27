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