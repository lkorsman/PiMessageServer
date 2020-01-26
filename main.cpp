//
//  main.cpp
//  PiMessage
//
//  Created by Luke Korsman on 1/23/20.
//  Copyright © 2020 Luke Korsman. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// Prints error messages to console
void error(const char *msg);

// Receives a message from a client
void getMessage(int clientSocket);

// Sends message back to client
void sendResponse(int clientSocket);

int main(int argc, const char * argv[]) {
	const int portNumber = 8866;
	int listenSocket;
	int clientSpecificSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_un clientAddr;
	socklen_t clientAddrSize;
	
	std::cout << "Starting program..." << std::endl;
	
	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portNumber);
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (bind(listenSocket, (struct sockaddr *) &serverAddr,
				sizeof(serverAddr)) < 0) 
		error("Error on binding");
	
	listen(listenSocket, 1);
	
	// Listen for new clients
	while (true) {
		clientAddrSize = sizeof(struct sockaddr_un);
		clientSpecificSocket = accept(listenSocket, 
		(struct sockaddr *) &clientAddr, &clientAddrSize);
		
		if (clientSpecificSocket < 0)
			fprintf(stderr, "Error on accept call.");
		else
			getMessage(clientSpecificSocket);
		close(clientSpecificSocket);
	}
	close(listenSocket);
	
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(-1);
}

void getMessage(int clientSocket)
{
	ssize_t nread;
	const long BUFFER_SIZE = 1000;
	char receiveBuffer[BUFFER_SIZE];

	nread = recv(clientSocket, receiveBuffer, BUFFER_SIZE, 0);
	 
	if (nread == -1) 
	{
		fprintf(stderr, "ERROR reading from socket");
	}
	else
	{
		for (unsigned long i = 0; i < (long)nread; i++) 
		{
			std::cout << receiveBuffer[i];
		}
		std::cout << std::endl;
		sendResponse(clientSocket);
	}
}

void sendResponse(int clientSocket)
{
	std::string message = "You are awesome";
	const int BUFFER_SIZE = 16;
	char responseBuffer[BUFFER_SIZE];
	long bytesLeft = BUFFER_SIZE;
	long bytesSent = 0;
	
	strcpy(responseBuffer, message.c_str());
	
	while (bytesLeft > 0) 
	{
		bytesSent = send(clientSocket, responseBuffer, BUFFER_SIZE, 0);
		if (bytesSent < 0) 
		{
			fprintf(stderr, "ERROR writing to socket");
			break;
		}
		bytesLeft = bytesLeft - bytesSent;
	}
}
