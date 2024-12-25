#include "RegKey.h"

RegKey::RegKey(HKEY baseKey, const String &subKeyName, const String &hostname, REGSAM access)
    : _hKey(NULL)
    , _lastStatus(ERROR_SUCCESS)
{
    if (baseKey)
        ConnectAndCreate(baseKey, subKeyName, hostname, access);
}

HKEY RegKey::Open(HKEY baseKey, const String &subKeyName, REGSAM access)
{
    if (!Close())
        return NULL;

    if (access)
    {
        if (SetLastStatus(RegOpenKeyExW(baseKey, subKeyName.c_str(), access, 0, &_hKey)) == ERROR_SUCCESS)
            return _hKey;
    }
    else
    {
        if (SetLastStatus(RegOpenKeyW(baseKey, subKeyName.c_str(), &_hKey)) == ERROR_SUCCESS)
            return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::Create(HKEY baseKey, const String &subKeyName, REGSAM access)
{
    if (!Close())
        return NULL;

    if (access)
    {
        if (SetLastStatus(RegCreateKeyExW(baseKey, subKeyName.c_str(), 0, NULL,
                                          REG_OPTION_NON_VOLATILE, access, NULL, &_hKey, NULL)) == ERROR_SUCCESS)
            return _hKey;
    }
    else
    {
        if (SetLastStatus(RegCreateKeyW(baseKey, subKeyName.c_str(), &_hKey)) == ERROR_SUCCESS)
            return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::Connect(const String &hostname, HKEY baseKey)
{
    if (!Close())
        return NULL;

    if (SetLastStatus(RegConnectRegistryW(hostname.c_str(), baseKey, &_hKey)) == ERROR_SUCCESS)
        return _hKey;
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::ConnectAndCreate(HKEY baseKey, const String &subKeyName, const String &hostname, REGSAM access)
{
    if (!Close())
        return NULL;

    HKEY hKey = baseKey;
    if (!hostname.empty())
    {
        if (SetLastStatus(RegConnectRegistryW(hostname.c_str(), hKey, &hKey)) != ERROR_SUCCESS)
            return NULL;
    }
    if (!subKeyName.empty())
    {
        if (access)
        {
            if (SetLastStatus(RegCreateKeyExW(baseKey, subKeyName.c_str(), 0, NULL,
                                              REG_OPTION_NON_VOLATILE, access, NULL, &hKey, NULL)) != ERROR_SUCCESS)
            {
                if (hKey != baseKey)
                    RegCloseKey(hKey);
                return NULL;
            }
        }
        else
        {
            if (SetLastStatus(RegCreateKeyW(baseKey, subKeyName.c_str(), &hKey)) != ERROR_SUCCESS)
            {
                if (hKey != baseKey)
                    RegCloseKey(hKey);
                return NULL;
            }
        }
    }
    _hKey = hKey;
    return hKey;
}

HKEY RegKey::Attach(HKEY hKey)
{
    HKEY oldKey = _hKey;
    _hKey = hKey;
    _lastStatus = ERROR_SUCCESS;
    return oldKey;
}

bool RegKey::Close()
{
    if (_hKey != NULL)
    {
        SetLastStatus(RegCloseKey(_hKey));
        _hKey = NULL;
        return _lastStatus == ERROR_SUCCESS;
    }
    return true;
}

bool RegKey::IsWritable()
{
    HKEY hKey = NULL;
    if (SetLastStatus(RegOpenKeyExW(_hKey, NULL, 0, KEY_WRITE, &hKey)) != ERROR_SUCCESS)
        return false;
    RegCloseKey(hKey);
    return true;
}

bool RegKey::Flush()
{
    return SetLastStatus(RegFlushKey(_hKey)) == ERROR_SUCCESS;
}

bool RegKey::CopyTree(HKEY hSrc)
{
    return SetLastStatus(RegCopyTreeW(hSrc, NULL, _hKey)) == ERROR_SUCCESS;
}

bool RegKey::Rename(const String &newName)
{
    return SetLastStatus(RegRenameKey(_hKey, NULL, newName.c_str())) == ERROR_SUCCESS;
}

HKEY RegKey::OpenSubkey(const String &subkeyName, REGSAM access)
{
    HKEY hKey = NULL;
    if (access)
    {
        if (SetLastStatus(RegOpenKeyExW(_hKey, subkeyName.c_str(), 0, access, &hKey)) == ERROR_SUCCESS)
            return hKey;
    }
    else
    {
        if (SetLastStatus(RegOpenKeyW(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS)
            return hKey;
    }

    return NULL;
}

HKEY RegKey::CreateSubkey(const String &subkeyName, REGSAM access)
{
    HKEY hKey = NULL;
    if (access)
    {
        if (SetLastStatus(RegCreateKeyExW(_hKey, subkeyName.c_str(), 0, NULL,
                                          REG_OPTION_NON_VOLATILE, access, NULL, &hKey, NULL)) == ERROR_SUCCESS)
            return hKey;
    }
    else
    {
        if (SetLastStatus(RegCreateKeyW(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS)
            return hKey;
    }

    return NULL;
}

bool RegKey::DeleteTree()
{
    return SetLastStatus(RegDeleteTreeW(_hKey, NULL)) == ERROR_SUCCESS;
}

bool RegKey::DeleteTree(const String &subkeyName)
{
    return SetLastStatus(RegDeleteKeyW(_hKey, subkeyName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::DeleteSubkey(const String &subkeyName)
{
    return SetLastStatus(RegDeleteTreeW(_hKey, subkeyName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::HasSubkey(const String &subkeyName)
{
    HKEY hKey = NULL;
    if (SetLastStatus(RegOpenKeyW(_hKey, subkeyName.c_str(), &hKey)) != ERROR_SUCCESS)
    {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

std::vector<String> RegKey::GetSubkeyNames()
{
    std::vector<String> subkeyNames;
    DWORD maxKeyLen = 0;
    if (SetLastStatus(RegQueryInfoKeyW(_hKey, NULL, NULL, NULL, NULL, &maxKeyLen, NULL, NULL, NULL, NULL, NULL, NULL)) != ERROR_SUCCESS)
        return subkeyNames;
    maxKeyLen++;
    std::unique_ptr<wchar_t[]> subkeyName(new wchar_t[maxKeyLen]);
    for (DWORD index = 0;; index++)
    {
        if (SetLastStatus(RegEnumKeyW(_hKey, index, subkeyName.get(), maxKeyLen)) != ERROR_SUCCESS)
            break;
        subkeyNames.push_back(subkeyName.get());
    }
    return subkeyNames;
}

RegValue RegKey::GetValue(const String &valueName, bool *success)
{
    RegValue info;
    info.name = valueName;
    info.type = REG_NONE;

    DWORD size = 0;
    if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &info.type, NULL, &size)) != ERROR_SUCCESS)
    {
        if (success != NULL)
            *success = false;
        return info;
    }
    if (size > 0)
    {
        info.data.resize(size);
        if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, info.data.data(), &size)) != ERROR_SUCCESS)
    {
            if (success != NULL)
                *success = false;
            return info;
        }
    }
    if (success != NULL)
        *success = true;
    return info;
}

DWORD RegKey::GetValueType(const String &valueName)
{
    DWORD type = REG_NONE;
    SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, NULL));
    return type;
}

DWORD RegKey::GetValueSize(const String &valueName)
{
    DWORD size = 0;
    if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &size)) == ERROR_SUCCESS)
        return size;
    else
        return 0;
}

ByteArray RegKey::GetBinaryValue(const String &valueName, bool *success)
{
    DWORD valueSize = 0;
    if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) != ERROR_SUCCESS)
    {
        if (success != NULL)
            *success = false;
        return ByteArray();
    }
    if (valueSize == 0)
    {
        if (success != NULL)
            *success = true;
        return ByteArray();
    }

    ByteArray value(valueSize, 0);
    bool res = (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, value.data(), &valueSize)) == ERROR_SUCCESS);

    if (success != NULL)
        *success = res;
    return value;
}

String RegKey::GetStringValue(const String &valueName, bool *success)
{
    DWORD valueSize = 0;
    DWORD type = 0;
    if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS)
    {
        if (success != NULL)
            *success = false;
        return STR("");
    }
    if (type != REG_SZ && type != REG_EXPAND_SZ)
    {
        SetLastStatus(ERROR_INVALID_DATA);
        if (success != NULL)
            *success = false;
        return STR("");
    }
    if (valueSize <= 0)
    {
        if (success != NULL)
            *success = true;
        return STR("");
    }
    
    std::unique_ptr<Char[]> valueData(new Char[valueSize]);
    bool res = (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData.get()), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    String value = res ? valueData.get() : STR("");
    return value;
}

DWORD RegKey::GetDwordValue(const String &valueName, bool *success)
{
    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    DWORD type = REG_NONE;
    bool res = (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);

    if (type != REG_DWORD)
    {
        SetLastStatus(ERROR_INVALID_DATA);
        res = false;
    }

    if (success != NULL)
        *success = res;
    return value;
}

QWORD RegKey::GetQwordValue(const String &valueName, bool *success)
{
    QWORD value = 0;
    DWORD valueSize = sizeof(value);
    DWORD type = REG_NONE;
    bool res = (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);

    if (type != REG_QWORD)
    {
        SetLastStatus(ERROR_INVALID_DATA);
        res = false;
    }

    if (success != NULL)
        *success = res;
    return value;
}

std::vector<String> RegKey::GetMultiStringValue(const String &valueName, bool *success)
{
    DWORD valueSize = 0;
    DWORD type = REG_NONE;
    if (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS)
    {
        if (success != NULL)
            *success = false;
        return {};
    }
    if (type != REG_MULTI_SZ)
    {
        SetLastStatus(ERROR_INVALID_DATA);
        if (success != NULL)
            *success = false;
        return {};
    }

    std::vector<String> values;

    if (valueSize <= 0)
    {
        if (success != NULL)
            *success = true;
        return values;
    }

    std::unique_ptr<Char[]> valueData(new Char[valueSize]);
    bool res = (SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData.get()), &valueSize)) == ERROR_SUCCESS);
    if (res)
    {
        for (wchar_t *p = valueData.get(); *p != 0; p += STRLEN(p) + 1)
            values.push_back(p);
    }
    
    if (success != NULL)
        *success = res;
    return values;
}

std::vector<RegValue> RegKey::GetValues()
{
    DWORD maxName = 0, maxValue = 0;
    if (SetLastStatus(
            RegQueryInfoKeyW(_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxName, &maxValue, NULL, NULL)
            != ERROR_SUCCESS))
        return {};
    maxName++;

    std::vector<RegValue> values;
    wchar_t *valueName = new wchar_t[maxName];
    byte *valueData = new byte[maxValue];

    for (DWORD index = 0; ; index++)
    {
        RegValue valueInfo;
        DWORD valueNameSize = maxName;
        valueInfo.type = REG_NONE;
        DWORD valueSize = 0;
        if (SetLastStatus(RegEnumValueW(_hKey, index, valueName, &valueNameSize, NULL,
                &valueInfo.type, NULL, &valueSize)) != ERROR_SUCCESS)
            break;
            
        valueInfo.name = valueName;

        if (valueSize > 0)
        {
            if (SetLastStatus(RegQueryValueExW(_hKey, valueInfo.name.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) != ERROR_SUCCESS)
                continue;
            valueInfo.data.assign(valueData, valueData + valueSize);
        }
        values.push_back(valueInfo);
    }

    delete[] valueData;
    delete[] valueName;

    return values;
}

std::vector<String> RegKey::GetValueNames()
{
    std::vector<String> valueNames;
    DWORD maxNameSize = 0;
    if (SetLastStatus(
            RegQueryInfoKeyW(_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxNameSize, NULL, NULL, NULL)
            != ERROR_SUCCESS))
        return valueNames;
    
    maxNameSize++;
    std::unique_ptr<Char[]> valueName(new Char[maxNameSize]);
    DWORD valueNameSize;
    for (DWORD index = 0; ; index++) {
        valueNameSize = maxNameSize;
        if (SetLastStatus(RegEnumValueW(_hKey, index, valueName.get(), &valueNameSize, nullptr, nullptr, nullptr, nullptr))
                != ERROR_SUCCESS)
            break;
        valueNames.push_back(valueName.get());
    }

    return valueNames;
}

bool RegKey::HasValue(const String &valueName)
{
    DWORD valueSize = 0;
    return SetLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) == ERROR_SUCCESS;
}

bool RegKey::PutValue(const RegValue &value)
{
    return SetLastStatus(RegSetValueExW(_hKey, value.name.c_str(), 0,
        value.type, value.data.data(), DWORD(value.data.size()))) == ERROR_SUCCESS;
}

size_t RegKey::PutValues(const std::vector<RegValue> &values)
{
    size_t successCount = 0;
    for (auto it = values.begin(); it != values.end(); it++)
    {
        if (PutValue(*it))
            successCount++;
        else
            break;
    }
    return successCount;
}

bool RegKey::SetStringValue(const String &valueName, const String &value, DWORD type)
{
    return SetLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type, 
        reinterpret_cast<const BYTE *>(value.c_str()), DWORD(sizeof(Char) * (value.size() + 1)))) == ERROR_SUCCESS;
}

bool RegKey::SetBinaryValue(const String &valueName, const void *value, size_t size, DWORD type)
{
    return SetLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        static_cast<const BYTE *>(value), DWORD(size))) == ERROR_SUCCESS;
}

bool RegKey::SetDwordValue(const String &valueName, DWORD value, DWORD type)
{
    return SetLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(DWORD))) == ERROR_SUCCESS;
}

bool RegKey::SetQwordValue(const String &valueName, QWORD value, DWORD type)
{
    return SetLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(QWORD))) == ERROR_SUCCESS;
}

bool RegKey::SetMultiStringValue(const String &valueName, const std::vector<String> &values, DWORD type)
{
    size_t size = 1;
    for (auto it = values.begin(); it != values.end(); it++)
        size += it->size() + 1;
        
    std::unique_ptr<Char[]> valueData(new Char[size]);
    wchar_t *p = valueData.get();
    for (auto it = values.begin(); it != values.end(); it++)
    {
        STRCOPY(p, it->size() + 1, it->c_str());
        p += it->size() + 1;
    }
    *p = 0;
    bool res = (SetLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type, LPBYTE(valueData.get()), DWORD(sizeof(Char) * size)))
                    == ERROR_SUCCESS);

    return res;
}

bool RegKey::DeleteValue(const String &valueName)
{
    return SetLastStatus(RegDeleteValueW(_hKey, valueName.c_str())) == ERROR_SUCCESS;
}