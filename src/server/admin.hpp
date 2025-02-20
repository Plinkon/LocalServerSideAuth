#include "../../include/includes.h"

void viewDatabase(easyAuth& auth) { // function to view user credentials and properties in database
    Database users = auth.getAllUsers();
    std::cout << "\nUSERS IN DATABASE:\n";
    std::cout << "(Max number of SET properties: " << users.properties.size() << ")\n\n"; // Log size of properties
    for (int i = 0; i < users.credentials[0].size(); i++) {
        std::cout << i << ". Username: " << users.credentials[0][i] << "\n" << i << ". Password: " << users.credentials[1][i] << "\n";

        if (!users.properties[0][i].empty()) { // only show properties if the account has them
            std::cout << "Properties for user " << i << ":\n";
            for (int j = 0; j < users.properties[0][i].size(); j++) {
                std::cout << "  - Property " << j + 1 << ": " << users.properties[0][i][j] << "\n";
            }
            std::cout << "\n";
        } else {
            std::cout << "No properties found for user " << i << "\n\n";
        }
    }
}

void adminPanel(easyAuth& auth) {
    while (true) {
        std::cout << "---ADMIN PANEL---\n";
        std::cout << "\nOptions: \n\n";

        std::cout << "-CREDENTIALS\n";
        std::cout << "1. Add user\n";
        std::cout << "2. Edit user\n";
        std::cout << "3. Delete user\n";
        std::cout << "4. View users\n\n";

        std::cout << "-PROPERTIES\n";
        std::cout << "5. Add properties\n";
        std::cout << "6. Edit properties\n";
        std::cout << "7. Delete properties\n\n";

        std::cout << "0. Exit\n\n";

        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) { // add user
            std::string username, password;

            std::cout << "Enter username: ";
            std::cin >> username;

            std::cout << "Enter password: ";
            std::cin >> password;

            auth.addCredentials(username, password);
            std::cout << "User added!\n";

            std::cout << "Would you like to add properties for this user? (y/n): ";
            std::string response;
            std::cin >> response;

            if (response == "y" || response == "Y") {
                int numberOfProperties;
                std::cout << "Enter the amount/number of properties you want to add to this user: ";
                std::cin >> numberOfProperties;

                if (numberOfProperties < 0) {
                    std::cout << "Number of properties cannot be negative\n";
                    continue;
                }

                else if (numberOfProperties > auth.getMaxNumberOfProperties()) {
                    std::cout << "Number of properties cannot be greater than the maximum number of properties\n";
                    continue;
                }

                for (int i = 0; i < numberOfProperties; i++) {
                    std::string property;
                    std::cout << "Enter property " << i + 1 << ": ";
                    std::cin >> property;
                    auth.addProperty(auth.getAccountNumberOfUser(username), i, property);
                }

                std::cout << "Properties added!\n";
            }
        }

        else if (choice == 2) { // edit user
            std::string username, password;
            int accountNumber;

            std::cout << "Enter account number: ";
            std::cin >> accountNumber;

            std::cout << "Enter new username: ";
            std::cin >> username;

            std::cout << "Enter new password: ";
            std::cin >> password;

            auth.editCredentials(accountNumber, username, password);
        }

        else if (choice == 3) { // delete user
            viewDatabase(auth);
            int accountNumber;
            std::cout << "Enter account number: ";
            std::cin >> accountNumber;
            auth.deleteCredentials(accountNumber);
        }

        else if (choice == 4) { // view users
            viewDatabase(auth);
        }

        else if (choice == 5) { // add properties
            viewDatabase(auth);
            int accountNumber;
            std::cout << "Enter account number of user to add properties to: ";
            std::cin >> accountNumber;

            if (accountNumber >= auth.getAllUsers().credentials[0].size()) {
                std::cout << "Account number not found!\n";
                continue;
            }

            int numberOfProperties;
            std::cout << "Enter the amount/number of properties you want to add to this user: ";
            std::cin >> numberOfProperties;

            if (numberOfProperties > auth.getMaxNumberOfProperties()) {
                std::cout << "You cannot add more properties than the maximum number of properties each user can have!\n";
                continue;
            }

            for (int i = 0; i < numberOfProperties; i++) {
                std::string property;
                std::cout << "Enter property " << i + 1 << ": ";
                std::cin >> property;
                auth.addProperty(accountNumber, i, property);
            }

            std::cout << "Properties added!\n";
        }

        else if (choice == 6) { // edit properties
            viewDatabase(auth);
            int accountNumber;
            std::cout << "Enter account number of user to edit properties from: ";
            std::cin >> accountNumber;

            if (accountNumber >= auth.getAllUsers().credentials[0].size()) {
                std::cout << "Account number not found!\n";
                continue;
            }

            int propertyNumber;
            std::cout << "Enter property number to edit: ";
            std::cin >> propertyNumber;

            propertyNumber--;

            if (propertyNumber >= auth.getAllUsers().properties[accountNumber].size()) {
                std::cout << "Property number not found!\n";
                continue;
            }

            std::string newProperty;
            std::cout << "Enter new property: ";
            std::cin >> newProperty; // get on human readable index
            std::size_t propertyIndex = auth.getPropertyIndexFromPropertyNumber(accountNumber, propertyNumber);
            if (propertyIndex == static_cast<std::size_t>(-1)) { // if not found
                std::cout << "Property not found!\n";
                continue;
            }
            auth.editProperty(accountNumber, propertyIndex, propertyNumber, newProperty);
            std::cout << "Property edited!\n";
        }

        else if (choice == 7) { // remove properties
            viewDatabase(auth);
            int accountNumber;
            std::cout << "Enter account number of user to remove properties from: ";
            std::cin >> accountNumber;

            if (accountNumber >= auth.getAllUsers().credentials[0].size()) {
                std::cout << "Account number not found!\n";
                continue;
            }

            int propertyNumber;
            std::cout << "Enter property number to remove: ";
            std::cin >> propertyNumber;

            if (propertyNumber >= auth.getAllUsers().properties.size()) {
                std::cout << "Property number not found!\n";
                continue;
            }

            if (propertyNumber >= auth.getAllUsers().properties[propertyNumber].size()) {
                std::cout << "Property number not found!\n";
                continue;
            }

            auth.deleteProperty(accountNumber, auth.getPropertyIndexFromPropertyNumber(accountNumber, propertyNumber), propertyNumber);
        }

        else if (choice == 0) { // exit
            break;
        }
    }
}