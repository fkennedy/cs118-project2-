#include <stdio.h>
#include <sys/types.h>  // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "packet.h"

// Return codes
#define RC_SUCCESS  0
#define RC_EXIT     1
#define RC_ERROR    -1

// Constants
#define MAX_SEQ_NO  30720
#define WINDOW_SIZE 5120
#define TIME_OUT    500
#define HEADER_SIZE 20
#define BUFFER_LEN  1024

// Function headers
void error(char* msg);

// Main
int main(int argc, char* argv[]) {
    // Validate args
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <port>\n", argv[0]);
        exit(RC_EXIT);
    }

    // Declare variables
    int sockfd; // socket
    int portno; // port number to listen on
    int opt; // setsockopt flag

    // Server
    struct sockaddr_in serv_addr; // server's address
    int servlen; // byte size of server's address

    // Client
    struct sockaddr_in cli_addr; // client's address
    int clilen; // byte size of client's address
    struct hostent *client; // client host info
    char *hostaddr; // client host address in dotted-decimal notation (IP address)

    // Buffer
    char buffer[BUFFER_LEN]; // buffer
    
    // Packets
    // struct packet packetReceive;
    // struct packet packetSend;
    // int total;
    // int packets;
    // int remainder;

    // File vars
    char* filename; // file name to open
    FILE* fp;
    long filesize;

    // Get port number
    portno = atoi(argv[1]);

    // Create parent socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == RC_ERROR)
        error("ERROR: Could not open socket\n");

    // This is to prevent "Error: Address already in use"
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &opt, sizeof(int));

    // Get server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == RC_ERROR)
        error("ERROR: Could not bind\n");

    printf("Server listening on port %d!\n", portno);

    clilen = sizeof(cli_addr);

    while (1) {
        // Receive packet from the client
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen) == RC_ERROR)
            error("ERROR: Could not receive packet\n");

        // Open the file
        // using rb because we're not only opening text files
        if ((fp = fopen(filename, "rb")) == NULL)
            error("ERROR: Could not open file\n");

        // Get filesize
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // // Determine number of packets (for large files)
        // packets = filesize/PACKET_SIZE;
        // remainder = filesize%PACKET_SIZE;
        // if (remainder == 0)
        //     total = packets;
        // else
        //     total = packets+1;     
    }

    fprintf(stdout, "Connection closed\n");
    fclose(fp);
    close(sockfd);

    return RC_SUCCESS;
}

// Helper functions
void error(char *msg) {
    perror(msg);
    exit(RC_EXIT);
}