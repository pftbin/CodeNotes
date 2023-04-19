// UDPRecvTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define RECV_LOOP_COUNT		1//重试次数，会累加超时时间
int recv_within_time(int fd, char *buf, size_t buf_n,int flags,struct sockaddr* addr,socklen_t *len,unsigned int sec,unsigned usec)
{
	struct timeval tv;
	fd_set readfds;
	unsigned int n=0;
	for(int i=0;i<RECV_LOOP_COUNT;i++)
	{
		FD_ZERO(&readfds);
		FD_SET(fd,&readfds);
		tv.tv_sec=sec;
		tv.tv_usec=usec;
		select(fd+1,&readfds,NULL,NULL,&tv);
		if(FD_ISSET(fd,&readfds))
		{
			if((n=recvfrom(fd,buf,buf_n,flags,addr,len))>=0)
			{
				return n;
			}
		}
	}
	return -1;
}

int _tmain(int argc, _TCHAR* argv[])
{

	int iResult = 0;

	WSADATA wsaData;

	SOCKET RecvSocket;
	sockaddr_in RecvAddr;

	unsigned short Port = 10888;

	char RecvBuf[1024];
	int BufLen = 1024;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof (SenderAddr);

	//-----------------------------------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		goto exit;
	}
	//-----------------------------------------------
	// Create a receiver socket to receive datagrams
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (RecvSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error %d\n", WSAGetLastError());
		goto exit;
	}


/*
	//-----------------------------------------------
	int imode = 0;//阻塞
	if (ioctlsocket(RecvSocket, FIONBIO, (u_long *)&imode) == SOCKET_ERROR)
	{
		wprintf(L"setsockopt failed with error %d\n", WSAGetLastError());
		goto exit;
	}

	//-----------------------------------------------
	struct timeval tv_out;
	tv_out.tv_sec = 10;//接收等待超时时间
	tv_out.tv_usec = 0;
	if(0 != setsockopt(RecvSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)))
	{
		wprintf(L"setsockopt failed with error %d\n", WSAGetLastError());
		goto exit;
	}
*/


	//-----------------------------------------------
	// Bind the socket to any address and the specified port.
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(RecvSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
	if (iResult != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		goto exit;
	}
	//-----------------------------------------------
	// Call the recvfrom function to receive datagrams
	// on the bound socket.
	wprintf(L"Receiving datagrams...\n");


	int nS = GetTickCount();
//	iResult = recvfrom(RecvSocket,RecvBuf, BufLen, 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
	iResult = recv_within_time(RecvSocket,RecvBuf, BufLen, 0, (SOCKADDR *) & SenderAddr, &SenderAddrSize,5,0);

	if (iResult == SOCKET_ERROR) {
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
	}
	int nE = GetTickCount();
	wprintf(L"recvfrom speed %d ms\n",nE-nS);

	//-----------------------------------------------
	// Close the socket when finished receiving datagrams
	wprintf(L"Finished receiving. Closing socket.\n");
	iResult = closesocket(RecvSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
		goto exit;
	}


exit:
	//-----------------------------------------------
	// Clean up and exit.
	wprintf(L"Exiting.\n");
	WSACleanup();


	system("pause");
	return 0;
}