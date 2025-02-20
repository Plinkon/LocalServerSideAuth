#include "../../include/includes.h"
#include "admin.hpp"
#include <string>

#define USE_PORT_FROM_FILE false // if true, make sure to put a port in the port.txt file
#define PORT 5816 // set this to the port you want to use IF you're not using the port from a file
#define HOST_IP_ADDRESS "127.0.0.1" // set this to the IP address you want to use

bool has_prefix(const std::string& s, const std::string& prefix) { // function to check if a string has a prefix
    if (s.length() < prefix.length()) {
        return false;
    }
    return s.substr(0, prefix.length()) == prefix;
}

void initServer(SimpleTCP::Server& server, easyAuth& auth, std::ofstream& logfile) {
    int port;
    if (USE_PORT_FROM_FILE) {
        std::ifstream ifs("../src/port/port.txt");

        if (!ifs.is_open()) {
            std::cerr << "Could not open file at port.txt\n";
            throw std::runtime_error("Failed to open port.txt");
        }

        if (!(ifs >> port)) {
            std::cerr << "Failed to read an integer from port.txt\n";
            throw std::runtime_error("Failed to extract port from file");
        }
    } else {
        port = PORT;
    }

    std::cout << "Server started on port: " << port << "\n";

    if (!server.start(port, [&auth, &logfile] (std::string request) -> std::string {
        logfile << "Received request: " << request << "\n";
        if (has_prefix(request, "LOGIN ")) {
            // Remove the "LOGIN " prefix (which is 6 characters)
            std::string credentials = request.substr(6);

            // Find the position of the '|' separator
            size_t separatorPos = credentials.find("|");

            if (separatorPos != std::string::npos) {
                // Extract the username (from start up to the separator)
                std::string username = credentials.substr(0, separatorPos);
                // Extract the password (from just after the separator to the end)
                std::string password = credentials.substr(separatorPos + 1);

                // Now username and password contain only the desired parts.

                if (auth.checkCredentials(username, password)) {
                    logfile << "Login successful: " << username << " " + password << "\n\n";
                    return "LOGIN_SUCCESS";
                } else {
                    logfile << "Username or password invalid: " << username << " " + password << "\n\n";
                    return "USERNAME_OR_PASSWORD_INVALID";
                }
            } else { // if separator not found
                logfile << "Invalid request format" << "\n\n";
                return "INVALID_REQUEST_FORMAT";
            }
        }

        if (has_prefix(request, "REGISTER ")) {
            // Remove the "REGISTER " prefix (which is 9 characters)
            std::string credentials = request.substr(9);

            // Find the position of the '|' separator
            size_t separatorPos = credentials.find("|");

            if (separatorPos != std::string::npos) {
                // Extract the username (from start up to the separator)
                std::string username = credentials.substr(0, separatorPos);
                // Extract the password (from just after the separator to the end)
                std::string password = credentials.substr(separatorPos + 1);
                
                if (!auth.checkCredentials(username, password)) {
                    auth.addCredentials(username, password);
                    auth.addProperty(auth.getAccountNumberOfUser(username), 0, "USER");
                    logfile << "Account registered: " << username << " " + password << "\n\n";
                    return "REGISTER_SUCCESS";
                } else {
                    logfile << "Account already exists: " << username << " " + password << "\n\n";
                    return "ACCOUNT_ALREADY_EXISTS";
                }
            } else { // if separator not found
                logfile << "Invalid request format" << "\n\n";
                return "INVALID_REQUEST_FORMAT";
            }
        }

        if (has_prefix(request, "GET_PROPERTIES ")) {
            // properties request is "GET_PROPERTIES " + username
            // Remove the "GET_PROPERTIES " prefix (which is 15 characters)
            std::string username = request.substr(15);

            // get the properties of the user
            std::vector<std::vector<std::string>> properties = auth.getProperties(auth.getAccountNumberOfUser(username));

            if (properties.empty()) {
                logfile << "No properties found for user: " << username << "\n\n";
                return "NO_PROPERTIES_FOUND";
            }

            // return the properties
            std::string propertiesString;
            for (const auto& propVector : properties) {
                for (const auto& prop : propVector) {
                    propertiesString += prop + "|";
                }
            }
            if (!propertiesString.empty()) {
                propertiesString.pop_back(); // remove the trailing '|'
            }
            logfile << "Properties returned: " << propertiesString << "\n\n";
            return propertiesString;
        }

        if (has_prefix(request, "RESET_PASSWORD ")) {
            // reset password request is "RESET_PASSWORD " + username + "|" + new password
            // Remove the "RESET_PASSWORD " prefix (which is 15 characters)
            std::string credentials = request.substr(15);

            // find the position of the '|' separator
            size_t separatorPos = credentials.find("|");

            if (separatorPos != std::string::npos) {
                // Extract the username (from start up to the separator)
                std::string username = credentials.substr(0, separatorPos);
                // Extract the password (from just after the separator to the end)
                std::string newPassword = credentials.substr(separatorPos + 1);

                std::string password = auth.getPassword(auth.getAccountNumberOfUser(username));

                if (auth.checkCredentials(username, password)) {
                    auth.editCredentials(auth.getAccountNumberOfUser(username), username, newPassword);
                    logfile << "Password reset for user: " << username << "\n\n";
                    return "PASSWORD_RESET_SUCCESS";
                } else {
                    logfile << "Invalid credentials" << "\n\n";
                    return "INVALID_CREDENTIALS";
                }
            } else {
                logfile << "Invalid request format" << "\n\n";
                return "INVALID_REQUEST_FORMAT";
            }
        }

        if (has_prefix(request, "BUY_PREMIUM ")) {
            // buy premium request is "BUY_PREMIUM " + username
            // Remove the "BUY_PREMIUM " prefix (which is 12 characters)
            std::string username = request.substr(12);

            if (auth.doesAccountHaveProperty(auth.getAccountNumberOfUser(username), "PREMIUM")) {
                logfile << "User already has premium: " << username << "\n\n";
                return "USER_ALREADY_HAS_PREMIUM";
            }

            auth.editProperty(auth.getAccountNumberOfUser(username), 0, 0, "PREMIUM");
            logfile << "Premium purchased for user: " << username << "\n\n";
            return "PREMIUM_PURCHASED";
        }

        logfile << "Invalid request: " << request << "\n\n";
        return "INVALID_REQUEST"; // if request is not valid
    }, HOST_IP_ADDRESS)) {
        std::cerr << "Failed to start server." << std::endl;
        return;
    }
}

void initDatabase(easyAuth& auth, std::string filename) {
    if (!auth.loadDatabase(filename)) { // cant load database
        return;
    }
    auth.decryptDatabase();
}

void closeDatabase(easyAuth& auth, std::string filename) {
    auth.encryptDatabase();
    auth.saveDatabase(filename);
}

int main() {
    int choice;
    bool running = false;
    bool stopped = false;

    easyAuth auth; // create object
    SimpleTCP::Server server;

    std::ofstream logfile("log.txt", std::ios::app);

    logfile << "\n\n-----NEW SESSION-----\n";

    while (true) {
        std::cout << "---SERVER---\n";
        std::cout << "RUNNING: "; if (running) std::cout << "true\n"; else std::cout << "false\n";
        std::cout << "STOPPED: "; if (stopped) std::cout << "true\n"; else std::cout << "false\n";
        std::cout << "0. Init, start, and goto admin panel\n1. Init and start server\n2. Admin panel\n3. Stop server\n4. Save database and exit\n5. Force exit\nEnter your choice: ";
        std::cin >> choice;
        std::cout << "\n";

        if (choice == 0) {
            if (!running) {
                std::cout << "1. Initializing database..\n";
                auth.initialize(1); // init with 1 special property for each user, being the level of the account
                std::cout << "initialized\n";
                initDatabase(auth, "database.db");
                std::cout << "Database initialized..\n";

                // setup server
                std::cout << "2. Starting server..\n";
                initServer(server, auth, logfile);
                std::cout << "Server started. Waiting for connections\n\n";
                running = true;

                adminPanel(auth);
            }
        }

        if (choice == 1) { // init and start server
            if (!running) {
                std::cout << "1. Initializing database..\n";
                auth.initialize(1); // init with 1 special property for each user, being the level of the account
                std::cout << "initialized\n";
                initDatabase(auth, "database.db");
                std::cout << "Database initialized..\n";

                // setup server
                std::cout << "2. Starting server..\n";
                initServer(server, auth, logfile);
                std::cout << "Server started. Waiting for connections\n\n";
                running = true;
            }
        }

        if (choice == 2) { // admin panel
            adminPanel(auth);
        }

        if (choice == 3) { // stop server
            std::cout << "Are you sure you want to stop the server? (y/n): ";
            std::string confirm;
            std::cin >> confirm;

            if (confirm != "y") {
                std::cout << "Cancelled\n";
                continue;
            }

            server.stop();
            running = false;
            stopped = true;
        }

        if (choice == 4) { // save and exit
            if (stopped && !running) {
                std::cout << "3. Saving database..\n";
                closeDatabase(auth, "database.db");
                logfile.close();
                std::cout << "Database saved. Exiting..\n";
                std::cin.clear();
                std::cin.get();
                return 0;
            }
        }

        if (choice == 5) { // force exit
            server.stop();
            logfile.close();
            std::cin.clear();
            std::cin.get();
            return 0;
        }
    }

    return 0;
}