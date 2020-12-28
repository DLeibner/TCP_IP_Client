#pragma once
#include <string>
#include <vector>

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
    I = 0,  // Input
    O = 1,  // Output
    IO = 2, // Input/Output
    E       // Empty
  };

  enum eVarType
  {
    AIN = 0,  // Analog in
    AOU = 1,  // Analog out
    DIN = 2,  // Digital in
    DOU = 3,  // Digital out
    SPT = 4,  // Setpoit
    ARI = 5   // Arithmetic
  };

  enum eDataType
  {
    BOOL = 0,
    SINT8 = 1,
    USINT8 = 2,
    SINT16 = 3,
    USINT16 = 4,
    SINT32 = 5,
    USINT32 = 6,
    FLOAT = 7,
    SET8 = 8,
    SET16 = 9,
    SET32 = 10,
    DOUBLE = 11
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

  std::vector<SlaveVariable> SlaveVariables;
};

class GantnerData
{
public:

  General General;
  std::vector<Slave> Slaves;

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

