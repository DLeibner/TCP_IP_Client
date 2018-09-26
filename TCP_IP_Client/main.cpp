#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "Structures.h"

using namespace std;

void main()
{
  string ipAddress = "192.168.45.15"; // from parameterization
  int port = 8001;

  // Initialize WinSock
  WSAData data;
  WORD ver = MAKEWORD(2, 2);
  int wsResult = WSAStartup(ver, &data);
  if (wsResult != 0)
  {
    cerr << "Can't start Winsock, Err #" << wsResult << endl;
    return;
  }

  // Create socket
  SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET)
  {
    cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
    WSACleanup();
    return;
  }

  // Fill in a hint structure
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port);
  inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

  // Connect to server
  int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
  if (connResult == SOCKET_ERROR)
  {
    cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  // get State
  //Request req
  //{
  //  0x0009, // LenghtOfFrame
  //  1, // Command
  //  0, // OffsetWrite
  //  0, // LengthWrite
  //     // DataWrite - LengthWrite Bytes
  //  0, // OffsetRead
  //  0xFFFF
  //};

  //Response resp
  //{
  //  0x000D, // LengthOfFrame
  //  0x00, // Return State
  //};

  // Read Directory
  Request_ReadDirectory req2("-1");

  int sendResult2 = req2.sendReq(sock);
  if (sendResult2 == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  char respBuff[60000] = {0};
  Response resp2(respBuff, sizeof(respBuff));

  int recvRes2 = resp2.getResp(sock);
  if (recvRes2 == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  Request_ReadFileSize req3("#summary.sta");

  sendResult2 = req3.sendReq(sock);
  if (sendResult2 == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  Response_FileSize resp3;

  recvRes2 = resp3.getResp(sock);
  if (recvRes2 == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  cout << resp3.GetSize() << endl;

/*

  int sendResult1 = send(sock, reinterpret_cast<char*>(&req1), sizeof(req1), 0);
  if (sendResult1 == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  Response resp1;
  int recvResult1 = recv(sock, reinterpret_cast<char*>(&resp1), sizeof(resp1), 0);
  if (recvResult1 == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  // ReadFileData
  Request req
  {
    22,
    0x05,
    0x0000,
    18,
    "#summary.sta",
    0x0001,
    0x0001
  };

  Response resp
  {

  };

  int sendResult = send(sock, reinterpret_cast<char*>(&req), sizeof(req), 0);
  if (sendResult == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  cout << "Bytes sent: " << sendResult << endl;

  //int recvResult = recv(sock, reinterpret_cast<char*>(&resp), sizeof(resp), 0);
  char buf[120];
  buf[0] = 0xD;
  int recvResult = recv(sock, buf, 120, 0);
  if (recvResult == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  */

  // do while loop to send and receive data
  while(true)
  {
    
  }

  return;
}

void PrintArray(const char* array)
{
  
}