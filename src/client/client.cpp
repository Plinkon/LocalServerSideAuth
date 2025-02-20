#include "../../include/includes.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define PORT 5816 // set this to the port you want to use IF you're not using the port from a file
#define HOST_IP_ADDRESS "127.0.0.1" // set this to the IP address you want to use

bool has_prefix(const std::string& s, const std::string& prefix) { // function to check if a string has a prefix
    if (s.length() < prefix.length()) {
        return false;
    }
    return s.substr(0, prefix.length()) == prefix;
}

void loadMenu(std::string username, std::string password, SimpleTCP::Client& client) {
    std::cout << "\n\nWelcome, " << username << "!\n\n";
    std::cout << "Status: ";

    std::string properties = client.sendRequest("GET_PROPERTIES " + username);
    if (properties == "NO_PROPERTIES_FOUND") { // if request gives no properties
        std::cout << "No properties found for this user.\n";
    } else {
        std::cout << properties << "\n";
    }

    // actual menu here

    while (true) {
        std::cout << "Options:\n";
        std::cout << "1. option1" << std::endl;
        std::cout << "2. option2" << std::endl;
        std::cout << "3. reset password" << std::endl;
        std::cout << "4. buy premium" << std::endl;
        std::cout << "5. exit" << std::endl << std::endl;

        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Option 1 selected." << std::endl;
        } else if (choice == 2) {
            std::cout << "Option 2 selected." << std::endl;
        } else if (choice == 3) {
            std::cout << "Enter new password: ";
            std::string newPassword;
            std::cin >> newPassword;

            if (newPassword.empty()) {
                std::cout << "\nPassword cannot be empty." << std::endl;
                continue;
            } else if (newPassword == password) {
                std::cout << "\nNew password cannot be the same as the old one." << std::endl;
                continue;
            }

            std::string response = client.sendRequest("RESET_PASSWORD " + username + "|" + newPassword);
            if (response == "PASSWORD_RESET_SUCCESS") {
                std::cout << "Password successfully reset!" << std::endl;
            } else if (response == "INVALID_CREDENTIALS") {
                std::cout << "Invalid credentials." << std::endl;
                continue;
            } else {
                std::cout << "An unknown error occurred." << std::endl;
            }
        } else if (choice == 4 && !has_prefix(client.sendRequest("GET_PROPERTIES " + username), "ADMIN") || has_prefix(client.sendRequest("GET_PROPERTIES " + username), "PREMIUM")) { // make sure you cant get premium if you are already premium or admin
            std::cout << "(imaginary checkout process)" << std::endl;
            std::string response = client.sendRequest("BUY_PREMIUM " + username);
            if (response == "PREMIUM_PURCHASED") {
                std::cout << "Account successfully upgraded to premium!" << std::endl;
            } else if (response == "INVALID_CREDENTIALS") {
                std::cout << "Invalid credentials." << std::endl;
                continue;
            } else if (response == "USER_ALREADY_HAS_PREMIUM") {
                std::cout << "User already has premium." << std::endl;
            } else {
                std::cout << "An unknown error occurred." << std::endl;
            }
        } else if (choice == 5) {
            return;
        } else {
            std::cout << "Invalid choice." << std::endl;
        }
    }

    return;
}

int main() {
    SimpleTCP::Client client;

    int port = PORT;

    if (!client.connectToServer(HOST_IP_ADDRESS, port)) { // connect to server
        std::cerr << "Failed to connect to server." << std::endl;
        std::cout << "Press any key to exit." << std::endl;
        std::cin.clear();
        std::cin.get();
        return 0;
    }

    std::cout << "Client successfully connceted to server..\n\n";

    std::string username, password;
    std::string response;

    while (true) {
        int choice;

        std::cout << 
        R"(
                    ***************************
                    *   _____  ___  ___  __   *
                    *  /__   \/___\/___\/ /   *
                    *    / /\//  ///  // /    *
                    *   / / / \_// \_// /___  *
                    *   \/  \___/\___/\____/  *
                    ***************************
                    - by plinkon

)";

        std::cout << "Would you like to register or login? (1 for register, 2 for login): ";
        std::cin >> choice;

        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice. Please enter 1 for register or 2 for login.\n";
            continue;
        }

        if (choice == 1) { // register
            std::cout << "Enter username: ";
            std::cin >> username;

            std::cout << "Enter password: ";
            std::cin >> password;

            char selection;
            std::cout << "Do you want to register with the credentials: " << username << " " << password << " (y/n): ";
            std::cin >> selection;

            if (selection != 'y' && selection != 'Y' && selection != 'n' && selection != 'N') {   
                std::cout << "Invalid choice. Please enter y for yes or n for no.\n";
                continue;
            }

            if (selection == 'n') {
                continue;
            }

            response = client.sendRequest("REGISTER " + username + "|" + password);
            break;
        } 
        
        else if (choice == 2) { // login
            std::cout << "Enter username: ";
            std::cin >> username;

            std::cout << "Enter password: ";
            std::cin >> password;

            response = client.sendRequest("LOGIN " + username + "|" + password);
            break;
        }
    }

    if (response == "LOGIN_SUCCESS") {
        std::cout << "Login successful!\n";
        loadMenu(username, password, client);
    } 

    else if (response == "REGISTER_SUCCESS") {
        std::cout << "Registration successful!\n";
        loadMenu(username, password, client);
    } 

    else if (response == "ACCOUNT_ALREADY_EXISTS") {
        std::cout << "Account already exists.\n";
    } 

    else if (response == "USERNAME_OR_PASSWORD_INVALID") {
        std::cout << "Invalid username or password.\n";
    } 

    else if (response == "INVALID_REQUEST_FORMAT") {
        std::cout << "Invalid request format.\n";
    } 

    else {
        std::cout << "Login failed: " << response << "\n";
    }

    std::cout << "\n\nPress any button to exit..";
    std::cin.ignore(); // to clear the input buffer
    std::cin.get();

    return 0;
}