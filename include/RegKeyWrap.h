#include "RegKey.h"
#include <napi.h>
#include <memory>

class RegKeyWrap : public Napi::ObjectWrap<RegKeyWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Env env, HKEY hKey, const std::string &path);

  RegKeyWrap(const Napi::CallbackInfo &info);

  /// Basic Attributes

  Napi::Value isOpen(const Napi::CallbackInfo &info);

  Napi::Value getPath(const Napi::CallbackInfo &info);

  Napi::Value getName(const Napi::CallbackInfo &info);

  void setName(const Napi::CallbackInfo &info, const Napi::Value &value);

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
  static void _throwRegKeyError(const Napi::CallbackInfo &info, const std::string &message, const std::string &value);

  std::shared_ptr<RegKey> _regKey;
  std::string _path;

  static Napi::FunctionReference constructor;
};
