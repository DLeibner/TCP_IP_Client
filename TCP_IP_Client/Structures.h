#pragma once
struct Request
{
  _int16 LenghtOfFrame = 0;
  _int8 Command = 0;
  _int16 OffsetWrite = 0;
  _int16 LengthWrite = 0;
   // DataWrite - LengthWrite Bytes
  const char *DataWrite;
  _int16 OffsetRead = 0;
  _int16 LengthRead = 0;
};

struct Response
{
  _int16 LengthOfFrame = 0;
  _int8 ReturnState = 0;
  //_int32 GeneralStates = 0;
  // Data Read - LengthRead Bytes
  const char *DataRead;
};