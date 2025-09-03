#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <windef.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 8

#include <signal.h>
#include <string.h>


static int stayup = TRUE;

enum RequestType { UNSET, GET, POST };

void signalHandler(int sig)
{
    printf("Caught signal %d\n", sig);
    stayup = FALSE;
    exit(0);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, signalHandler);

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

    while (stayup == TRUE) {
        // Listen for connections
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            printf("Listen failed with error: %ld\n", WSAGetLastError());
            stayup = FALSE;
        }

        // Beginning of clientSocket code

        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            stayup = FALSE;
        }

        // closesocket(listenSocket);

        char buffer[DEFAULT_BUFLEN + 1];

        int bytesRead;

        int maxRequestSize = 7000;
        char * request = malloc(maxRequestSize);
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
                stayup = FALSE;
            }

            if (strncmp(request + requestCurLen - 4, "\r\n\r\n", 4) == 0) {
                break;
            }

        } while (bytesRead > 0);

        request[requestCurLen + 1] = '\0';

        // Variables about the request metadata
        enum RequestType requestType = UNSET;
        char pathname[400] = ".";

        char *token = strtok(request, " ");
        int tokenNo = 0;
        while (token != NULL) {
            // Process the stuff based on index.
            switch (tokenNo) {
            case 0:
                if (strncmp(token, "GET", 3) == 0) {
                    requestType = GET;
                }
                break;
            case 1:
                strncat(pathname, token, sizeof(pathname) - 1);
                break;
            defaut:
                break;
            }
            token = strtok(NULL, " ");
            tokenNo++;
        }


        int headerSize = 4000;
        char header[headerSize];

        if (requestType == GET) {

            int validRequest = TRUE;

            // Metadata
            long fileSize = 0;
            char fileType[32] = "text/html"; // Default
            // Determine if it's a valid thing ---------------#

            // Make sure that it contains no ..
            if (strstr(pathname, "..") != NULL) {
                // Return a 404 error
                validRequest = FALSE;
            }

            // Check if the file exists

            FILE *fptr = fopen(pathname, "rb");

            if (fptr != NULL) {
                fseek(fptr, 0, SEEK_END);
                fileSize = ftell(fptr);
                fseek(fptr, 0, SEEK_SET);
                // Also figure out the file type
            } else {
                // Return a 404 error
                validRequest = FALSE;
            }

            // Read the file ---------------------------------#

            char *fileInMem = malloc(fileSize + 10);

            fread(fileInMem, fileSize, 1, fptr);

            strcpy(fileInMem + fileSize, "\r\n\r\n\0");

            fclose(fptr);

            // Construct the header --------------------------#

            snprintf(header, headerSize,
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: %s\r\n"
                     "Content-Length: %ld\r\n"
                     "Connection: keep-alive\r\n"
                     "\r\n",
                     fileType, fileSize);

            // Stream the file -------------------------------#

            send(clientSocket, header, strlen(header), 0);

            send(clientSocket, fileInMem, strlen(fileInMem), 0);

            free(fileInMem);
            free(request);

        }

        // Close the socket and send a FIN
        iResult = shutdown(clientSocket, SD_SEND);

        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed %d\n", WSAGetLastError());
            stayup = FALSE;
        }

        // cleanup
        closesocket(clientSocket);
        // End of client Socket code
    }

    // Probably best to put this in an array of memory thingies and make it so
    // All get freed when they are done
    // Kinda looks like there might be race condition
    // This is for request and fileInMem

    freeaddrinfo(result);

    closesocket(listenSocket);

    WSACleanup();

    return 0;
}
