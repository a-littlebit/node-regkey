#include "RegKey.h"
#include <napi.h>

class RegKeyWrap : public Napi::ObjectWrap<RegKeyWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Env env, HKEY hKey, const std::wstring &path);

  RegKeyWrap(const Napi::CallbackInfo &info);

  // Properties

  Napi::Value IsValid(const Napi::CallbackInfo &info);
  Napi::Value GetPath(const Napi::CallbackInfo &info);
  Napi::Value GetHost(const Napi::CallbackInfo &info);
  Napi::Value GetName(const Napi::CallbackInfo &info);
  void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetLastStatus(const Napi::CallbackInfo &info);
  Napi::Value IsWritable(const Napi::CallbackInfo &info);
  Napi::Value Flush(const Napi::CallbackInfo &info);
  void SetLastStatus(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetLastError(const Napi::CallbackInfo &info);
  Napi::Value Close(const Napi::CallbackInfo &info);
  Napi::Value CopyTree(const Napi::CallbackInfo &info);

  // Value Operations

  Napi::Value GetBinaryValue(const Napi::CallbackInfo &info);
  Napi::Value GetStringValue(const Napi::CallbackInfo &info);
  Napi::Value GetMultiStringValue(const Napi::CallbackInfo &info);
  Napi::Value GetDwordValue(const Napi::CallbackInfo &info);
  Napi::Value GetQwordValue(const Napi::CallbackInfo &info);
  Napi::Value GetValueType(const Napi::CallbackInfo &info);
  Napi::Value HasValue(const Napi::CallbackInfo &info);
  Napi::Value GetValueNames(const Napi::CallbackInfo &info);
  Napi::Value SetBinaryValue(const Napi::CallbackInfo &info);
  Napi::Value SetStringValue(const Napi::CallbackInfo &info);
  Napi::Value SetMultiStringValue(const Napi::CallbackInfo &info);
  Napi::Value SetDwordValue(const Napi::CallbackInfo &info);
  Napi::Value SetQwordValue(const Napi::CallbackInfo &info);
  Napi::Value DeleteValue(const Napi::CallbackInfo &info);

  // Key Operations

  Napi::Value DeleteTree(const Napi::CallbackInfo &info);
  Napi::Value OpenSubKey(const Napi::CallbackInfo &info);
  Napi::Value CreateSubKey(const Napi::CallbackInfo &info);
  Napi::Value DeleteSubKey(const Napi::CallbackInfo &info);
  Napi::Value GetSubKeyNames(const Napi::CallbackInfo &info);
  Napi::Value HasSubKey(const Napi::CallbackInfo &info);

private:
  void _ThrowRegKeyError(const Napi::CallbackInfo &info,
                         const std::string &message,
                         const std::wstring &value = L"");

  static Napi::FunctionReference constructor;

  RegKey _regKey;
  std::wstring _path;
};
