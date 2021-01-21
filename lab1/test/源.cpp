#include <cstdio>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <winsock2.h>
#include "mapType.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
using namespace std;
namespace fs = experimental::filesystem;

int nRC;
int main(int argv,char **argc) {
	
	if (argv < 4)
	{
		cout << "��������" << endl;
		return 0;
	}
	char *address = argc[1];
	char *portChar = argc[2];
	string str(portChar);
	int port = stoi(portChar);
	string path(argc[3]);
	
	//string s1(".jpg");
	//cout << fileTypes[s1] << endl;
	/*
	const char *address = "127.0.0.1";
	const char *portChar = "1234";
	string str(portChar);
	int port = stoi(portChar);
	string path("D:/file");*/
	WSADATA wsaData;
	nRC = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRC)
	{
		printf("Server initialize winsock error!\n");
		return 1;
	}
	SOCKET srvSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srvSock == INVALID_SOCKET)
	{
		printf("Server create socket error! the error code is: %ld\n",WSAGetLastError());
		WSACleanup();
		return 1;
	}
	sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	nRC = inet_pton(AF_INET, address, &seraddr.sin_addr);  //�����ʮ���ƴ�ת���������ֽ��������ֵ
	seraddr.sin_port = htons(port);  //ת��Ϊ�����ֽ�˳��
	if (nRC != 1)
	{
		printf("The IP is wrong!\n");
		closesocket(srvSock);
		WSACleanup();
		return 1;
	}
	nRC = bind(srvSock, (sockaddr*)&seraddr, sizeof(sockaddr));
	if (nRC != NO_ERROR)
	{
		printf("the bind function worked wrongly! Ther error code is: %ld\n", WSAGetLastError());
		closesocket(srvSock);
		WSACleanup();
		return 1;
	}
	listen(srvSock, 10); //��������

	while (true) {
		sockaddr cliAddr;
		int nSize = sizeof(sockaddr);
		SOCKET cliSock = accept(srvSock, (sockaddr*)&cliAddr, &nSize);
		if (cliSock == INVALID_SOCKET)
		{
			printf("Server accept connection request error! The error code is: %ld\n",WSAGetLastError());
			closesocket(srvSock);
			WSACleanup();
			return 1;

		}
		char cliIP[20];
		inet_ntop(AF_INET, &((sockaddr_in *)&cliAddr)->sin_addr, cliIP, 20);  //ת��Ϊʮ����IP�ַ���
		int cliPort = *(&((sockaddr_in *)&cliAddr)->sin_port);
		cout << "������Դ��IPΪ:" << cliIP << " �˿�Ϊ:" << cliPort << endl;
		char recvbuf[1000];
		memset(recvbuf, '\0', 1000 * sizeof(char));
		nRC = recv(cliSock, recvbuf, 1000, 0);
		if (nRC == 0)
		{
			printf("the client closed! We get empty!\n");
			//closesocket(srvSock);
			closesocket(cliSock);
			continue;
			//WSACleanup();
			//return 1;
		}
		if (nRC == SOCKET_ERROR) {
			printf("�����socket���գ�recv���������󣬴����룺%ld\n", WSAGetLastError());
			closesocket(cliSock);
			closesocket(srvSock);
			WSACleanup();
			return 1;
		}
		stringstream ss(recvbuf);
		string method;
		string URL; 
		ss >> method;
		cout << recvbuf << endl;
		ss >> URL;
		fs::path filepath(path + URL);
		string k = path + URL;
		string responsType;
		if (fs::exists(filepath)) {
			string cc = filepath.extension().string();
			responsType = fileTypes[cc];
			cout << "�ļ���׺:" << filepath.extension().string() << endl;
			if (fileTypes.find(filepath.extension().string()) == fileTypes.end())  responsType = "application/oct-stream";
			else responsType == fileTypes[filepath.extension().string()];
			cout <<"��������Ϊ:"<< responsType<<endl;
			string str("HTTP/1.1 200 OK\r\n");
			int responsFileSize = fs::file_size(filepath);
			str += "Content-Length:" + std::to_string(responsFileSize) + "\r\n";
			str += "Content-Type:" + responsType ;
			if (responsType[0] == 't') str += "; charset=gbk,utf-8";
			str += "\r\n\r\n";
			cout << str << endl;
			ifstream ifs(filepath, ios::binary);
			stringstream buffer;
			buffer << ifs.rdbuf();
			str += buffer.str();
			nRC = send(cliSock, str.c_str(), str.size() + 1, NULL);
			if (nRC == SOCKET_ERROR)
			{
				printf("��������ʧ�ܣ�������Ϊ:%ld\n",WSAGetLastError());
				closesocket(srvSock);
				closesocket(cliSock);
				return 1;
			}
			printf("�Ѿ����Ϳͻ�������Ҫ���ļ���·��Ϊ:%s\n", k.c_str());
			/*
			if (filepath.extension() == ".htm"|| filepath.extension() == ".html") {
				string str("HTTP/1.1 200 OK\r\n");
				int responsFileSize =  fs::file_size(filepath);
				str += "ContentLength" + std::to_string(responsFileSize) + "\r\n";
				string str2("Content-Type: text/html; charset=gbk\r\n\r\n");
				ifstream ifs(filepath);
				stringstream buffer; 
				buffer << ifs.rdbuf();
				str += buffer.str();
				nRC = send(cliSock, str.c_str(), str.size() + 1, NULL);
				if (nRC == SOCKET_ERROR)
				{
					printf("��������ʧ��\n");
				}
				printf("�Ѿ����Ϳͻ�������Ҫ���ļ���·��Ϊ:%s\n", k.c_str());
			}
			else {
				string str("HTTP/1.1 200 OK\r\n\r\n");
				ifstream ifs(filepath, ios::binary);
				ss.str(""); ss << ifs.rdbuf();
				str += ss.str();
				nRC = send(cliSock, str.c_str(), str.size() + 1, NULL);
				if (nRC == SOCKET_ERROR)
				{
					printf("��������ʧ��\n");
				}
				printf("�Ѿ����Ϳͻ�������Ҫ���ļ���·��Ϊ:%s\n", k.c_str());
			}*/
		}
		else {
			cout << "�Ҳ������ļ�" << k << endl;
			const char *str = "HTTP/1.1 404 Not Found\r\nContent-Length:22\r\nContent-Type: text/html; charset=gbk\r\n\r\n<h1>404 Not Found</h1>";
			nRC = send(cliSock, str, strlen(str) + 1, NULL);
			if (nRC == SOCKET_ERROR)
			{
				printf("��������ʧ�ܣ�������Ϊ:%ld\n", WSAGetLastError());
				closesocket(srvSock);
				closesocket(cliSock);
				return 1;
			}
		}
		closesocket(cliSock);
	}

	closesocket(srvSock);
	WSACleanup();
	return 0;
}