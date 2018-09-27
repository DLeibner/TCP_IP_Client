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

  // unsigned int RecvTimeout = 0;
  // setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)& RecvTimeout, sizeof(RecvTimeout));

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

  u_long NonBlocking = 1;
  ioctlsocket(sock, FIONBIO, &NonBlocking);

  // Read Directory
  Request_ReadDirectory req("-1");

  int sendResult = req.sendReq(sock);
  if (sendResult == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  char respBuff[60000] = {0};
  Response resp(respBuff, sizeof(respBuff));

  int recvRes = resp.getResp(sock, 500);
  if (recvRes == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  // Read File Size
  Request_ReadFileSize req2("#summary.sta");

  sendResult = req2.sendReq(sock);
  if (sendResult == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
    return;
  }

  Response_FileSize resp2;

  recvRes = resp2.getResp(sock, 500);
  if (recvRes == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  int fileSize = resp2.GetSize();

  cout << fileSize << endl;

  if (fileSize <= 0)
  {
    cout << "Size = 0, exiting" << endl;
    closesocket(sock);
    WSACleanup();
    return;
  }

  // Read File Data
  Request_ReadFile req3("#summary.sta");

  sendResult = req3.sendReq(sock);
  if (sendResult == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
    return;
  }

  char *respBuff3 = new char[fileSize];
  memset(respBuff3, 0, fileSize);

  Response resp3(respBuff3, fileSize, true);
  recvRes = resp3.getResp(sock, 1000);
  if (recvRes == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
    return;
  }

  // Parse file

  delete[] respBuff3;

  // do while loop to send and receive data
  //while(true)
  {
    
  }

  closesocket(sock);
  WSACleanup();

  return;
}