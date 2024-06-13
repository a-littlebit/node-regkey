#include "RegKeyWrap.h"

HKEY getBaseKey(const std::string &baseKeyName)
{
    HKEY baseKey = NULL;
    if (baseKeyName == "HKEY_CLASSES_ROOT")
    {
        baseKey = HKEY_CLASSES_ROOT;
    }
    else if (baseKeyName == "HKEY_CURRENT_USER")
    {
        baseKey = HKEY_CURRENT_USER;
    }
    else if (baseKeyName == "HKEY_LOCAL_MACHINE")
    {
        baseKey = HKEY_LOCAL_MACHINE;
    }
    else if (baseKeyName == "HKEY_USERS")
    {
        baseKey = HKEY_USERS;
    }
    else if (baseKeyName == "HKEY_PERFORMANCE_DATA")
    {
        baseKey = HKEY_PERFORMANCE_DATA;
    }
    else if (baseKeyName == "HKEY_PERFORMANCE_NLSTEXT")
    {
        baseKey = HKEY_PERFORMANCE_NLSTEXT;
    }
    else if (baseKeyName == "HKEY_PERFORMANCE_TEXT")
    {
        baseKey = HKEY_PERFORMANCE_TEXT;
    }
    else if (baseKeyName == "HKEY_CURRENT_CONFIG")
    {
        baseKey = HKEY_CURRENT_CONFIG;
    }
    
    return baseKey;
}

DWORD getKeyTypeValue(const std::string &keyType)
{
    if (keyType == "REG_SZ")
    {
        return REG_SZ;
    }
    else if (keyType == "REG_EXPAND_SZ")
    {
        return REG_EXPAND_SZ;
    }
    else if (keyType == "REG_BINARY")
    {
        return REG_BINARY;
    }
    else if (keyType == "REG_DWORD" || keyType == "REG_DWORD_LITTLE_ENDIAN")
    {
            return REG_DWORD;
    }
    else if (keyType == "REG_DWORD_BIG_ENDIAN")
    {
        return REG_DWORD_BIG_ENDIAN;
    }
    else if (keyType == "REG_QWORD" || keyType == "REG_QWORD_LITTLE_ENDIAN")
    {
        return REG_QWORD;
    }
    else if (keyType == "REG_MULTI_SZ")
    {
        return REG_MULTI_SZ;
    }
    else if (keyType == "REG_RESOURCE_LIST")
    {
        return REG_RESOURCE_LIST;
    }
    else if (keyType == "REG_FULL_RESOURCE_DESCRIPTOR")
    {
        return REG_FULL_RESOURCE_DESCRIPTOR;
    }
    else if (keyType == "REG_RESOURCE_REQUIREMENTS_LIST")
    {
        return REG_RESOURCE_REQUIREMENTS_LIST;
    }
    
    return REG_NONE;
}

std::string getKeyTypeName(DWORD keyType)
{
    switch (keyType)
    {
    case REG_SZ:
        return "REG_SZ";
    case REG_EXPAND_SZ:
        return "REG_EXPAND_SZ";
    case REG_BINARY:
        return "REG_BINARY";
    case REG_DWORD:
        return "REG_DWORD";
    case REG_DWORD_BIG_ENDIAN:
        return "REG_DWORD_BIG_ENDIAN";
    case REG_QWORD:
        return "REG_QWORD";
    case REG_MULTI_SZ:
        return "REG_MULTI_SZ";
    case REG_RESOURCE_LIST:
        return "REG_RESOURCE_LIST";
    case REG_FULL_RESOURCE_DESCRIPTOR:
        return "REG_FULL_RESOURCE_DESCRIPTOR";
    case REG_RESOURCE_REQUIREMENTS_LIST:
        return "REG_RESOURCE_REQUIREMENTS_LIST";
    default:
        return "REG_NONE";
    }
}

std::string replace_string(const std::string& str, const std::string &from, const std::string &to)
{
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos)
    {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

Napi::FunctionReference RegKeyWrap::constructor;

Napi::Object RegKeyWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env,
                                      "RegKey",
                                      {
                                          InstanceAccessor("path", &RegKeyWrap::getPath, nullptr),

                                          InstanceMethod("isValid", &RegKeyWrap::isValid),
                                          InstanceMethod("copyTree", &RegKeyWrap::copyTree),
                                          InstanceMethod("close", &RegKeyWrap::close),

                                          InstanceMethod("getValue", &RegKeyWrap::getValue),
                                          InstanceMethod("getValueType", &RegKeyWrap::getValueType),
                                          InstanceMethod("getStringValue", &RegKeyWrap::getStringValue),
                                          InstanceMethod("getNumberValue", &RegKeyWrap::getNumberValue),
                                          InstanceMethod("hasValue", &RegKeyWrap::hasValue),
                                          InstanceMethod("deleteValue", &RegKeyWrap::deleteValue),

                                          InstanceMethod("getValues", &RegKeyWrap::getValues),
                                          InstanceMethod("getStringValues", &RegKeyWrap::getStringValues),
                                          InstanceMethod("getNumberValues", &RegKeyWrap::getNumberValues),
                                          InstanceMethod("getValueNames", &RegKeyWrap::getValueNames),

                                          InstanceMethod("setValue", &RegKeyWrap::setValue),
                                          InstanceMethod("setStringValue", &RegKeyWrap::setStringValue),
                                          InstanceMethod("setNumberValue", &RegKeyWrap::setNumberValue),
                                          InstanceMethod("applyValues", &RegKeyWrap::applyValues),

                                          InstanceMethod("rename", &RegKeyWrap::rename),
                                          InstanceMethod("deleteKey", &RegKeyWrap::deleteKey),
                                          InstanceMethod("openSubkey", &RegKeyWrap::openSubkey),
                                          InstanceMethod("createSubkey", &RegKeyWrap::createSubkey),
                                          InstanceMethod("deleteSubkey", &RegKeyWrap::deleteSubkey),
                                          InstanceMethod("getSubkeyNames", &RegKeyWrap::getSubkeyNames),
                                          InstanceMethod("hasSubkey", &RegKeyWrap::hasSubkey)
                                      });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    // exports.Set("RegKey", func);

    exports.Set("hkcr", NewInstance(env, HKEY_CLASSES_ROOT,        "HKEY_CLASS_ROOT"));
    exports.Set("hkcu", NewInstance(env, HKEY_CURRENT_USER,        "HKEY_CURRENT_USER"));
    exports.Set("hklm", NewInstance(env, HKEY_LOCAL_MACHINE,       "HKEY_LOCAL_MACHINE"));
    exports.Set("hku" , NewInstance(env, HKEY_USERS,               "HKEY_USERS"));
    exports.Set("hkcc", NewInstance(env, HKEY_CURRENT_CONFIG,      "HKEY_CURRENT_CONFIG"));
    exports.Set("hkpd", NewInstance(env, HKEY_PERFORMANCE_DATA,    "HKEY_PERFORMANCE_DATA"));
    exports.Set("hknt", NewInstance(env, HKEY_PERFORMANCE_NLSTEXT, "HKEY_PERFORMANCE_NLSTEXT"));
    exports.Set("hkpn", NewInstance(env, HKEY_PERFORMANCE_NLSTEXT, "HKEY_PERFORMANCE_NLSTEXT"));
    

    return exports;
}

Napi::Object RegKeyWrap::NewInstance(Napi::Env env, HKEY hKey, const std::string &path)
{
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({
        Napi::External<HKEY>::New(env, &hKey),
        Napi::String::New(env, path)
    });
    return scope.Escape(obj).ToObject();
}

RegKeyWrap::RegKeyWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RegKeyWrap>(info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Base Key Expected");
    }

    if (info[0].IsExternal()) {
        Napi::External<HKEY> external = info[0].As<Napi::External<HKEY>>();
        _regKey.reset(new RegKey(HKEY(*external.Data())));
        _path = info[1].As<Napi::String>().Utf8Value();
    } else if (info[0].IsString()) {
        std::string baseKeyName = info[0].As<Napi::String>().Utf8Value();
        HKEY baseKey = getBaseKey(baseKeyName);
        if (baseKey == NULL) {
            throw Napi::TypeError::New(info.Env(), "Invalid Base Key");
        }

        if (info.Length() > 1) {
            if (info[1].IsString()) {
                std::string keyName = info[1].As<Napi::String>().Utf8Value();
                _regKey.reset(new RegKey(baseKey, keyName));
            } else {
                throw Napi::TypeError::New(info.Env(), "Subkey shuold be a String");
            }
        } else {
            _regKey.reset(new RegKey(baseKey));
        }
    } else if (info[0].IsObject()
        && info[0].As<Napi::Object>().InstanceOf(RegKeyWrap::constructor.Value())) {
        RegKeyWrap * pRegKeyWrap = Napi::ObjectWrap<RegKeyWrap>::Unwrap(info[0].As<Napi::Object>());
        _regKey = pRegKeyWrap->_regKey;
    } else {
        throw Napi::TypeError::New(info.Env(), "Invalid Base Key");
    }
}

Napi::Value RegKeyWrap::isValid(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey->isOpen());
}

Napi::Value RegKeyWrap::getPath(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), _path);
}

Napi::Value RegKeyWrap::close(const Napi::CallbackInfo &info)
{
    _regKey->close();
    return info.Env().Undefined();
}

Napi::Value RegKeyWrap::copyTree(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Source Key Expected");
    }

    if (info[0].IsObject()
        && info[0].As<Napi::Object>().InstanceOf(RegKeyWrap::constructor.Value())) {
        RegKeyWrap * pRegKeyWrap = Napi::ObjectWrap<RegKeyWrap>::Unwrap(info[0].As<Napi::Object>());
        return Napi::Boolean::New(info.Env(), _regKey->copyTree(pRegKeyWrap->_regKey->getHandle()));
    } else {
        throw Napi::TypeError::New(info.Env(), "Invalid Source Key");
    }
}

Napi::Value RegKeyWrap::getValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();

        size_t size = _regKey->getValueSize(valueName);
        if (size == 0) {
            return Napi::Buffer<BYTE>::New(info.Env(), 0);
        }

        byte *buffer = new byte[size];
        size = _regKey->getValue(valueName, buffer, size);
        auto res = Napi::Buffer<BYTE>::Copy(info.Env(), buffer, size);
        delete[] buffer;
        return res;
    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::getStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();
        return Napi::String::New(info.Env(), _regKey->getStringValue(valueName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::getNumberValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();
        DWORD type = _regKey->getValueType(valueName);

        bool success = false;
        QWORD val;
        std::string sVal;
        switch (type)
        {
        case REG_DWORD:
            val = _regKey->getDwordValue(valueName, &success);
            if (success) {
                return Napi::Number::New(info.Env(), val);
            }
            // fall through
        case REG_QWORD:
            val = _regKey->getQwordValue(valueName, &success);
            if (success) {
                return Napi::Number::New(info.Env(), val);
            }
            // fall through
        default:
            sVal = _regKey->getStringValue(valueName);
            try {
                return Napi::Number::New(info.Env(), std::stod(sVal));
            } catch (std::invalid_argument &) {
                throw Napi::TypeError::New(info.Env(), "Value is not a Number");
            }
        }

    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::getValues(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey->getValues();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++, i++)
    {
        Napi::Object result = Napi::Object::New(info.Env());
        result.Set("name", Napi::String::New(info.Env(), it->name));
        result.Set("type", Napi::String::New(info.Env(), getKeyTypeName(it->type)));
        result.Set("data", Napi::Buffer<byte>::Copy(info.Env(), it->data.data(), it->data.size()));
        results.Set(i, result);
    }
    return results;
}

Napi::Value RegKeyWrap::getStringValues(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey->getValues();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++)
    {
        if (it->type != REG_SZ && it->type != REG_EXPAND_SZ) {
            continue;
        }

        Napi::Object result = Napi::Object::New(info.Env());
        result.Set("name", Napi::String::New(info.Env(), it->name));
        result.Set("type", Napi::String::New(info.Env(), getKeyTypeName(it->type)));
        result.Set("data", Napi::String::New(info.Env(), reinterpret_cast<char*>(it->data.data())));
        results.Set(i, result);
        i++;
    }
    return results;
}

Napi::Value RegKeyWrap::getNumberValues(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey->getValues();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++, i++)
    {
        Napi::Object result = Napi::Object::New(info.Env());
        result.Set("name", Napi::String::New(info.Env(), it->name));
        result.Set("type", Napi::String::New(info.Env(), getKeyTypeName(it->type)));

        switch (it->type)
        {
        case REG_DWORD:
            result.Set("data", Napi::Number::New(info.Env(), reinterpret_cast<DWORD *>(it->data.data())[0]));
            results.Set(i, result);
            i++;
            break;

        case REG_QWORD:
            result.Set("data", Napi::Number::New(info.Env(), reinterpret_cast<QWORD *>(it->data.data())[0]));
            results.Set(i, result);
            i++;
            break;

        case REG_SZ:
        case REG_EXPAND_SZ:
        {
            std::string sVal = reinterpret_cast<char*>(it->data.data());
            try {
                result.Set("data", Napi::Number::New(info.Env(), std::stod(sVal)));
                results.Set(i, result);
                i++;
            } catch (std::invalid_argument &) {
                continue;
            }
        }
        
        default:
            break;
        }
    }
    return results;
}

Napi::Value RegKeyWrap::getValueType(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();
        return Napi::String::New(info.Env(), getKeyTypeName(_regKey->getValueType(valueName)));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::hasValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(info.Env(), _regKey->hasValue(valueName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::setValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info.Length() < 2 || !info[1].IsBuffer()) {
        throw Napi::TypeError::New(info.Env(), "Buffer Data Expected");
    }

    DWORD type = REG_BINARY;
    if (info.Length() > 2 || !info[2].IsString()) {
        type = getKeyTypeValue(info[2].As<Napi::String>().Utf8Value());
    }
    if (type == REG_NONE) {
        type = REG_BINARY;
    }

    Napi::Buffer<byte> data = info[1].As<Napi::Buffer<byte>>();
    return Napi::Boolean::New(info.Env(), _regKey->setValue(
        info[0].As<Napi::String>().Utf8Value(),
        data.Data(),
        data.Length(),
        type
    ));
}

Napi::Value RegKeyWrap::applyValues(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsArray()) {
        throw Napi::TypeError::New(info.Env(), "Array of Values Expected");
    }

    Napi::Array values = info[0].As<Napi::Array>();
    int success = 0;
    for (uint32_t i = 0; i < values.Length(); i++) {
        if (!values.Get(i).IsObject()) {
            continue;
        }

        Napi::Object value = values.Get(i).As<Napi::Object>();
        std::string name = value.Get("name").As<Napi::String>().Utf8Value();
        DWORD type = REG_BINARY;
        if (value.Has("type") && value.Get("type").IsString()) {
            type = getKeyTypeValue(value.Get("type").As<Napi::String>().Utf8Value());
        }
        Napi::Value data = value.Get("data");


        if (data.IsBuffer()) {
            Napi::Buffer<byte> bufferData = data.As<Napi::Buffer<byte>>();
            if (_regKey->setValue(name, bufferData.Data(), bufferData.Length(), type)) {
                success++;
            }
            continue;
        }

        // 支持值类型转换
        if (data.IsString()) {
            if (_regKey->setStringValue(name, data.As<Napi::String>().Utf8Value())) {
                success++;
            }
        } else if (data.IsNumber()) {
            Napi::Number number = data.As<Napi::Number>();
            QWORD val = number.Int64Value();
            double dVal = number.DoubleValue();
            if (val == dVal)
            {
                if (info.Env(), _regKey->setQwordValue(name, val)) {
                    success++;
                }
            }
            else
            {
                // 转化为字符串存储
                std::string sVal = std::to_string(dVal);
                if (info.Env(), _regKey->setStringValue(name, sVal)) {
                    success++;
                }
            }
        }
    }

    return Napi::Number::New(info.Env(), success);
}

Napi::Value RegKeyWrap::setStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
    if (info.Length() < 2 || !info[1].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value Expected");
    }

    std::string valueName = info[0].As<Napi::String>().Utf8Value();
    std::string value = info[1].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(info.Env(), _regKey->setStringValue(valueName, value));
}

Napi::Value RegKeyWrap::setNumberValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info.Length() < 2 || !info[1].IsNumber()) {
        throw Napi::TypeError::New(info.Env(), "Value Expected");
    }

    Napi::Number value = info[1].As<Napi::Number>();

    QWORD val = value.Int64Value();
    double dVal = value.DoubleValue();
    if (val == dVal) {
        return Napi::Boolean::New(info.Env(), _regKey->setQwordValue(
                                                  info[0].As<Napi::String>().Utf8Value(),
                                                  val
                                                ));
    } else {
        // 转化为字符串存储
        std::string sVal = std::to_string(dVal);
        return Napi::Boolean::New(info.Env(), _regKey->setStringValue(
                                                  info[0].As<Napi::String>().Utf8Value(),
                                                  sVal
                                                ));
    }
}

Napi::Value RegKeyWrap::deleteValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }

    if (info[0].IsString()) {
        std::string valueName = info[0].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(info.Env(), _regKey->deleteValue(valueName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value Name Expected");
    }
}

Napi::Value RegKeyWrap::getValueNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey->getValueNames();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++, i++)
    {
        results.Set(i, Napi::String::New(info.Env(), *it));
    }
    return results;
}

Napi::Value RegKeyWrap::rename(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "New Key Name Expected");
    }

    if (info[0].IsString()) {
        std::string newName = info[0].As<Napi::String>().Utf8Value();
        if (Napi::Boolean::New(info.Env(), _regKey->rename(newName))) {
            _path = _path.substr(0, _path.find_last_of('\\')) + '\\' + newName;
            return Napi::Boolean::New(info.Env(), true);
        }
        return Napi::Boolean::New(info.Env(), false);
    } else {
        throw Napi::TypeError::New(info.Env(), "New Key Name Expected");
    }
}

Napi::Value RegKeyWrap::deleteKey(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey->deleteKey());
}

Napi::Value RegKeyWrap::openSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey Expected");
    }

    HKEY hKey;
    std::string keyName;
    if (info[0].IsString()) {
        keyName = info[0].As<Napi::String>().Utf8Value();
        keyName = replace_string(keyName, "/", "\\");
        hKey = _regKey->openSubkey(keyName);
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey shuold be a String");
    }

    if (hKey == NULL)
    {
        return info.Env().Null();
    }

    return RegKeyWrap::NewInstance(info.Env(), hKey, _path + '\\' + keyName);
}

Napi::Value RegKeyWrap::createSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey Expected");
    }

    if (info[0].IsString()) {
        std::string keyName = info[0].As<Napi::String>().Utf8Value();
        keyName = replace_string(keyName, "/", "\\");
        std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::tolower);
        HKEY hKey = _regKey->createSubkey(keyName);

        if (hKey == NULL)
        {
            return info.Env().Null();
        }

        return RegKeyWrap::NewInstance(info.Env(), hKey, _path + '\\' + keyName);
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey shuold be a String");
    }
}

Napi::Value RegKeyWrap::deleteSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey Expected");
    }

    if (info[0].IsString()) {
        std::string keyName = info[0].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(info.Env(), _regKey->deleteSubkey(keyName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey shuold be a String");
    }
}

Napi::Value RegKeyWrap::getSubkeyNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto subkeys = _regKey->getSubkeyNames();
    int i = 0;
    for (auto it = subkeys.begin(); it != subkeys.end(); it++, i++)
    {
        results.Set(i, Napi::String::New(info.Env(), *it));
    }
    return results;
}

Napi::Value RegKeyWrap::hasSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey Expected");
    }

    if (info[0].IsString()) {
        std::string keyName = info[0].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(info.Env(), _regKey->hasSubkey(keyName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey Name shuold be a String");
    }
}

Napi::Value RegKeyWrap::isSubkeyWriteable(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey Expected");
    }

    if (info[0].IsString()) {
        std::string keyName = info[0].As<Napi::String>().Utf8Value();
        return Napi::Boolean::New(info.Env(), _regKey->isSubkeyWriteable(keyName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey Name shuold be a String");
    }
}