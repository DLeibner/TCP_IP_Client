#pragma once
#include <string>
#include <list>


struct General
{
  std::string Type;
  int IsBigEndian;
  std::string AppName;
  std::string AppVersion;
  std::string Vendor;
  double SampleRate;
  int VariablesCount;
  int SlavesCount;
};

struct SlaveVariable
{
  std::string Name;
  std::string VarType;
  std::string DataDirection;
  std::string DataType;
  double RangeMin;
  double RangeMax;
  std::string Unit;
  int AccessIndex;
  int InpSplitDataFieldOffs;
  int InpCombDataFieldOffs;
  int OutSplitDataFieldOffs;
  int OutCombDataFieldOffs;
};

struct Slave
{
  int UartIndex;
  int Address;
  int VariablesCount;

  std::list<SlaveVariable> slaveVariables;
};

struct GantnerData
{
  General General;
  std::list<Slave> Slaves;

  bool ParseFile(const char* buff, unsigned int size);

private:

  const char* _buff = nullptr;
  unsigned int _size = 0;

  /// this function updates the line position
  /// to get the first line, set startOfLine to nullptr before calling
  /// when there are no more lines, the function returns false
  bool _GetLineFromFile(const char*& startOfLine);

  void _GetGeneralSectionData(const char*& startOfLine);

  std::string _GetRightSide(std::string data);

  std::string _GetLeftSide(std::string data);
};

