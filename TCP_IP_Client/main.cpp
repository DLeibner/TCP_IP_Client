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

  int recvRes = resp.getResp(sock);
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
  }

  Response_FileSize resp2;

  recvRes = resp2.getResp(sock);
  if (recvRes == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  cout << resp2.GetSize() << endl;

  // Read File Data
  Request_ReadFile req3("#summary.sta");

  sendResult = req3.sendReq(sock);
  if (sendResult == SOCKET_ERROR)
  {
    cerr << "Send failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  int fileSize = resp2.GetSize();
  char *respBuff3 = new char[fileSize];
  memset(respBuff3, 0, fileSize);

  Response resp3(respBuff3, fileSize);
  recvRes = resp3.getResp(sock);
  if (recvRes == SOCKET_ERROR)
  {
    cerr << "Recv failed, Err#" << WSAGetLastError() << endl;
    closesocket(sock);
    WSACleanup();
  }

  // Parse file

  delete[] respBuff3;

  // do while loop to send and receive data
  while(true)
  {
    
  }

  return;
}