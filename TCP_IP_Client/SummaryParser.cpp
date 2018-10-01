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

    if (_GetLeftSide(lineData) == "Type")
    {
      General.Type = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "IsBigEndian")
    {
      General.IsBigEndian = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "AppName")
    {
      General.AppName = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "AppVersion")
    {
      General.AppVersion = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "Vendor")
    {
      General.Vendor = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "SampleRate")
    {
      General.SampleRate = stod(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "VariablesCount")
    {
      General.VariablesCount = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "SlavesCount")
    {
      General.SlavesCount = stoi(_GetRightSide(lineData));
    }
    else if (lineData == "")
    {
      return;
    }
  }
}

void GantnerData::_GetSlavesData(const char*& line)
{
  int currentSlaveCount = 1;
  std::shared_ptr<Slave> currentSlave (new Slave);
  bool slaveVariableDataExists = false;

  while (_GetLineFromFile(line))
  {
    std::string lineData(line);
    lineData = lineData.substr(0, lineData.find("\r\n"));

    if (_GetLeftSide(lineData) == "UartIndex")
    {
      currentSlave->UartIndex = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "Address")
    {
      currentSlave->Address = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "VariablesCount")
    {
      currentSlave->VariablesCount = stoi(_GetRightSide(lineData));

      if (currentSlave->VariablesCount > 0)
      {
        slaveVariableDataExists = true;
      }
    }
    else if (lineData == "")
    {
      if (slaveVariableDataExists)
      {
        slaveVariableDataExists = false;

        _GetSlaveVariableData(line, *currentSlave);
      }

      Slaves.push_back(currentSlave);

      currentSlaveCount++;
      if (currentSlaveCount > General.SlavesCount)
      {
        return;
      }

      currentSlave.reset(new Slave);
    }
  }
}

void GantnerData::_GetSlaveVariableData(const char*& line, Slave& slave)
{
  int currentVariableCount = 1;
  std::shared_ptr<SlaveVariable> currentSlaveVariable(new SlaveVariable);

  while (_GetLineFromFile(line))
  {
    std::string lineData(line);
    lineData = lineData.substr(0, lineData.find("\r\n"));

    if (_GetLeftSide(lineData) == "Name")
    {
      currentSlaveVariable->Name = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "VarType")
    {
      currentSlaveVariable->VarType = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "DataDirection")
    {
      currentSlaveVariable->DataDirection = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "DataType")
    {
      currentSlaveVariable->DataType = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "RangeMin")
    {
      currentSlaveVariable->RangeMin = stod(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "RangeMax")
    {
      currentSlaveVariable->RangeMax = stod(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "Unit")
    {
      currentSlaveVariable->Unit = _GetRightSide(lineData);
    }
    else if (_GetLeftSide(lineData) == "AccessIndex")
    {
      currentSlaveVariable->AccessIndex = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "InpSplitDataFieldOffs")
    {
      currentSlaveVariable->InpSplitDataFieldOffs = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "InpCombDataFieldOffs")
    {
      currentSlaveVariable->InpCombDataFieldOffs = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "OutSplitDataFieldOffs")
    {
      currentSlaveVariable->OutSplitDataFieldOffs = stoi(_GetRightSide(lineData));
    }
    else if (_GetLeftSide(lineData) == "OutCombDataFieldOffs")
    {
      currentSlaveVariable->OutCombDataFieldOffs = stoi(_GetRightSide(lineData));
    }
    else if (lineData == "")
    {
      slave.slaveVariables.push_back(currentSlaveVariable);

      currentVariableCount++;
      if (currentVariableCount > slave.VariablesCount)
      {
        return;
      }

      currentSlaveVariable.reset(new SlaveVariable);
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
