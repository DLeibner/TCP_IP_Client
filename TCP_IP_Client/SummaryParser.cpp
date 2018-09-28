#include "Variables.h"


bool GantnerData::ParseFile(const char* buff, unsigned int size)
{
  _buff = buff;
  _size = size;
  const char* line = nullptr;

  while (_GetLineFromFile(line))
  {
    std::string lineData(line);
    lineData = lineData.substr(0, lineData.find("\r\n"));

    if (lineData == "[GENERAL]")
    {
      _GetGeneralSectionData(line);
    }
  }

  return true;
}

bool GantnerData::_GetLineFromFile(const char*& startOfLine)
{
  if (startOfLine == nullptr)
  {
    startOfLine = _buff;
    return true;
  }

  while (*startOfLine != '\r' && *startOfLine != '\n')
  {
    startOfLine++;

    if (startOfLine - _buff >= _size)
    {
      return false;
    }
  }
  while (*startOfLine == '\r' || *startOfLine == '\n')
  {
    startOfLine++;

    if (startOfLine - _buff >= _size)
    {
      return false;
    }
  }

  return true;
}

void GantnerData::_GetGeneralSectionData(const char*& line)
{
  while(_GetLineFromFile(line))
  {
    std::string lineData(line);
    lineData = lineData.substr(0, lineData.find("\r\n"));

    if (_GetLeftSide(lineData) == "SlavesCount")
    {
      General.SlavesCount = stoi(_GetRightSide(lineData));
      return;
    }
  }
}

std::string GantnerData::_GetRightSide(std::string data)
{
  std::string rightSide = data.substr(data.find_last_of("=") + 1);
  return rightSide;
}

std::string GantnerData::_GetLeftSide(std::string data)
{
  std::string rightSide = data.substr(0, data.find_first_of("="));
  return rightSide;
}
