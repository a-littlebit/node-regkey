#include "RegKey.h"
#include <napi.h>

class RegKeyWrap : public Napi::ObjectWrap<RegKeyWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Env env, HKEY hKey, const std::wstring &path);

  RegKeyWrap(const Napi::CallbackInfo &info);

  /// Basic Attributes

  Napi::Value isOpen(const Napi::CallbackInfo &info);

  Napi::Value getPath(const Napi::CallbackInfo &info);

  Napi::Value getHost(const Napi::CallbackInfo &info);

  Napi::Value getName(const Napi::CallbackInfo &info);

  void setName(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value getLastStatus(const Napi::CallbackInfo &info);

  void setLastStatus(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value getLastError(const Napi::CallbackInfo &info);

  Napi::Value close(const Napi::CallbackInfo &info);

  Napi::Value copy(const Napi::CallbackInfo &info);

  // Value Operations

  Napi::Value getBinaryValue(const Napi::CallbackInfo &info);

  Napi::Value getStringValue(const Napi::CallbackInfo &info);

  Napi::Value getMultiStringValue(const Napi::CallbackInfo &info);

  Napi::Value getDwordValue(const Napi::CallbackInfo &info);

  Napi::Value getQwordValue(const Napi::CallbackInfo &info);

  Napi::Value getValueType(const Napi::CallbackInfo &info);

  Napi::Value hasValue(const Napi::CallbackInfo &info);

  Napi::Value getValueNames(const Napi::CallbackInfo &info);

  Napi::Value setBinaryValue(const Napi::CallbackInfo &info);

  Napi::Value setStringValue(const Napi::CallbackInfo &info);

  Napi::Value setMultiStringValue(const Napi::CallbackInfo &info);

  Napi::Value setDwordValue(const Napi::CallbackInfo &info);

  Napi::Value setQwordValue(const Napi::CallbackInfo &info);

  Napi::Value deleteValue(const Napi::CallbackInfo &info);

  // Key Operations

  Napi::Value deleteKey(const Napi::CallbackInfo &info);

  Napi::Value openSubkey(const Napi::CallbackInfo &info);

  Napi::Value createSubkey(const Napi::CallbackInfo &info);

  Napi::Value deleteSubkey(const Napi::CallbackInfo &info);

  Napi::Value getSubkeyNames(const Napi::CallbackInfo &info);

  Napi::Value hasSubkey(const Napi::CallbackInfo &info);

  Napi::Value isWritable(const Napi::CallbackInfo &info);

private:
  void _throwRegKeyError(const Napi::CallbackInfo &info,
                         const std::string &message,
                         const std::wstring &value = L"");

  RegKey _regKey;
  std::wstring _path;

  static Napi::FunctionReference constructor;
};
