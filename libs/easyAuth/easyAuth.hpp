/*
VERSION 3.0
Made by: Plinkon

Changelog:
V: 2.0
- for version 2 i completely forgot to add a chanelog but basically i changed the DB structure to be only vectors
V: 3.0
- changed loadDatabase to be bool and not void so you can check if it successfully loads or not without throwing a error and stopping
- added getUsername()
- added getPassword()
- added doesUserHaveProperty()
- idk what else tbh a couple more small things
*/

#ifndef EasyAuth_HPP
#define EasyAuth_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

struct Database {
    // credentials[credentialType][accountNumber] credentialType 0 = username, 1 = password
    std::vector<std::vector<std::string>> credentials;
    // properties[propertyIndex][accountNumber][propertyNumber] propertyIndex is the current property of the max number of properties each user can have
    std::vector<std::vector<std::vector<std::string>>> properties;

    void clear() {
        credentials.clear();
        properties.clear();
    }

    // Resize the outer vectors: one for credentials (e.g. username, password)
    // and one for property types.
    void resize(size_t credentialTypes, size_t propertyTypes) {
        credentials.resize(credentialTypes);
        properties.resize(propertyTypes);
    }

    // Adds a new account. Each credential type gets an empty string,
    // and each property type gets an empty vector for that account.
    int addAccount() {
        int accountNumber = credentials.empty() ? 0 : credentials[0].size();
        for (auto &cred : credentials) {
            cred.push_back("");
        }
        for (auto &prop : properties) {
            prop.push_back(std::vector<std::string>());
        }
        return accountNumber;
    }

    // “Delete” an account by marking its entries as empty.
    // (Erasing from a vector would shift indices and change account numbers.)
    void deleteAccount(int accountNumber) {
        if (credentials.empty() || accountNumber < 0 || accountNumber >= credentials[0].size())
            throw std::runtime_error("Account not found");
        // Erase the credentials for the specified account.
        for (auto &cred : credentials) {
            cred.erase(cred.begin() + accountNumber);
        }
        // Erase the properties for the specified account.
        for (auto &prop : properties) {
            prop.erase(prop.begin() + accountNumber);
        }
    }
};

class easyAuth {
 private:
    Database db;
    int numberOfProperties;
 public:
    const std::string XOR_KEY = "YOUR_KEY_HERE";
    easyAuth() = default;
    easyAuth(Database database) { // Option to initialize with an existing database.
        this->db = database;
    }
    ~easyAuth() = default;

    // Initialize with 2 credential types (username and password) and a given number of property types.
    void initialize(int numberOfProperties) {
        if (numberOfProperties < 0) {
            throw std::invalid_argument("Number of properties cannot be negative");
        }
        db.resize(2, numberOfProperties);
        this->numberOfProperties = numberOfProperties;
    }

    /* USERS / AUTH / CREDENTIALS */

    bool checkCredentials(const std::string& username, const std::string& password) {
        if (username.empty() || password.empty()) {
            throw std::invalid_argument("Username and password cannot be empty");
        }
        // Iterate over all accounts (by index)
        for (size_t accountNumber = 0; accountNumber < db.credentials[0].size(); ++accountNumber) {
            if (db.credentials[0][accountNumber] == username) {
                if (db.credentials[1][accountNumber] == password) {
                    return true;
                }
            }
        }
        return false;
    }

    void addCredentials(const std::string& username, const std::string& password) {
        if (username.empty() || password.empty()) {
            throw std::invalid_argument("Username and password cannot be empty");
        }
        // Check if username already exists
        for (const auto &name : db.credentials[0]) {
            if (name == username) {
                throw std::runtime_error("Username already exists");
            }
        }
        // Add a new account and then set its credentials.
        int accountNumber = db.addAccount();
        db.credentials[0][accountNumber] = username;
        db.credentials[1][accountNumber] = password;
    }

    void deleteCredentials(int accountNumber) {
        if (accountNumber < 0 || accountNumber >= db.credentials[0].size() ||
            db.credentials[0][accountNumber].empty())
        {
            throw std::runtime_error("Account not found");
        }
        db.deleteAccount(accountNumber);
    }

    void editCredentials(int accountNumber, const std::string& username, const std::string& password) {
        if (accountNumber < 0 || accountNumber >= db.credentials[0].size() ||
            db.credentials[0][accountNumber].empty())
        {
            throw std::runtime_error("Account not found");
        }
        db.credentials[0][accountNumber] = username;
        db.credentials[1][accountNumber] = password;
    }

    Database getAllUsers() {
        // Returns the entire database.
        if (db.credentials.empty()) {
            throw std::runtime_error("Database is empty");
        }
        return this->db;
    }

    int getAccountNumberOfUser(const std::string& username) {
        if (username.empty()) {
            throw std::invalid_argument("Username cannot be empty");
        }
        for (size_t i = 0; i < db.credentials[0].size(); ++i) {
            if (db.credentials[0][i] == username) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    std::string getPassword(int accountNumber) {
        if (accountNumber < 0 || accountNumber >= db.credentials[0].size() ||
            db.credentials[0][accountNumber].empty())
        {
            throw std::runtime_error("Account not found");
        }
        return db.credentials[1][accountNumber];
    }

    std::string getUsername(int accountNumber) {
        if (accountNumber < 0 || accountNumber >= db.credentials[0].size() ||
            db.credentials[0][accountNumber].empty())
        {
            throw std::runtime_error("Account not found");
        }
        return db.credentials[0][accountNumber];
    }

    /* USER PROPERTIES */

    bool checkProperty(int accountNumber, std::size_t propertyIndex, std::size_t propertyNumber, const std::string& property) {
        if (propertyIndex >= db.properties.size())
            throw std::runtime_error("Property index out of range");
        if (accountNumber >= db.properties[propertyIndex].size())
            throw std::runtime_error("Account not found");
        if (propertyNumber >= db.properties[propertyIndex][accountNumber].size())
            throw std::runtime_error("Property number out of range");

        return db.properties[propertyIndex][accountNumber][propertyNumber] == property;
    }

    void addProperty(int accountNumber, std::size_t propertyIndex, const std::string& property) {
        if (propertyIndex >= db.properties.size())
            throw std::runtime_error("Property index out of range");
        if (accountNumber >= db.properties[propertyIndex].size())
            throw std::runtime_error("Account not found");

        db.properties[propertyIndex][accountNumber].push_back(property);
    }

    void deleteProperty(int accountNumber, std::size_t propertyIndex, std::size_t propertyNumber) {
        if (propertyIndex >= db.properties.size())
            throw std::runtime_error("Property index out of range");
        if (accountNumber >= db.properties[propertyIndex].size())
            throw std::runtime_error("Account not found");
        if (propertyNumber >= db.properties[propertyIndex][accountNumber].size())
            throw std::runtime_error("Property number out of range");

        db.properties[propertyIndex][accountNumber].erase(db.properties[propertyIndex][accountNumber].begin() + propertyNumber);
    }

    void editProperty(int accountNumber, std::size_t propertyIndex, std::size_t propertyNumber, const std::string& newProperty) {
        if (propertyIndex >= db.properties.size())
            throw std::runtime_error("Property index out of range");
        if (accountNumber >= db.properties[propertyIndex].size())
            throw std::runtime_error("Account not found");
        if (propertyNumber >= db.properties[propertyIndex][accountNumber].size())
            throw std::runtime_error("Property number out of range");

        db.properties[propertyIndex][accountNumber][propertyNumber] = newProperty;
    }

    std::vector<std::vector<std::string>> getProperties(int accountNumber) {
        std::vector<std::vector<std::string>> userProperties;
        bool foundProperties = false;
        for (size_t propIndex = 0; propIndex < db.properties.size(); ++propIndex) {
            if (accountNumber < db.properties[propIndex].size()) {
                userProperties.push_back(db.properties[propIndex][accountNumber]);
                if (!db.properties[propIndex][accountNumber].empty())
                    foundProperties = true;
            } else {
                userProperties.push_back(std::vector<std::string>());
            }
        }
        if (!foundProperties)
            userProperties.clear();
        return userProperties;
    }

    std::size_t getPropertyNumber(int accountNumber, std::size_t propertyIndex, const std::string& property) {
        if (property.empty()) {
            throw std::invalid_argument("Property cannot be empty");
        }
        if (propertyIndex >= db.properties.size())
            throw std::runtime_error("Property index out of range");
        if (accountNumber >= db.properties[propertyIndex].size())
            throw std::runtime_error("Account not found");

        const auto &props = db.properties[propertyIndex][accountNumber];
        for (std::size_t i = 0; i < props.size(); i++) {
            if (props[i] == property) {
                return i;
            }
        }
        return static_cast<std::size_t>(-1);
    }

    std::size_t getPropertyIndex(int accountNumber, const std::string& property) {
        if (property.empty()) {
            throw std::invalid_argument("Property cannot be empty");
        }
        for (std::size_t propIndex = 0; propIndex < db.properties.size(); propIndex++) {
            if (accountNumber < db.properties[propIndex].size()) {
                const auto &props = db.properties[propIndex][accountNumber];
                if (!props.empty() && props.back() == property) {
                    return propIndex;
                }
            }
        }
        return static_cast<std::size_t>(-1);
    }

    std::size_t getPropertyIndexFromPropertyNumber(int accountNumber, std::size_t propertyNumber) {
        for (std::size_t propIndex = 0; propIndex < db.properties.size(); propIndex++) {
            if (accountNumber < db.properties[propIndex].size()) {
                const auto &props = db.properties[propIndex][accountNumber];
                if (propertyNumber < props.size()) {
                    return propIndex;
                }
            }
        }
        return static_cast<std::size_t>(-1);
    }

    int getMaxNumberOfProperties() {
        return this->numberOfProperties;
    }

    bool doesAccountHaveProperty(int accountNumber, std::string property) {
        return getPropertyIndex(accountNumber, property) != static_cast<std::size_t>(-1);
    }

    /* SAVING / LOADING / ENCRYPTING / DECRYPTING DATABASE */

    void saveDatabase(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open file for writing: " + filename);
        }

        // Save credentials
        size_t credentialTypes = db.credentials.size();
        file.write(reinterpret_cast<const char*>(&credentialTypes), sizeof(credentialTypes));
        for (const auto &credVector : db.credentials) {
            size_t numAccounts = credVector.size();
            file.write(reinterpret_cast<const char*>(&numAccounts), sizeof(numAccounts));
            for (const auto &cred : credVector) {
                size_t strLen = cred.length();
                file.write(reinterpret_cast<const char*>(&strLen), sizeof(strLen));
                file.write(cred.c_str(), strLen);
            }
        }

        // Save properties
        size_t propertyTypes = db.properties.size();
        file.write(reinterpret_cast<const char*>(&propertyTypes), sizeof(propertyTypes));
        for (const auto &propertyType : db.properties) {
            size_t numAccounts = propertyType.size();
            file.write(reinterpret_cast<const char*>(&numAccounts), sizeof(numAccounts));
            for (const auto &props : propertyType) {
                size_t numProperties = props.size();
                file.write(reinterpret_cast<const char*>(&numProperties), sizeof(numProperties));
                for (const auto &prop : props) {
                    size_t strLen = prop.length();
                    file.write(reinterpret_cast<const char*>(&strLen), sizeof(strLen));
                    file.write(prop.c_str(), strLen);
                }
            }
        }

        file.close();
    }

    bool loadDatabase(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            // cant open file
            return false;
        }

        // Clear existing database.
        db.credentials.clear();
        db.properties.clear();

        // Load credentials
        size_t credentialTypes;
        file.read(reinterpret_cast<char*>(&credentialTypes), sizeof(credentialTypes));
        db.credentials.resize(credentialTypes);
        for (size_t i = 0; i < credentialTypes; i++) {
            size_t numAccounts;
            file.read(reinterpret_cast<char*>(&numAccounts), sizeof(numAccounts));
            db.credentials[i].resize(numAccounts);
            for (size_t j = 0; j < numAccounts; j++) {
                size_t strLen;
                file.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));
                std::string cred(strLen, '\0');
                file.read(&cred[0], strLen);
                db.credentials[i][j] = cred;
            }
        }

        // Load properties
        size_t propertyTypes;
        file.read(reinterpret_cast<char*>(&propertyTypes), sizeof(propertyTypes));
        db.properties.resize(propertyTypes);
        for (size_t i = 0; i < propertyTypes; i++) {
            size_t numAccounts;
            file.read(reinterpret_cast<char*>(&numAccounts), sizeof(numAccounts));
            db.properties[i].resize(numAccounts);
            for (size_t j = 0; j < numAccounts; j++) {
                size_t numProperties;
                file.read(reinterpret_cast<char*>(&numProperties), sizeof(numProperties));
                db.properties[i][j].resize(numProperties);
                for (size_t k = 0; k < numProperties; k++) {
                    size_t strLen;
                    file.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));
                    std::string prop(strLen, '\0');
                    file.read(&prop[0], strLen);
                    db.properties[i][j][k] = prop;
                }
            }
        }

        file.close();
        return true;
    }

    void encryptDatabase() {
        if (db.credentials.empty() && db.properties.empty()) {
            throw std::runtime_error("Cannot encrypt empty database");
        }
        std::string key = XOR_KEY.empty() ? "XOR_KEY_HERE" : XOR_KEY;
        size_t keyIndex = 0;

        // Encrypt credentials
        for (auto &credVector : db.credentials) {
            for (auto &cred : credVector) {
                for (char &c : cred) {
                    c ^= key[keyIndex];
                    keyIndex = (keyIndex + 1) % key.length();
                }
            }
        }

        // Encrypt properties
        for (auto &propertyType : db.properties) {
            for (auto &accountProps : propertyType) {
                for (auto &prop : accountProps) {
                    for (char &c : prop) {
                        c ^= key[keyIndex];
                        keyIndex = (keyIndex + 1) % key.length();
                    }
                }
            }
        }
    }

    void decryptDatabase() {
        // XOR decryption is identical to encryption.
        encryptDatabase();
    }
};

#endif // EasyAuth_HPP
