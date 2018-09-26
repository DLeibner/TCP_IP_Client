#pragma once


#pragma pack(push, 1)

struct GanterHeader
{
  // BinarySize: size of payload (outside of header and tail)
  GanterHeader(const _int16 BinarySize, const _int8 command)
  {
    LengthOfFrame = _byteswap_ushort(9 + BinarySize);
    Command = command;
    OffsetWrite = 0;
    LengthWrite = _byteswap_ushort(BinarySize);
  }

  // this must be big-endian!
  _int16 LengthOfFrame;
  _int8  Command;

  // this muse be big-endian!
  _int16 OffsetWrite;

  // this must be big-endian!
  _int16 LengthWrite;
};

struct GantnerTail
{
  GantnerTail(const _int16 ofsRead, const _int16 lenRead)
  {
    OffsetRead = _byteswap_ushort(ofsRead);
    LengthRead = _byteswap_ushort(lenRead);
  }

  // this must be big-endian!
  _int16 OffsetRead = 0;

  // this must be big-endian!
  _int16 LengthRead = 0;
};

struct Request
{
  Request(const _int16 BinarySize, const _int8 command, const _int16 ofsRead, const _int16 lenRead, const char* pBinary = nullptr):
    m_Header(BinarySize, command),
    m_Tail(ofsRead, lenRead),
    m_BinarySize(BinarySize),
    m_Binary(pBinary)
  {}

  GanterHeader m_Header;
  const char* m_Binary = nullptr;
  GantnerTail m_Tail;

  __int16 m_BinarySize;

  int sendReq(const SOCKET s) const
  {
    int res = send(s, reinterpret_cast<const char*>(&m_Header), sizeof(GanterHeader), 0);

    if (res < 0)
    {
      return res;
    }

    if (m_BinarySize)
    {
      int res2 = send(s, m_Binary, m_BinarySize, 0);

      if (res2 < 0)
      {
        return res2;
      }

      res += res2;
    }

    int res3 = send(s, reinterpret_cast<const char*>(&m_Tail), sizeof(GantnerTail), 0);

    if (res3 < 0)
    {
      return res3;
    }

    return res + res3;
  }
};

struct Request_ReadDirectory: public Request
{
  Request_ReadDirectory(const char* driveIndex):
    Request(strlen(driveIndex), 5, 0, 1, driveIndex)
  { }
};

struct Request_ReadFileSize : public Request
{
  Request_ReadFileSize(const char* fileName) :
    Request(strlen(fileName), 5, 1, 0, fileName)
  { }
};

struct Request_ReadFile : public Request
{
  Request_ReadFile(const char* fileName) :
    Request(strlen(fileName), 5, 1, 1, fileName)
  { }
};

struct Response
{
  Response(char *buff = nullptr, unsigned int BuffSize = 0)
  {
    m_RecvBuffer = buff;
    m_RecvBuffSize = BuffSize;
  }

  unsigned _int16 LengthOfFrame = 0;
  unsigned _int32 ExtLengthOfFrame = 0;
  _int8 ReturnState = 0;

  unsigned int m_Length = 0;
  
  char* m_RecvBuffer;
  int m_RecvBuffSize;
  int m_RecvBuffUsed = 0;

  int getResp(const SOCKET s)
  {
    int res = recv(s, reinterpret_cast<char*>(&LengthOfFrame), 2, 0);

    if (res < 0)
    {
      return res;
    }

    if (LengthOfFrame == 0xFFFF)
    {
      // read extended length
      int resX = recv(s, reinterpret_cast<char*>(&ExtLengthOfFrame), 4, 0);

      if (resX < 0)
      {
        return resX;
      }

      res += resX;

      // get the real length
      m_Length = _byteswap_ulong(ExtLengthOfFrame);
    }
    else
    {
      m_Length = _byteswap_ushort(LengthOfFrame);
    }

    int res2 = recv(s, reinterpret_cast<char*>(&ReturnState), 1, 0);

    if (res2 < 0)
    {
      return res2;
    }

    int res3 = 0;
    
    if (m_RecvBuffSize > 0)
    {
      res3 = recv(s, m_RecvBuffer, m_RecvBuffSize, 0);

      if (res3 < 0)
      {
        return res3;
      }
    }

    m_RecvBuffUsed = res3;
    return res + res2 + res3;
  }
};

struct Response_FileSize : public Response
{
  Response_FileSize() :
    Response(m_RespBuff, sizeof(m_RespBuff))
  {
    memset(m_RespBuff, 0, sizeof(m_RespBuff));
  }

  int GetSize() const
  {
    return atoi(m_RespBuff);
  }

  char m_RespBuff[20];
};

#pragma pack(pop)
