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
  int SlavesCount = 0;
};

struct SlaveVariable
{
  enum eDataDirection
  {
    In = 0,
    Out = 1,
    InAndOut = 2
  };

  std::string Name;
  std::string VarType;
  eDataDirection DataDirection;
  std::string DataType;
  double RangeMin = 0;
  double RangeMax = 0;
  std::string Unit;
  int AccessIndex;
  int InpSplitDataFieldOffs = -1;
  int InpCombDataFieldOffs = -1;
  int OutSplitDataFieldOffs = -1;
  int OutCombDataFieldOffs = -1;
};

struct Slave
{
  int UartIndex = 0;
  int Address = 0;
  int VariablesCount = 0;

  std::list<SlaveVariable> slaveVariables;
};

class GantnerData
{
public:

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

  void _GetSlavesData(const char*& line);

  void _GetSlaveVariableData(const char*& line, Slave& slave);

  // divides string to the left part and the right part
  // it's assumed that the string looks like <A>=<B>
  // returns false if the parsing cannot be done
  static bool _SplitString(const std::string data, std::string& left, std::string& right);

  static int _GetStringIndex(const std::string& value, const char** table, int items);
};

