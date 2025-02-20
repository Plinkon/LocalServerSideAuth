#pragma once

// SimpleTCP.hpp
// A header-only library to create a basic TCP server and client using WinSock.
// Only Windows API and STL are used.
// Usage:
//   For the server, include this header, create a SimpleTCP::Server instance, and call start(port, handler).
//     The handler is a function/lambda that takes a request string and returns a response string.
//   For the client, include this header, create a SimpleTCP::Client instance, call connectToServer(address, port),
//     and then call sendRequest() to exchange messages.

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

namespace SimpleTCP {

    // TCP Server class
    class Server {
    public:
        // Define a callback type for processing client requests.
        // The callback takes the request string and returns a response.
        using RequestHandler = std::function<std::string(const std::string&)>;

        Server() : listenSocket(INVALID_SOCKET), running(false) {
            // Initialize WinSock
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                std::cerr << "WSAStartup failed: " << iResult << std::endl;
            }
        }

        ~Server() {
            stop();
            WSACleanup();
        }

        // Starts the server on the given port. The provided handler is invoked for each incoming request.
        bool start(unsigned short port, RequestHandler handler, std::string HOST_IP_ADDRESS) {
            requestHandler = handler;
            listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (listenSocket == INVALID_SOCKET) {
                std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
                return false;
            }

            sockaddr_in service;
            service.sin_family = AF_INET; // IPv4
            // Bind to the specific IP address
            service.sin_addr.s_addr = inet_addr(HOST_IP_ADDRESS.c_str());
            service.sin_port = htons(port);

            if (bind(listenSocket, reinterpret_cast<sockaddr*>(&service), sizeof(service)) == SOCKET_ERROR) {
                std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
                closesocket(listenSocket);
                return false;
            }

            if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
                std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
                closesocket(listenSocket);
                return false;
            }

            running = true;
            acceptThread = std::thread(&Server::acceptLoop, this);
            return true;
        }

        // Stops the server and cleans up connections.
        void stop() {
            running = false;
            if (listenSocket != INVALID_SOCKET) {
                // Shutdown to unblock accept()
                shutdown(listenSocket, SD_BOTH);
                closesocket(listenSocket);
                listenSocket = INVALID_SOCKET;
            }
            if (acceptThread.joinable())
                acceptThread.join();

            // Join all client threads.
            {
                std::lock_guard<std::mutex> lock(clientThreadsMutex);
                for (auto& t : clientThreads) {
                    if (t.joinable())
                        t.join();
                }
                clientThreads.clear();
            }
        }

    private:
        SOCKET listenSocket;
        std::thread acceptThread;
        std::vector<std::thread> clientThreads;
        std::mutex clientThreadsMutex;
        RequestHandler requestHandler;
        std::atomic<bool> running;

        // The accept loop runs in its own thread.
        void acceptLoop() {
            while (running) {
                SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
                if (clientSocket != INVALID_SOCKET) {
                    //std::cout << "Accepted a connection!" << std::endl;
                }
                if (clientSocket == INVALID_SOCKET) {
                    if (running) {  // Only report errors if still running
                        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
                    }
                    break;
                }
                // Spawn a thread to handle each client.
                {
                    std::lock_guard<std::mutex> lock(clientThreadsMutex);
                    clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
                }
            }
        }

        // Handles communication with a single client.
        void handleClient(SOCKET clientSocket) {
            const int bufSize = 512;
            char buffer[bufSize];
            int iResult = 0;

            while ((iResult = recv(clientSocket, buffer, bufSize, 0)) > 0) {
                std::string request(buffer, iResult);
                std::string response;
                if (requestHandler) {
                    response = requestHandler(request);
                }
                // Send back the response.
                int sendResult = send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
                if (sendResult == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
            closesocket(clientSocket);
        }
    };

    // TCP Client class
    class Client {
    public:
        Client() : connectSocket(INVALID_SOCKET) {
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                std::cerr << "WSAStartup failed: " << iResult << std::endl;
            }
        }

        ~Client() {
            if (connectSocket != INVALID_SOCKET) {
                closesocket(connectSocket);
            }
            WSACleanup();
        }

        // Connects to the server at the specified address and port.
        bool connectToServer(const std::string& address, unsigned short port) {
            connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (connectSocket == INVALID_SOCKET) {
                std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
                return false;
            }

            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            // Use inet_pton instead of inet_addr to avoid deprecation warnings.
            if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) != 1) {
                std::cerr << "Invalid IP address: " << address << std::endl;
                closesocket(connectSocket);
                connectSocket = INVALID_SOCKET;
                return false;
            }

            if (::connect(connectSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
                closesocket(connectSocket);
                connectSocket = INVALID_SOCKET;
                return false;
            }
            return true;
        }

        // Sends a request to the server and waits for a response.
        std::string sendRequest(const std::string& request) {
            if (connectSocket == INVALID_SOCKET) {
                return "";
            }

            int sendResult = send(connectSocket, request.c_str(), static_cast<int>(request.size()), 0);
            if (sendResult == SOCKET_ERROR) {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                return "";
            }

            const int bufSize = 512;
            char buffer[bufSize];
            int iResult = recv(connectSocket, buffer, bufSize, 0);
            if (iResult > 0) {
                return std::string(buffer, iResult);
            }
            return "";
        }

    private:
        SOCKET connectSocket;
    };

} // namespace SimpleTCP
