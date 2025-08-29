#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 8

#include <string.h>

int main(int argc, char *argv[])
{
    // Ignore the errors.
    // clang doesn't parse the types in windows headers very well

    // Initialize and start the socket

    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Create socket

    SOCKET listenSocket =
        socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (listenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Listen for connections
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(listenSocket);

    char buffer[DEFAULT_BUFLEN + 1];

    int bytesRead;

    int maxRequestSize = 7000;
    char *request = malloc(maxRequestSize);
    ZeroMemory(request, maxRequestSize);
    int requestCurLen = 0;

    do {
        buffer[DEFAULT_BUFLEN + 1] = '\0';
        bytesRead = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);

        if ((requestCurLen + bytesRead) < maxRequestSize) {
            // Copy to the request buffer
            strncpy(request + requestCurLen, buffer, bytesRead);
            requestCurLen += bytesRead;
        }

        if (bytesRead > 0) {
            // printf("%s", buffer);
        } else if (bytesRead == 0) {
        } else {
            printf("\nrecv failed: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        if (strncmp(request + requestCurLen - 4, "\r\n\r\n", 4) == 0) {
            printf("End of string\n");
            for (int i = 8; i > 0; i--) {
                printf("%d ", request[requestCurLen - i]);
            }
            break;
        }

    } while (bytesRead > 0);

    request[requestCurLen + 1] = '\0';
    printf("Bytes read : %d\n", requestCurLen);

    // for (int i = 8; i > 0; i--) {
    //     printf("%c", request[requestCurLen - i]);
    // }
    //

    char * response = "HTTP/1.1 200 OK\r\n\r\nHello!";

    send(clientSocket,response,strlen(response),0);

    printf("%s", request);
    free(request);

    // Close the socket and send a FIN
    iResult = shutdown(clientSocket, SD_SEND);

    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
