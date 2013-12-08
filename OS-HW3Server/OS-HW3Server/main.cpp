//
//  main.cpp
//  OS-HW3Server
//
//  Created by James Garcia on 12/6/13.
//  Copyright (c) 2013 James Garcia. All rights reserved.
//


/*
 ** server.c -- a stream socket server demo
 */

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
 
#define PORT "3490"  // the port users will be connecting to
 
#define BACKLOG 10     // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define EMPTY 1
#define RESERVE 2
#define TICKET 3

//using namespace std;
 
void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
 
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void cancel(int ***flight, int flightNumber, int rowNumber, int seatNumber)
{
    flight[flightNumber][rowNumber][seatNumber] = EMPTY;
    std::cout<<flight[flightNumber][rowNumber][seatNumber]<<std::endl;
}
void reserve(int ***flight, int flightNumber, int rowNumber, int seatNumber)
{
    flight[flightNumber][rowNumber][seatNumber] = RESERVE;
    std::cout<<flight[flightNumber][rowNumber][seatNumber]<<std::endl;

}
void ticket(int ***flight, int flightNumber, int rowNumber, int seatNumber)
{
    flight[flightNumber][rowNumber][seatNumber] = TICKET;
    std::cout<<flight[flightNumber][rowNumber][seatNumber]<<std::endl;

}


int main(void)
{
    
    int numberOfFlights;
    int flightNumber;
    int rows;
    int seats;
    int numberOfAgents;
    std::string command;
    int flightDetailsNumber[numberOfFlights];
    
    
    
    
    std::ifstream myfile ("input.txt");
    if (myfile.is_open())
    {
        std::string line;
        //get numberOfFlights
        getline(myfile,line);
        std::stringstream(line) >> numberOfFlights;
        
        std::cout<<numberOfFlights<<std::endl;
        
        
        //get flight seating information
        std::istringstream iss;
        std::stringstream ss;
        
        for(int i = 0; i<numberOfFlights; i++)
        {
            getline(myfile,line);
            iss.str(line);
            iss >> flightNumber;
            iss >> rows;
            iss >> seats;
            
        }

}
    else std::cout << "Unable to open file\n";
    myfile.close();
    /* END READ FILE */
    
    //create flight table
    int flight[numberOfFlights+1][rows+1][seats+1];
    
    for (int j = 1; j<numberOfFlights+1; j++)
    {
        for (int k = 1; k<rows+1; k++)
        {
            for(int l = 1; l<seats+1; l++)
            {
                flight[j][k][l]=0;
                std::cout<<"flight: "<<j<<"| rows: "<<k<<"| seats:"<<l<<std::endl;
            }
        }
    }
    
    
    /* SOCKET COMMUNICATION */
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure
    
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    
    printf("server: waiting for connections...\n");
    while(1)
    {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        int numbytes;
        char buf[MAXDATASIZE];
        
        ticket(flight, 8, 15, 3);
        
        if (send(new_fd, "What up fam!", 13, 0) == -1)
            perror("send");
        
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        
        printf("client: received '%s'\n",buf);
        close(new_fd);  // parent doesn't need this
    }
    /* SOCKET COMMUNICATION END*/
    return 0;
}

