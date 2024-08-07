#include "RegKeyWrap.h"
#include <regex>

inline Napi::String makeString(Napi::Env env, const std::wstring& str)
{
    return Napi::String::New(
        env,
        reinterpret_cast<const char16_t *>(str.c_str()),
        str.size()
    );
}

inline std::wstring toWString(const Napi::String &str)
{
    const auto wstr = str.Utf16Value();
    return std::wstring(
        reinterpret_cast<const wchar_t *>(wstr.c_str()),
        wstr.size()
    );
}

HKEY getBaseKey(const std::wstring &baseKeyName)
{
    HKEY baseKey = NULL;
    std::wstring upperName = baseKeyName;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
    if (upperName == L"HKEY_CLASSES_ROOT" || upperName == L"HKCR")
    {
        baseKey = HKEY_CLASSES_ROOT;
    }
    else if (upperName == L"HKEY_CURRENT_USER" || upperName == L"HKCU")
    {
        baseKey = HKEY_CURRENT_USER;
    }
    else if (upperName == L"HKEY_LOCAL_MACHINE" || upperName == L"HKLM")
    {
        baseKey = HKEY_LOCAL_MACHINE;
    }
    else if (upperName == L"HKEY_USERS" || upperName == L"HKU")
    {
        baseKey = HKEY_USERS;
    }
    else if (upperName == L"HKEY_PERFORMANCE_DATA" || upperName == L"HKPD")
    {
        baseKey = HKEY_PERFORMANCE_DATA;
    }
    else if (upperName == L"HKEY_PERFORMANCE_NLSTEXT" || upperName == L"HKPN")
    {
        baseKey = HKEY_PERFORMANCE_NLSTEXT;
    }
    else if (upperName == L"HKEY_PERFORMANCE_TEXT" || upperName == L"HKPT")
    {
        baseKey = HKEY_PERFORMANCE_TEXT;
    }
    else if (upperName == L"HKEY_CURRENT_CONFIG" || upperName == L"HKCC")
    {
        baseKey = HKEY_CURRENT_CONFIG;
    }
    
    return baseKey;
}

DWORD getKeyTypeValue(const std::wstring &keyType)
{
    if (keyType == L"REG_SZ")
    {
        return REG_SZ;
    }
    else if (keyType == L"REG_EXPAND_SZ")
    {
        return REG_EXPAND_SZ;
    }
    else if (keyType == L"REG_BINARY")
    {
        return REG_BINARY;
    }
    else if (keyType == L"REG_DWORD" || keyType == L"REG_DWORD_LITTLE_ENDIAN")
    {
            return REG_DWORD;
    }
    else if (keyType == L"REG_DWORD_BIG_ENDIAN")
    {
        return REG_DWORD_BIG_ENDIAN;
    }
    else if (keyType == L"REG_QWORD" || keyType == L"REG_QWORD_LITTLE_ENDIAN")
    {
        return REG_QWORD;
    }
    else if (keyType == L"REG_MULTI_SZ")
    {
        return REG_MULTI_SZ;
    }
    else if (keyType == L"REG_RESOURCE_LIST")
    {
        return REG_RESOURCE_LIST;
    }
    else if (keyType == L"REG_FULL_RESOURCE_DESCRIPTOR")
    {
        return REG_FULL_RESOURCE_DESCRIPTOR;
    }
    else if (keyType == L"REG_RESOURCE_REQUIREMENTS_LIST")
    {
        return REG_RESOURCE_REQUIREMENTS_LIST;
    }
    
    return REG_NONE;
}

std::wstring getKeyTypeName(DWORD keyType)
{
    switch (keyType)
    {
    case REG_SZ:
        return L"REG_SZ";
    case REG_EXPAND_SZ:
        return L"REG_EXPAND_SZ";
    case REG_BINARY:
        return L"REG_BINARY";
    case REG_DWORD:
        return L"REG_DWORD";
    case REG_DWORD_BIG_ENDIAN:
        return L"REG_DWORD_BIG_ENDIAN";
    case REG_QWORD:
        return L"REG_QWORD";
    case REG_MULTI_SZ:
        return L"REG_MULTI_SZ";
    case REG_RESOURCE_LIST:
        return L"REG_RESOURCE_LIST";
    case REG_FULL_RESOURCE_DESCRIPTOR:
        return L"REG_FULL_RESOURCE_DESCRIPTOR";
    case REG_RESOURCE_REQUIREMENTS_LIST:
        return L"REG_RESOURCE_REQUIREMENTS_LIST";
    default:
        return L"REG_NONE";
    }
}

std::wstring replace_string(const std::wstring& str, const std::wstring &from, const std::wstring &to)
{
    std::wstring result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos)
    {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::wstring getErrorString(DWORD errorCode) {
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

	if (size == 0) {
        // Failed to format message, handle error...
        LocalFree(messageBuffer);
        wchar_t errorMessage[64];
        wsprintfW(errorMessage, L"Failed to format message with error code %d.", errorCode);
        return errorMessage;
	}

	std::wstring errorMessage(messageBuffer, size);
	LocalFree(messageBuffer); // Free the buffer allocated by FormatMessage

	return errorMessage;
}

Napi::FunctionReference RegKeyWrap::constructor;

Napi::Object RegKeyWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "RegKey", {
        InstanceAccessor("path", &RegKeyWrap::getPath, nullptr),
        InstanceAccessor("host", &RegKeyWrap::getHost, nullptr),
        InstanceAccessor("name", &RegKeyWrap::getName, &RegKeyWrap::setName),
        InstanceAccessor("open", &RegKeyWrap::isOpen, nullptr),
        InstanceAccessor("lastStatus", &RegKeyWrap::getLastStatus, &RegKeyWrap::setLastStatus),

        InstanceMethod("getLastError", &RegKeyWrap::getLastError),
        InstanceMethod("copy", &RegKeyWrap::copy),
        InstanceMethod("close", &RegKeyWrap::close),

        InstanceMethod("getBinaryValue", &RegKeyWrap::getBinaryValue),
        InstanceMethod("getStringValue", &RegKeyWrap::getStringValue),
        InstanceMethod("getMultiStringValue", &RegKeyWrap::getMultiStringValue),
        InstanceMethod("getDwordValue", &RegKeyWrap::getDwordValue),
        InstanceMethod("getQwordValue", &RegKeyWrap::getQwordValue),
        InstanceMethod("getValueType", &RegKeyWrap::getValueType),
        InstanceMethod("hasValue", &RegKeyWrap::hasValue),
        InstanceMethod("getValueNames", &RegKeyWrap::getValueNames),

        InstanceMethod("setBinaryValue", &RegKeyWrap::setBinaryValue),
        InstanceMethod("setStringValue", &RegKeyWrap::setStringValue),
        InstanceMethod("setMultiStringValue", &RegKeyWrap::setMultiStringValue),
        InstanceMethod("setDwordValue", &RegKeyWrap::setDwordValue),
        InstanceMethod("setQwordValue", &RegKeyWrap::setQwordValue),
        InstanceMethod("deleteValue", &RegKeyWrap::deleteValue),

        InstanceMethod("delete", &RegKeyWrap::deleteKey),
        InstanceMethod("openSubkey", &RegKeyWrap::openSubkey),
        InstanceMethod("createSubkey", &RegKeyWrap::createSubkey),
        InstanceMethod("deleteSubkey", &RegKeyWrap::deleteSubkey),
        InstanceMethod("getSubkeyNames", &RegKeyWrap::getSubkeyNames),
        InstanceMethod("hasSubkey", &RegKeyWrap::hasSubkey),
        InstanceMethod("isWritable", &RegKeyWrap::isWritable)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("RegKey", func);

    exports.Set("hkcr", NewInstance(env, HKEY_CLASSES_ROOT,        L"HKEY_CLASS_ROOT"));
    exports.Set("hkcu", NewInstance(env, HKEY_CURRENT_USER,        L"HKEY_CURRENT_USER"));
    exports.Set("hklm", NewInstance(env, HKEY_LOCAL_MACHINE,       L"HKEY_LOCAL_MACHINE"));
    exports.Set("hku" , NewInstance(env, HKEY_USERS,               L"HKEY_USERS"));
    exports.Set("hkcc", NewInstance(env, HKEY_CURRENT_CONFIG,      L"HKEY_CURRENT_CONFIG"));
    exports.Set("hkpd", NewInstance(env, HKEY_PERFORMANCE_DATA,    L"HKEY_PERFORMANCE_DATA"));
    exports.Set("hkpt", NewInstance(env, HKEY_PERFORMANCE_TEXT,    L"HKEY_PERFORMANCE_TEXT"));
    exports.Set("hkpn", NewInstance(env, HKEY_PERFORMANCE_NLSTEXT, L"HKEY_PERFORMANCE_NLSTEXT"));

    return exports;
}

Napi::Object RegKeyWrap::NewInstance(Napi::Env env, HKEY hKey, const std::wstring &path)
{
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({
        Napi::External<HKEY>::New(env, &hKey),
        makeString(env, path)
    });
    return scope.Escape(obj).ToObject();
}

RegKeyWrap::RegKeyWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RegKeyWrap>(info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Base key expected");
    }

    if (info[0].IsExternal()) {
        Napi::External<HKEY> external = info[0].As<Napi::External<HKEY>>();
        _regKey.attach(*external.Data());
        _path = toWString(info[1].As<Napi::String>());
    } else if (info[0].IsString()) {
        _path = toWString(info[0].As<Napi::String>());

        REGSAM access = 0;

        for (uint32_t i = 1; i < info.Length(); i++) {
            if (info[i].IsString()) {
                _path += L"\\" + toWString(info[i].As<Napi::String>());
            } else if (info[i].IsNumber()) {
                access |= info[i].As<Napi::Number>().Int32Value();
            } else if (info[i].IsArray()) {
                Napi::Array accessArray = info[i].As<Napi::Array>();
                for (uint32_t j = 0; j < accessArray.Length(); j++) {
                    if (accessArray.Get(j).IsNumber()) {
                        access |= accessArray.Get(j).As<Napi::Number>().Int32Value();
                    }
                }
            } else {
                throw Napi::TypeError::New(info.Env(), "Invalid key name");
            }
        }

        _path = replace_string(_path, L"/", L"\\");

        std::wstring hostname, baseKeyName, subKeyName;

        std::wregex pattern(L"^(\\\\\\\\(.+?)\\\\)?([a-zA-Z_]+?)(\\\\(.*))?$");
        std::wsmatch match;
        if (std::regex_match(_path, match, pattern) && match.size() == 6) {
            hostname = match[2];
            baseKeyName = match[3];
            subKeyName = match[5];
        } else {
            _throwRegKeyError(info, "Invalid key path");
            return;
        }

        HKEY baseKey = getBaseKey(baseKeyName);
        if (baseKey == NULL) {
            _throwRegKeyError(info, "Invalid base key name");
            return;
        }

        _regKey.reset(baseKey, subKeyName, hostname, access);
    } else {
        throw Napi::TypeError::New(info.Env(), "Invalid base key");
    }
}

Napi::Value RegKeyWrap::isOpen(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey.isOpen());
}

Napi::Value RegKeyWrap::getPath(const Napi::CallbackInfo &info)
{
    return makeString(info.Env(), _path);
}

Napi::Value RegKeyWrap::getHost(const Napi::CallbackInfo &info)
{
    if (_path.find(L"\\\\") == 0) {
        return makeString(info.Env(), _path.substr(2, _path.find(L"\\", 2) - 2));
    }
    return Napi::String::New(info.Env(), "");
}

Napi::Value RegKeyWrap::getName(const Napi::CallbackInfo &info)
{
    size_t pos = _path.find_last_of(L"\\");
    if (pos == std::string::npos) {
        return makeString(info.Env(), _path);
    }
    return makeString(info.Env(), _path.substr(pos + 1));
}

Napi::Value RegKeyWrap::close(const Napi::CallbackInfo &info)
{
    _regKey.close();
    return info.Env().Undefined();
}

Napi::Value RegKeyWrap::copy(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Source key expected");
    }

    if (info[0].IsObject()
        && info[0].As<Napi::Object>().InstanceOf(RegKeyWrap::constructor.Value())) {
        RegKeyWrap *pRegKeyWrap = Napi::ObjectWrap<RegKeyWrap>::Unwrap(info[0].As<Napi::Object>());
        return Napi::Boolean::New(info.Env(), _regKey.copyTree(pRegKeyWrap->_regKey.getHandle()));
    } else {
        throw Napi::TypeError::New(info.Env(), "Invalid source key");
    }
}

Napi::Value RegKeyWrap::getBinaryValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        bool success = false;
        ByteArray buffer = _regKey.getBinaryValue(valueName, &success);
        if (!success) {
            _throwRegKeyError(info, "Failed to get value", valueName);
            return info.Env().Null();
        }
        if (buffer.empty()) {
            return Napi::Buffer<BYTE>::New(info.Env(), 0);
        }
        return Napi::Buffer<BYTE>::Copy(info.Env(), buffer.data(), buffer.size());
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        bool success = false;
        std::wstring value = _regKey.getStringValue(valueName, &success);
        if (!success) {
            _throwRegKeyError(info, "Failed to get value", valueName);
            return info.Env().Null();
        }
        return makeString(info.Env(), value);
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getMultiStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        bool success = false;
        auto values = _regKey.getMultiStringValue(valueName, &success);
        if (!success) {
            _throwRegKeyError(info, "Failed to get value", valueName);
            return info.Env().Null();
        }
        Napi::Array results = Napi::Array::New(info.Env());
        int i = 0;
        for (auto it = values.begin(); it != values.end(); it++, i++) {
            results.Set(i, makeString(info.Env(), *it));
        }
        return results;
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getDwordValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        bool success = false;
        DWORD value = _regKey.getDwordValue(valueName, &success);
        if (!success) {
            _throwRegKeyError(info, "Failed to get value", valueName);
            return info.Env().Null();
        }
        return Napi::Number::New(info.Env(), double(value));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getQwordValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        bool success = false;
        QWORD value = _regKey.getQwordValue(valueName, &success);
        if (!success) {
            _throwRegKeyError(info, "Failed to get value", valueName);
            return info.Env().Null();
        }
        return Napi::BigInt::New(info.Env(), value);
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getValueType(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        DWORD type = _regKey.getValueType(valueName);
        if (!type) {
            _throwRegKeyError(info, "Failed to get value type", valueName);
        }
        return makeString(info.Env(), getKeyTypeName(type));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::hasValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.hasValue(valueName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

Napi::Value RegKeyWrap::getValueNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto values = _regKey.getValueNames();
    int i = 0;
    for (auto it = values.begin(); it != values.end(); it++, i++)
    {
        results.Set(i, makeString(info.Env(), *it));
    }
    return results;
}

Napi::Value RegKeyWrap::setBinaryValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info.Length() < 2 || !info[1].IsBuffer()) {
        throw Napi::TypeError::New(info.Env(), "Buffer value expected");
    }

    DWORD type = REG_BINARY;
    if (info.Length() > 2 && info[2].IsString()) {
        type = getKeyTypeValue(toWString(info[2].As<Napi::String>()));
    }
    if (type == REG_NONE) {
        type = REG_BINARY;
    }

    Napi::Buffer<byte> data = info[1].As<Napi::Buffer<byte>>();
    return Napi::Boolean::New(info.Env(), _regKey.setBinaryValue(
        toWString(info[0].As<Napi::String>()),
        data.Data(),
        data.Length(),
        type
    ));
}

Napi::Value RegKeyWrap::setStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
    if (info.Length() < 2 || !info[1].IsString()) {
        throw Napi::TypeError::New(info.Env(), "String value expected");
    }

    DWORD type = REG_SZ;
    if (info.Length() > 2 && info[2].IsString()) {
        type = getKeyTypeValue(toWString(info[2].As<Napi::String>()));
    }
    if (type == REG_NONE) {
        type = REG_SZ;
    }

    std::wstring valueName = toWString(info[0].As<Napi::String>());
    std::wstring value = toWString(info[1].As<Napi::String>());
    return Napi::Boolean::New(info.Env(), _regKey.setStringValue(valueName, value, type));
}

Napi::Value RegKeyWrap::setMultiStringValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info.Length() < 2 || !info[1].IsArray()) {
        throw Napi::TypeError::New(info.Env(), "Array of string value expected");
    }

    DWORD type = REG_MULTI_SZ;
    if (info.Length() > 2 && info[2].IsString()) {
        type = getKeyTypeValue(toWString(info[2].As<Napi::String>()));
    }
    if (type == REG_NONE) {
        type = REG_MULTI_SZ;
    }

    Napi::Array values = info[1].As<Napi::Array>();
    std::list<std::wstring> data;
    for (uint32_t i = 0; i < values.Length(); i++) {
        if (values.Get(i).IsString()) {
            data.push_back(toWString(values.Get(i).As<Napi::String>()));
        }
    }
    return Napi::Boolean::New(info.Env(), _regKey.setMultiStringValue(
                                                toWString(info[0].As<Napi::String>()),
                                                data,
                                                type
                                              ));
}

Napi::Value RegKeyWrap::setDwordValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
    if (info.Length() < 2 || !info[1].IsNumber()) {
        throw Napi::TypeError::New(info.Env(), "Number value expected");
    }
    
    DWORD type = REG_DWORD;
    if (info.Length() > 2 && info[2].IsString()) {
        type = getKeyTypeValue(toWString(info[2].As<Napi::String>()));
    }
    if (type == REG_NONE) {
        type = REG_DWORD;
    }

    return Napi::Boolean::New(info.Env(), _regKey.setDwordValue(
                                                toWString(info[0].As<Napi::String>()),
                                                info[1].As<Napi::Number>().Uint32Value(),
                                                type
                                              ));
}

Napi::Value RegKeyWrap::setQwordValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
    if (info.Length() < 2) {
        throw Napi::TypeError::New(info.Env(), "Number value expected");
    }

    QWORD value = 0;
    bool loss = false;
    if (info[1].IsNumber()) {
        value = QWORD(info[1].As<Napi::Number>().Int64Value());
    } else if (info[1].IsBigInt()) {
        value = info[1].As<Napi::BigInt>().Uint64Value(&loss);
    } else {
        throw Napi::TypeError::New(info.Env(), "Number value expected");
    }
    DWORD type = REG_QWORD;
    if (info.Length() > 2 && info[2].IsString()) {
        type = getKeyTypeValue(toWString(info[2].As<Napi::String>()));
    }
    if (type == REG_NONE) {
        type = REG_QWORD;
    }

    return Napi::Boolean::New(info.Env(), _regKey.setQwordValue(
                                                toWString(info[0].As<Napi::String>()),
                                                value,
                                                type
                                              ));
}

Napi::Value RegKeyWrap::deleteValue(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1 || !info[0].IsString()) {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }

    if (info[0].IsString()) {
        std::wstring valueName = toWString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.deleteValue(valueName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Value name expected");
    }
}

void RegKeyWrap::setName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    if (value.IsString()) {
        std::wstring newName = toWString(info[0].As<Napi::String>());
        if (_regKey.rename(newName)) {
            _path = _path.substr(0, _path.find_last_of(L'\\')) + L'\\' + newName;
        }
    } else {
        throw Napi::TypeError::New(info.Env(), "New key name expected");
    }
}

Napi::Value RegKeyWrap::getLastStatus(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), _regKey.getLastStatus());
}

void RegKeyWrap::setLastStatus(const Napi::CallbackInfo &info, const Napi::Value &value)
{
    if (value.IsNumber()) {
        _regKey.setLastStatus(value.As<Napi::Number>().Int32Value());
    } else {
        throw Napi::TypeError::New(info.Env(), "Number expected");
    }
}

Napi::Value RegKeyWrap::getLastError(const Napi::CallbackInfo &info)
{
    return makeString(info.Env(), getErrorString(_regKey.getLastStatus()));
}

Napi::Value RegKeyWrap::deleteKey(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey.deleteKey());
}

Napi::Value RegKeyWrap::openSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey name expected");
    }

    HKEY hKey;
    std::wstring keyName;
    REGSAM access = 0;
    if (info[0].IsString()) {
        keyName = toWString(info[0].As<Napi::String>());
        keyName = replace_string(keyName, L"/", L"\\");
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey name shuold be a String");
    }
    if (info.Length() > 1) {
        if (info[1].IsNumber()) {
            access = info[1].As<Napi::Number>().Int32Value();
        } else if (info[1].IsArray()) {
            Napi::Array accessArray = info[1].As<Napi::Array>();
            for (uint32_t i = 0; i < accessArray.Length(); i++) {
                if (accessArray.Get(i).IsNumber()) {
                    access |= accessArray.Get(i).As<Napi::Number>().Int32Value();
                }
            }
        }
    }

    hKey = _regKey.openSubkey(keyName, access);
    if (hKey == NULL) {
        _throwRegKeyError(info, "Failed to open subkey");
        return info.Env().Null();
    }

    return RegKeyWrap::NewInstance(info.Env(), hKey, _path + L'\\' + keyName);
}

Napi::Value RegKeyWrap::createSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey name expected");
    }

    HKEY hKey;
    std::wstring keyName;
    REGSAM access = 0;
    if (info[0].IsString()) {
        keyName = toWString(info[0].As<Napi::String>());
        keyName = replace_string(keyName, L"/", L"\\");
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey name shuold be a String");
    }
    if (info.Length() > 1) {
        if (info[1].IsNumber()) {
            access = info[1].As<Napi::Number>().Int32Value();
        } else if (info[1].IsArray()) {
            Napi::Array accessArray = info[1].As<Napi::Array>();
            for (uint32_t i = 0; i < accessArray.Length(); i++) {
                if (accessArray.Get(i).IsNumber()) {
                    access |= accessArray.Get(i).As<Napi::Number>().Int32Value();
                }
            }
        }
    }

    hKey = _regKey.createSubkey(keyName, access);
    if (hKey == NULL) {
        _throwRegKeyError(info, "Failed to create subkey");
        return info.Env().Null();
    }

    return RegKeyWrap::NewInstance(info.Env(), hKey, _path + L'\\' + keyName);
}

Napi::Value RegKeyWrap::deleteSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey name expected");
    }

    if (info[0].IsString()) {
        std::wstring keyName = toWString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.deleteSubkey(keyName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey name shuold be a String");
    }
}

Napi::Value RegKeyWrap::getSubkeyNames(const Napi::CallbackInfo &info)
{
    Napi::Array results = Napi::Array::New(info.Env());
    auto subkeys = _regKey.getSubkeyNames();
    int i = 0;
    for (auto it = subkeys.begin(); it != subkeys.end(); it++, i++)
    {
        results.Set(i, makeString(info.Env(), *it));
    }
    return results;
}

Napi::Value RegKeyWrap::hasSubkey(const Napi::CallbackInfo &info)
{
    if (info.Length() < 1) {
        throw Napi::TypeError::New(info.Env(), "Subkey name expected");
    }

    if (info[0].IsString()) {
        std::wstring keyName = toWString(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), _regKey.hasSubkey(keyName));
    } else {
        throw Napi::TypeError::New(info.Env(), "Subkey name shuold be a string");
    }
}

Napi::Value RegKeyWrap::isWritable(const Napi::CallbackInfo &info)
{
    return Napi::Boolean::New(info.Env(), _regKey.isWritable());
}

void RegKeyWrap::_throwRegKeyError(const Napi::CallbackInfo &info,
                                   const std::string &message,
                                   const std::wstring &value)
{
    // 从 prototype 获取 __throwRegKeyError__ 函数
    Napi::Object thisObj = info.This().As<Napi::Object>();
    Napi::Value regKeyError = thisObj.Get("__throwRegKeyError__");
    if (regKeyError.IsFunction()) {
        Napi::Function throwFunc = regKeyError.As<Napi::Function>();
        throwFunc.Call({
            Napi::String::New(info.Env(), message),
            thisObj,
            makeString(info.Env(), value),
            makeString(info.Env(), getErrorString(_regKey.getLastStatus()))
        });
    }
}
