#include "RegKeyWrap.h"

inline Napi::String ConvertToNapiString(Napi::Env env, const String &str)
{
    return Napi::String::New(
        env,
        reinterpret_cast<const char16_t *>(str.c_str()),
        str.size()
    );
}

inline String ConvertToStdString(const Napi::String &str)
{
    const auto wstr = str.Utf16Value();
    return String(
        reinterpret_cast<const wchar_t *>(wstr.c_str()),
        wstr.size()
    );
}

HKEY ParseBaseKey(const String &baseKeyName)
{
    String upperName = baseKeyName;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);

    if (upperName == STR("HKEY_CLASSES_ROOT") || upperName == STR("HKCR"))
        return HKEY_CLASSES_ROOT;
    else if (upperName == STR("HKEY_CURRENT_USER") || upperName == STR("HKCU"))
        return HKEY_CURRENT_USER;
    else if (upperName == STR("HKEY_LOCAL_MACHINE") || upperName == STR("HKLM"))
        return HKEY_LOCAL_MACHINE;
    else if (upperName == STR("HKEY_USERS") || upperName == STR("HKU"))
        return HKEY_USERS;
    else if (upperName == STR("HKEY_PERFORMANCE_DATA") || upperName == STR("HKPD"))
        return HKEY_PERFORMANCE_DATA;
    else if (upperName == STR("HKEY_PERFORMANCE_NLSTEXT") || upperName == STR("HKPN"))
        return HKEY_PERFORMANCE_NLSTEXT;
    else if (upperName == STR("HKEY_PERFORMANCE_TEXT") || upperName == STR("HKPT"))
        return HKEY_PERFORMANCE_TEXT;
    else if (upperName == STR("HKEY_CURRENT_CONFIG") || upperName == STR("HKCC"))
        return HKEY_CURRENT_CONFIG;
    
    return NULL;
}

DWORD ParseKeyType(const String &keyType, DWORD fallbackValue = REG_NONE)
{
    if (keyType == STR("REG_SZ"))
        return REG_SZ;
    else if (keyType == STR("REG_EXPAND_SZ"))
        return REG_EXPAND_SZ;
    else if (keyType == STR("REG_BINARY"))
        return REG_BINARY;
    else if (keyType == STR("REG_DWORD") || keyType == STR("REG_DWORD_LITTLE_ENDIAN"))
        return REG_DWORD;
    else if (keyType == STR("REG_DWORD_BIG_ENDIAN"))
        return REG_DWORD_BIG_ENDIAN;
    else if (keyType == STR("REG_QWORD") || keyType == STR("REG_QWORD_LITTLE_ENDIAN"))
        return REG_QWORD;
    else if (keyType == STR("REG_MULTI_SZ"))
        return REG_MULTI_SZ;
    else if (keyType == STR("REG_RESOURCE_LIST"))
        return REG_RESOURCE_LIST;
    else if (keyType == STR("REG_FULL_RESOURCE_DESCRIPTOR"))
        return REG_FULL_RESOURCE_DESCRIPTOR;
    else if (keyType == STR("REG_RESOURCE_REQUIREMENTS_LIST"))
        return REG_RESOURCE_REQUIREMENTS_LIST;
    
    return fallbackValue;
}

String StringifyKeyTypeName(DWORD keyType)
{
    switch (keyType)
    {
    case REG_NONE:
        return STR("REG_NONE");
    case REG_SZ:
        return STR("REG_SZ");
    case REG_EXPAND_SZ:
        return STR("REG_EXPAND_SZ");
    case REG_BINARY:
        return STR("REG_BINARY");
    case REG_DWORD:
        return STR("REG_DWORD");
    case REG_DWORD_BIG_ENDIAN:
        return STR("REG_DWORD_BIG_ENDIAN");
    case REG_QWORD:
        return STR("REG_QWORD");
    case REG_MULTI_SZ:
        return STR("REG_MULTI_SZ");
    case REG_RESOURCE_LIST:
        return STR("REG_RESOURCE_LIST");
    case REG_FULL_RESOURCE_DESCRIPTOR:
        return STR("REG_FULL_RESOURCE_DESCRIPTOR");
    case REG_RESOURCE_REQUIREMENTS_LIST:
        return STR("REG_RESOURCE_REQUIREMENTS_LIST");
    default:
        return STR("");
    }
}

String ReplaceString(const String &str, const String &from, const String &to)
{
    String result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos)
    {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

String TranslateError(DWORD errorCode)
{
	LPWSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, // Source
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPWSTR)&messageBuffer, // Output buffer
		0, // Buffer size (ignored because FORMAT_MESSAGE_ALLOCATE_BUFFER is set)
		nullptr // Arguments
	);

	if (size == 0)
    {
        // Failed to format message, handle error...
        LocalFree(messageBuffer);
        Char errorMessage[64];
        wsprintfW(errorMessage, STR("An unknown error occurred. (code %d)"), errorCode);
        return errorMessage;
	}

	String errorMessage(messageBuffer, size);
	LocalFree(messageBuffer); // Free the buffer allocated by FormatMessage

	return errorMessage;
}

Napi::FunctionReference RegKeyWrap::constructor;

Napi::Object RegKeyWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function cons = DefineClass(env, "RegKey", {
        InstanceAccessor("path", &RegKeyWrap::GetPath, nullptr),
        InstanceAccessor("host", &RegKeyWrap::GetHost, nullptr),
        InstanceAccessor("name", &RegKeyWrap::GetName, &RegKeyWrap::SetName),
        InstanceAccessor("valid", &RegKeyWrap::IsValid, nullptr),
        InstanceAccessor("lastStatus", &RegKeyWrap::GetLastStatus, &RegKeyWrap::SetLastStatus),
        InstanceMethod("isWritable", &RegKeyWrap::IsWritable),
        InstanceMethod("flush", &RegKeyWrap::Flush),

        InstanceMethod("getLastError", &RegKeyWrap::GetLastError),
        InstanceMethod("copyTree", &RegKeyWrap::CopyTree),
        InstanceMethod("close", &RegKeyWrap::Close),

        InstanceMethod("deleteTree", &RegKeyWrap::DeleteTree),
        InstanceMethod("openSubkey", &RegKeyWrap::OpenSubkey),
        InstanceMethod("createSubkey", &RegKeyWrap::CreateSubkey),
        InstanceMethod("deleteSubkey", &RegKeyWrap::DeleteSubkey),
        InstanceMethod("getSubkeyNames", &RegKeyWrap::GetSubkeyNames),
        InstanceMethod("hasSubkey", &RegKeyWrap::HasSubkey),

        InstanceMethod("getBinaryValue", &RegKeyWrap::GetBinaryValue),
        InstanceMethod("getStringValue", &RegKeyWrap::GetStringValue),
        InstanceMethod("getMultiStringValue", &RegKeyWrap::GetMultiStringValue),
        InstanceMethod("getDwordValue", &RegKeyWrap::GetDwordValue),
        InstanceMethod("getQwordValue", &RegKeyWrap::GetQwordValue),
        InstanceMethod("getValueType", &RegKeyWrap::GetValueType),
        InstanceMethod("hasValue", &RegKeyWrap::HasValue),
        InstanceMethod("getValueNames", &RegKeyWrap::GetValueNames),

        InstanceMethod("setBinaryValue", &RegKeyWrap::SetBinaryValue),
        InstanceMethod("setStringValue", &RegKeyWrap::SetStringValue),
        InstanceMethod("setMultiStringValue", &RegKeyWrap::SetMultiStringValue),
        InstanceMethod("setDwordValue", &RegKeyWrap::SetDwordValue),
        InstanceMethod("setQwordValue", &RegKeyWrap::SetQwordValue),
        InstanceMethod("deleteValue", &RegKeyWrap::DeleteValue)
    });

    constructor = Napi::Persistent(cons);
    constructor.SuppressDestruct();
    exports.Set("RegKey", cons);

    exports.Set("hkcr", NewInstance(env, HKEY_CLASSES_ROOT,        STR("HKEY_CLASS_ROOT")));
    exports.Set("hkcu", NewInstance(env, HKEY_CURRENT_USER,        STR("HKEY_CURRENT_USER")));
    exports.Set("hklm", NewInstance(env, HKEY_LOCAL_MACHINE,       STR("HKEY_LOCAL_MACHINE")));
    exports.Set("hku" , NewInstance(env, HKEY_USERS,               STR("HKEY_USERS")));
    exports.Set("hkcc", NewInstance(env, HKEY_CURRENT_CONFIG,      STR("HKEY_CURRENT_CONFIG")));
    exports.Set("hkpd", NewInstance(env, HKEY_PERFORMANCE_DATA,    STR("HKEY_PERFORMANCE_DATA")));
    exports.Set("hkpt", NewInstance(env, HKEY_PERFORMANCE_TEXT,    STR("HKEY_PERFORMANCE_TEXT")));
    exports.Set("hkpn", NewInstance(env, HKEY_PERFORMANCE_NLSTEXT, STR("HKEY_PERFORMANCE_NLSTEXT")));

    return exports;
}

Napi::Object RegKeyWrap::NewInstance(Napi::Env env, HKEY hKey, const String &path)
{
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({
        Napi::External<HKEY>::New(env, &hKey),
        ConvertToNapiString(env, path)
    });
    return scope.Escape(obj).ToObject();
}

RegKeyWrap::RegKeyWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RegKeyWrap>(info)
{
    String hostname, baseKeyName, subkeyName;
    REGSAM access = 0;
    if (info[0].IsExternal())
    {
        Napi::External<HKEY> external = info[0].As<Napi::External<HKEY>>();
        _regKey.Attach(*external.Data());
        _path = ConvertToStdString(info[1].As<Napi::String>());
        return;
    }
    else if (info[0].IsString())
    {
        String path = ReplaceString(ConvertToStdString(info[0].As<Napi::String>()), STR("/"), STR("\\"));
        if (path.substr(0, 2) == STR("\\\\"))
        {
            size_t slashPos = path.find(STR('\\'), 2);
            if (slashPos != std::string::npos)
            {
                hostname = path.substr(2, slashPos);
                path = path.substr(slashPos + 1);
            }
            else
            {
                _ThrowRegKeyError(info, "Invalid path format.");
                return;
            }
        }

        size_t slashPos = path.find(STR('\\'));
        // Allow leading slash
        if (slashPos == 0)
        {
            path = path.substr(1);
            slashPos = path.find(STR('\\'));
        }
        if (slashPos != std::string::npos)
        {
            baseKeyName = path.substr(0, slashPos);
            subkeyName = path.substr(slashPos + 1);
        }
        else
            baseKeyName = path;

        Napi::Value accessValue = info[1];
        if (accessValue.IsNumber())
            access = accessValue.As<Napi::Number>().Uint32Value();
        else if (accessValue.IsArray())
        {
            Napi::Array accessArray = accessValue.As<Napi::Array>();
            for (size_t i = 0; i < accessArray.Length(); i++)
                access |= accessArray.Get(i).As<Napi::Number>().Uint32Value();
        }
    }
    else if (info[0].IsObject())
    {
        Napi::Object options = info[0].As<Napi::Object>();

        Napi::Value hostValue = options.Get("hostname");
        Napi::Value baseKeyValue = options.Get("baseKey");
        Napi::Value subkeyValue = options.Get("subkey");
        Napi::Value accessValue = options.Get("access");

        if (!baseKeyValue.IsString())
            throw Napi::TypeError::New(info.Env(), "baseKey must be a string.");

        baseKeyName = ConvertToStdString(baseKeyValue.As<Napi::String>());
        if (hostValue.IsString())
            hostname = ConvertToStdString(hostValue.As<Napi::String>());
        if (subkeyValue.IsString())
            subkeyName = ReplaceString(ConvertToStdString(subkeyValue.As<Napi::String>()), STR("/"), STR("\\"));
        if (accessValue.IsNumber())
            access = accessValue.As<Napi::Number>().Uint32Value();
        else if (accessValue.IsArray())
        {
            Napi::Array arr = accessValue.As<Napi::Array>();
            for (uint32_t i = 0; i < arr.Length(); i++)
                access |= arr.Get(i).As<Napi::Number>().Uint32Value();
        }
    }
    else
        throw Napi::TypeError::New(info.Env(), "Path or options expected.");

    if (!hostname.empty())
        _path = STR("\\\\") + hostname + STR('\\') + baseKeyName;
    else
        _path = baseKeyName;

    if (!subkeyName.empty())
        _path += STR('\\') + subkeyName;

    HKEY baseKey = ParseBaseKey(baseKeyName);
    if (baseKey == NULL)
    {
        _ThrowRegKeyError(info, "Invalid base key name.");
        return;
    }
    if (_regKey.ConnectAndCreate(baseKey, subkeyName, hostname, access) == NULL)
    {
        _ThrowRegKeyError(info, "Failed to create registry key.");
    }
}

Napi::Value RegKeyWrap::GetPath(const Napi::CallbackInfo &info)
{
    return ConvertToNapiString(info.Env(), _path);
}

Napi::Value RegKeyWrap::GetHost(const Napi::CallbackInfo &info)
{
    if (_path.find(STR("\\\\")) == 0)
        return ConvertToNapiString(info.Env(), _path.substr(2, _path.find(STR("\\"), 2) - 2));
        
    return Napi::String::New(info.Env(), "");
}

Napi::Value RegKeyWrap::GetName(const Napi::CallbackInfo &info)
{
    size_t pos = _path.find_last_of(STR("\\"));
    if (pos == std::string::npos)
        return ConvertToNapiString(info.Env(), _path);
        
    return ConvertToNapiString(info.Env(), _path.substr(pos + 1));
}

void RegKeyWrap::SetName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    if (value.IsString())
    {
        String newName = ConvertToStdString(info[0].As<Napi::String>());
        if (_regKey.Rename(newName))
            _path = _path.substr(0, _path.find_last_of(STR('\\'))) + STR('\\') + newName;
    }
    else
        throw Napi::TypeError::New(info.Env(), "New key name expected.");
}

Napi::Value RegKeyWrap::IsValid(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey.IsValid());
}

Napi::Value RegKeyWrap::GetLastStatus(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), _regKey.GetLastStatus());
}

void RegKeyWrap::SetLastStatus(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    if (value.IsNumber())
        _regKey.SetLastStatus(value.As<Napi::Number>().Int32Value());
    else
        throw Napi::TypeError::New(info.Env(), "Number expected.");
}

Napi::Value RegKeyWrap::IsWritable(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey.IsWritable());
}

Napi::Value RegKeyWrap::Flush(const Napi::CallbackInfo &info)
{
    bool res = _regKey.Flush();
    if (!res)
        _ThrowRegKeyError(info, "Failed to flush key.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::GetLastError(const Napi::CallbackInfo &info)
{
    return ConvertToNapiString(info.Env(), TranslateError(_regKey.GetLastStatus()));
}

Napi::Value RegKeyWrap::CopyTree(const Napi::CallbackInfo &info)
{
    if (info[0].IsObject() && info[0].As<Napi::Object>().InstanceOf(RegKeyWrap::constructor.Value()))
    {
        RegKeyWrap *pRegKeyWrap = Napi::ObjectWrap<RegKeyWrap>::Unwrap(info[0].As<Napi::Object>());
        bool res = _regKey.CopyTree(pRegKeyWrap->_regKey.GetHandle());
        if (!res)
            _ThrowRegKeyError(info, "Failed to copy tree.");
        return Napi::Boolean::New(info.Env(), res);
    }
    else
        throw Napi::TypeError::New(info.Env(), "Invalid source key.");
}

Napi::Value RegKeyWrap::Close(const Napi::CallbackInfo &info)
{
    _regKey.Close();
    return info.Env().Undefined();
}

Napi::Value RegKeyWrap::DeleteTree(const Napi::CallbackInfo &info)
{
    bool res;
    if (info[0].IsString())
        res = _regKey.DeleteTree(ConvertToStdString(info[0].As<Napi::String>()));
    else if (info.Length() == 0)
        res = _regKey.DeleteTree();
    else
    {
        _ThrowRegKeyError(info, "Invalid arguments.");
        return Napi::Boolean::New(info.Env(), false);
    }

    if (!res)
        _ThrowRegKeyError(info, "Failed to delete key.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::OpenSubkey(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Subkey name expected.");

    HKEY hKey;
    REGSAM access = 0;
    String keyName = ConvertToStdString(info[0].As<Napi::String>());
    keyName = ReplaceString(keyName, STR("/"), STR("\\"));
        
    if (info[1].IsNumber())
        access = info[1].As<Napi::Number>().Uint32Value();
    else if (info[1].IsArray())
    {
        Napi::Array accessArray = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < accessArray.Length(); i++)
        {
            if (accessArray.Get(i).IsNumber())
                access |= accessArray.Get(i).As<Napi::Number>().Uint32Value();
        }
    }

    hKey = _regKey.OpenSubkey(keyName, access);
    if (hKey == NULL)
    {
        _ThrowRegKeyError(info, "Failed to open subkey.");
        return info.Env().Null();
    }

    return RegKeyWrap::NewInstance(info.Env(), hKey, _path + STR('\\') + keyName);
}

Napi::Value RegKeyWrap::CreateSubkey(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Subkey name expected.");

    HKEY hKey;
    REGSAM access = 0;
    String keyName = ConvertToStdString(info[0].As<Napi::String>());
    keyName = ReplaceString(keyName, STR("/"), STR("\\"));
        
    if (info[1].IsNumber())
        access = info[1].As<Napi::Number>().Uint32Value();
    else if (info[1].IsArray())
    {
        Napi::Array accessArray = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < accessArray.Length(); i++)
        {
            if (accessArray.Get(i).IsNumber())
                access |= accessArray.Get(i).As<Napi::Number>().Uint32Value();
        }
    }

    hKey = _regKey.CreateSubkey(keyName, access);
    if (hKey == NULL)
    {
        _ThrowRegKeyError(info, "Failed to create subkey.");
        return info.Env().Null();
    }

    return RegKeyWrap::NewInstance(info.Env(), hKey, _path + STR('\\') + keyName);
}

Napi::Value RegKeyWrap::DeleteSubkey(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String keyName = ConvertToStdString(info[0].As<Napi::String>());
        bool res = _regKey.DeleteSubkey(keyName);
        if (!res)
            _ThrowRegKeyError(info, "Failed to delete subkey.");
        return Napi::Boolean::New(info.Env(), res);
    }
    else
        throw Napi::TypeError::New(info.Env(), "Subkey name expected.");
}

Napi::Value RegKeyWrap::GetSubkeyNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto subkeys = _regKey.GetSubkeyNames();
    int i = 0;
    for (auto it = subkeys.begin(); it != subkeys.end(); it++)
        results.Set(i++, ConvertToNapiString(info.Env(), *it));
        
    return results;
}

Napi::Value RegKeyWrap::HasSubkey(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String keyName = ConvertToStdString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.HasSubkey(keyName));
    }
    else
        throw Napi::TypeError::New(info.Env(), "Subkey name expected.");
}

Napi::Value RegKeyWrap::GetBinaryValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool success = false;
        ByteArray buffer = _regKey.GetBinaryValue(valueName, &success);
        if (!success)
        {
            _ThrowRegKeyError(info, "Failed to get value.", valueName);
            return info.Env().Null();
        }
            
        return Napi::Buffer<BYTE>::Copy(info.Env(), buffer.data(), buffer.size());
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetStringValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool success = false;
        String value = _regKey.GetStringValue(valueName, &success);
        if (!success)
        {
            _ThrowRegKeyError(info, "Failed to get value.", valueName);
            return info.Env().Null();
        }

        return ConvertToNapiString(info.Env(), value);
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetMultiStringValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool success = false;
        auto values = _regKey.GetMultiStringValue(valueName, &success);
        if (!success)
        {
            _ThrowRegKeyError(info, "Failed to get value.", valueName);
            return info.Env().Null();
        }

        Napi::Array results = Napi::Array::New(info.Env());
        int i = 0;
        for (auto it = values.begin(); it != values.end(); it++)
            results.Set(i++, ConvertToNapiString(info.Env(), *it));
            
        return results;
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetDwordValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool success = false;
        DWORD value = _regKey.GetDwordValue(valueName, &success);
        if (!success)
        {
            _ThrowRegKeyError(info, "Failed to get value.", valueName);
            return info.Env().Null();
        }
        return Napi::Number::New(info.Env(), double(value));
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetQwordValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool success = false;
        QWORD value = _regKey.GetQwordValue(valueName, &success);
        if (!success)
        {
            _ThrowRegKeyError(info, "Failed to get value.", valueName);
            return info.Env().Null();
        }
        return Napi::BigInt::New(info.Env(), value);
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetValueType(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        DWORD type = _regKey.GetValueType(valueName);
        if (!type)
            _ThrowRegKeyError(info, "Failed to get value type.", valueName);
            
        return ConvertToNapiString(info.Env(), StringifyKeyTypeName(type));
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::HasValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.HasValue(valueName));
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

Napi::Value RegKeyWrap::GetValueNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey.GetValueNames();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++)
        results.Set(i++, ConvertToNapiString(info.Env(), *it));
        
    return results;
}

Napi::Value RegKeyWrap::SetBinaryValue(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Value name expected.");

    if (!info[1].IsBuffer())
        throw Napi::TypeError::New(info.Env(), "Buffer value expected.");

    DWORD type = REG_BINARY;
    if (info[2].IsString())
        type = ParseKeyType(ConvertToStdString(info[2].As<Napi::String>()), REG_BINARY);

    Napi::Buffer<byte> data = info[1].As<Napi::Buffer<byte>>();
    bool res = _regKey.SetBinaryValue(ConvertToStdString(info[0].As<Napi::String>()),
                                      data.Data(),
                                      data.Length(),
                                      type);
    if (!res)
        _ThrowRegKeyError(info, "Failed to set binary value.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::SetStringValue(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
        
    if (!info[1].IsString())
        throw Napi::TypeError::New(info.Env(), "String value expected.");

    DWORD type = REG_SZ;
    if (info[2].IsString())
        type = ParseKeyType(ConvertToStdString(info[2].As<Napi::String>()), REG_SZ);

    String valueName = ConvertToStdString(info[0].As<Napi::String>());
    String value = ConvertToStdString(info[1].As<Napi::String>());
    bool res = _regKey.SetStringValue(valueName, value, type);
    if (!res)
        _ThrowRegKeyError(info, "Failed to set string value.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::SetMultiStringValue(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Value name expected.");

    if (!info[1].IsArray())
        throw Napi::TypeError::New(info.Env(), "Array of string value expected.");

    DWORD type = REG_MULTI_SZ;
    if (info[2].IsString())
        type = ParseKeyType(ConvertToStdString(info[2].As<Napi::String>()), REG_MULTI_SZ);

    Napi::Array values = info[1].As<Napi::Array>();
    std::vector<String> data;
    for (uint32_t i = 0; i < values.Length(); i++)
        data.push_back(ConvertToStdString(values.Get(i).ToString()));

    bool res = _regKey.SetMultiStringValue(
        ConvertToStdString(info[0].As<Napi::String>()), data, type);
    if (!res)
        _ThrowRegKeyError(info, "Failed to set multi-string value.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::SetDwordValue(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString())
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
        
    if (!info[1].IsNumber())
        throw Napi::TypeError::New(info.Env(), "Number value expected.");
    
    DWORD type = REG_DWORD;
    if (info[2].IsString())
        type = ParseKeyType(ConvertToStdString(info[2].As<Napi::String>()), REG_DWORD);

    bool res = _regKey.SetDwordValue(ConvertToStdString(info[0].As<Napi::String>()),
                                     info[1].As<Napi::Number>().Uint32Value(),
                                     type);
    if (!res)
        _ThrowRegKeyError(info, "Failed to set DWORD value.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::SetQwordValue(const Napi::CallbackInfo &info)
{
    if (!info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
    }
    if (!info[1].IsNumber()) {
        throw Napi::TypeError::New(info.Env(), "Number value expected.");
    }

    QWORD value = 0;
    if (info[1].IsNumber())
        value = QWORD(info[1].As<Napi::Number>().Uint32Value());
    else if (info[1].IsBigInt())
    {
        bool loss = false;
        value = info[1].As<Napi::BigInt>().Uint64Value(&loss);
        if (loss)
        {
            _ThrowRegKeyError(info, "BigInt value too big.");
            return Napi::Boolean::New(info.Env(), false);
        }
    }
    else
        throw Napi::TypeError::New(info.Env(), "Number value expected.");
    DWORD type = REG_QWORD;
    if (info[2].IsString())
        type = ParseKeyType(ConvertToStdString(info[2].As<Napi::String>()), REG_QWORD);

    bool res = _regKey.SetQwordValue(ConvertToStdString(info[0].As<Napi::String>()), value, type);
    if (!res)
        _ThrowRegKeyError(info, "Failed to set QWORD value.");
    return Napi::Boolean::New(info.Env(), res);
}

Napi::Value RegKeyWrap::DeleteValue(const Napi::CallbackInfo &info)
{
    if (info[0].IsString())
    {
        String valueName = ConvertToStdString(info[0].As<Napi::String>());
        bool res = _regKey.DeleteValue(valueName);
        if (!res)
            _ThrowRegKeyError(info, "Failed to set delete value.");
        return Napi::Boolean::New(info.Env(), res);
    }
    else
        throw Napi::TypeError::New(info.Env(), "Value name expected.");
}

void RegKeyWrap::_ThrowRegKeyError(const Napi::CallbackInfo &info,
                                   const std::string &message,
                                   const String &value)
{
    // 从 prototype 获取 __throwRegKeyError__ 函数
    Napi::Object thisObj = info.This().As<Napi::Object>();
    Napi::Value regKeyError = thisObj.Get("__throwRegKeyError__");
    if (regKeyError.IsFunction())
    {
        Napi::Function throwFunc = regKeyError.As<Napi::Function>();
        throwFunc.Call({
            Napi::String::New(info.Env(), message),
            thisObj,
            ConvertToNapiString(info.Env(), value),
            ConvertToNapiString(info.Env(), TranslateError(_regKey.GetLastStatus()))
        });
    }
}
