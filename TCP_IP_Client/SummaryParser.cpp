#include "Variables.h"

#ifndef COUNT_OF
#define COUNT_OF(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

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
    else if (lineData == "[M0]")
    {
      _GetSlavesData(line);
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

  if (*startOfLine == '\r' && startOfLine[1] == '\n')
  {
    startOfLine += 2;

    if (startOfLine - _buff >= _size)
    {
      return false;
    }
  }
  else if (*startOfLine == '\r' || *startOfLine == '\n')
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
    std::string left, right;
    _SplitString(lineData, left, right);

    if (left == "Type")
    {
      General.Type = right;
    }
    else if (left == "IsBigEndian")
    {
      General.IsBigEndian = stoi(right);
    }
    else if (left == "AppName")
    {
      General.AppName = right;
    }
    else if (left == "AppVersion")
    {
      General.AppVersion = right;
    }
    else if (left == "Vendor")
    {
      General.Vendor = right;
    }
    else if (left == "SampleRate")
    {
      General.SampleRate = stod(right);
    }
    else if (left == "VariablesCount")
    {
      General.VariablesCount = stoi(right);
    }
    else if (left == "SlavesCount")
    {
      General.SlavesCount = stoi(right);
    }
    else if (lineData == "")
    {
      return;
    }
  }
}

void GantnerData::_GetSlavesData(const char*& line)
{
  for (int currentSlaveCount = 1; currentSlaveCount <= General.SlavesCount; currentSlaveCount++)
  {
    Slave s;

    for (;;)
    {
      if (!_GetLineFromFile(line))
      {
        return;
      }

      std::string lineData(line);

      // read all data for a slave
      while (!lineData.empty() && _GetLineFromFile(line))
      {
        lineData = line;
        lineData = lineData.substr(0, lineData.find("\r\n"));
        std::string left, right;
        _SplitString(lineData, left, right);

        if (left == "UartIndex")
        {
          s.UartIndex = stoi(right);
        }
        else if (left == "Address")
        {
          s.Address = stoi(right);
        }
        else if (left == "VariablesCount")
        {
          s.VariablesCount = stoi(right);
        }
        else if (lineData == "")
        {
          if (s.VariablesCount > 0)
          {
            _GetSlaveVariableData(line, s);
          }

          break;
        }
      }

      Slaves.push_back(s);
      break;
    }
  }
}

int GantnerData::_GetStringIndex(const std::string& value, const char** table, int items)
{
  for (int i = 0; i < items; i++)
  {
    if (value == table[i])
    {
      return i;
    }
  }

  // not found: error
  return -1;
}

void GantnerData::_GetSlaveVariableData(const char*& line, Slave& slave)
{
  for (int currentVariableCount = 1; currentVariableCount <= slave.VariablesCount; currentVariableCount++)
  {
    SlaveVariable slaveVariable;

    for (;;)
    {
      if (!_GetLineFromFile(line))
      {
        slave.SlaveVariables.push_back(slaveVariable);
        return;
      }

      std::string lineData(line);
      lineData = lineData.substr(0, lineData.find("\r\n"));

      std::string left, right;
      _SplitString(lineData, left, right);

      if (left == "Name")
      {
        slaveVariable.Name = right;
      }
      else if (left == "VarType")
      {
        slaveVariable.VarType = right;
      }
      else if (left == "DataDirection")
      {
        const char* s_Directions[] =
        {
          "I", // In
          "O", // Out
          "IO" // InAndOut
        };

        int Dir = _GetStringIndex(right, s_Directions, COUNT_OF(s_Directions));

        if (Dir < 0)
        {
          // todo: can't parse :(
        }

        slaveVariable.DataDirection = (SlaveVariable::eDataDirection) Dir;
      }
      else if (left == "DataType")
      {
        slaveVariable.DataType = right;
      }
      else if (left == "RangeMin")
      {
        slaveVariable.RangeMin = stod(right);
      }
      else if (left == "RangeMax")
      {
        slaveVariable.RangeMax = stod(right);
      }
      else if (left == "Unit")
      {
        slaveVariable.Unit = right;
      }
      else if (left == "AccessIndex")
      {
        slaveVariable.AccessIndex = stoi(right);
      }
      else if (left == "InpSplitDataFieldOffs")
      {
        slaveVariable.InpSplitDataFieldOffs = stoi(right, nullptr, 16);
      }
      else if (left == "InpCombDataFieldOffs")
      {
        slaveVariable.InpCombDataFieldOffs = stoi(right, nullptr, 16);
      }
      else if (left == "OutSplitDataFieldOffs")
      {
        slaveVariable.OutSplitDataFieldOffs = stoi(right, nullptr, 16);
      }
      else if (left == "OutCombDataFieldOffs")
      {
        slaveVariable.OutCombDataFieldOffs = stoi(right, nullptr, 16);
      }
      else if (lineData == "")
      {
        slave.SlaveVariables.push_back(slaveVariable);
        break;
      }
    }
  }
}

bool GantnerData::_SplitString(const std::string data, std::string& left, std::string& right)
{
  size_t s = data.find('=');

  if (s == std::string::npos)
  {
    return false;
  }

  left = data.substr(0, s);
  right = data.substr(s + 1);
  return true;
}

