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

  Napi::Value isValid(const Napi::CallbackInfo &info);

  Napi::Value getPath(const Napi::CallbackInfo &info);

  Napi::Value close(const Napi::CallbackInfo &info);

  Napi::Value copyTree(const Napi::CallbackInfo &info);

  // Value Operations

  Napi::Value getValue(const Napi::CallbackInfo &info);

  Napi::Value getStringValue(const Napi::CallbackInfo &info);

  Napi::Value getNumberValue(const Napi::CallbackInfo &info);

  Napi::Value getValues(const Napi::CallbackInfo &info);

  Napi::Value getStringValues(const Napi::CallbackInfo &info);

  Napi::Value getNumberValues(const Napi::CallbackInfo &info);

  Napi::Value getValueType(const Napi::CallbackInfo &info);

  Napi::Value hasValue(const Napi::CallbackInfo &info);

  Napi::Value setValue(const Napi::CallbackInfo &info);

  Napi::Value applyValues(const Napi::CallbackInfo &info);

  Napi::Value setStringValue(const Napi::CallbackInfo &info);

  Napi::Value setNumberValue(const Napi::CallbackInfo &info);

  Napi::Value deleteValue(const Napi::CallbackInfo &info);

  Napi::Value getValueNames(const Napi::CallbackInfo &info);

  // Key Operations

  Napi::Value rename(const Napi::CallbackInfo &info);

  Napi::Value deleteKey(const Napi::CallbackInfo &info);

  Napi::Value openSubkey(const Napi::CallbackInfo &info);

  Napi::Value createSubkey(const Napi::CallbackInfo &info);

  Napi::Value deleteSubkey(const Napi::CallbackInfo &info);

  Napi::Value getSubkeyNames(const Napi::CallbackInfo &info);

  Napi::Value hasSubkey(const Napi::CallbackInfo &info);

  Napi::Value isSubkeyWriteable(const Napi::CallbackInfo &info);

private:
  std::shared_ptr<RegKey> _regKey;
  std::string _path;

  static Napi::FunctionReference constructor;
};