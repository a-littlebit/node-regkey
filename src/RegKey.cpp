#include "RegKey.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

RegKey::RegKey(HKEY baseKey, const std::wstring &subKeyName, const std::wstring &hostname, REGSAM access)
    : _hKey(NULL)
    , _lastStatus(ERROR_SUCCESS)
{
    if (baseKey) {
        reset(baseKey, subKeyName, hostname, access);
    }
}

HKEY RegKey::open(HKEY baseKey, const std::wstring &subKeyName, REGSAM access)
{
    close();

    if (access && setLastStatus(RegOpenKeyExW(baseKey, subKeyName.c_str(), access, 0, &_hKey)) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!access && setLastStatus(RegOpenKeyW(baseKey, subKeyName.c_str(), &_hKey)) == ERROR_SUCCESS) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::create(HKEY baseKey, const std::wstring &subKeyName, REGSAM access)
{
    close();

    if (access && setLastStatus(RegCreateKeyExW(baseKey, subKeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, access, NULL, &_hKey, NULL)) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!access && setLastStatus(RegCreateKeyW(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS)) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::connect(HKEY baseKey, const std::wstring &hostname)
{
    close();

    if (setLastStatus(RegConnectRegistryW(hostname.c_str(), baseKey, &_hKey)) == ERROR_SUCCESS)
        return _hKey;
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::reset(HKEY baseKey, const std::wstring &subKeyName, const std::wstring &hostname, REGSAM access)
{
    close();

    _hKey = baseKey;
    if (!hostname.empty()) {
        connect(_hKey, hostname);
    }
    if (!subKeyName.empty()) {
        create(_hKey, subKeyName, access);
    }
    return _hKey;
}

HKEY RegKey::attach(HKEY hKey)
{
    HKEY oldKey = _hKey;
    _hKey = hKey;
    return oldKey;
}

void RegKey::close()
{
    if (_hKey != NULL)
    {
        setLastStatus(RegCloseKey(_hKey));
        _hKey = NULL;
    }
}

bool RegKey::copyTree(HKEY hSrc)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegCopyTreeW(hSrc, NULL, _hKey)) == ERROR_SUCCESS;
}

bool RegKey::rename(const std::wstring &newName)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegRenameKey(_hKey, NULL, newName.c_str()) == ERROR_SUCCESS);
}

ValueInfo RegKey::getValue(const std::wstring &valueName, bool *success)
{
    ValueInfo info;
    info.name = valueName;
    info.type = REG_NONE;
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return info;
    }

    DWORD size = 0;
    if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &info.type, NULL, &size)) != ERROR_SUCCESS) {
        if (success != NULL)
            *success = false;
        return info;
    }
    if (size > 0) {
        info.data.resize(size);
        if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, info.data.data(), &size)) != ERROR_SUCCESS) {
            if (success != NULL) {
                *success = false;
            }
            return info;
        }
    }
    if (success != NULL) {
        *success = true;
    }
    return info;
}

int RegKey::getValueSize(const std::wstring &valueName)
{
    if (_hKey == NULL)
        return -1;

    DWORD size = 0;
    if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &size)) == ERROR_SUCCESS)
        return size;
    else
        return -1;
}

ByteArray RegKey::getBinaryValue(const std::wstring &valueName, bool *success)
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return ByteArray();
    }

    DWORD valueSize = 0;
    if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) != ERROR_SUCCESS) {
        if (success != NULL)
            *success = false;
        return ByteArray();
    }
    if (valueSize <= 0) {
        if (success != NULL)
            *success = true;
        return ByteArray();
    }
    ByteArray value(valueSize, 0);
    bool res = (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, value.data(), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;
    
    return value;
}

std::wstring RegKey::getStringValue(const std::wstring &valueName, bool *success)
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return L"";
    }
        
    DWORD valueSize = 0;
    DWORD type = 0;
    if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS
        || (type != REG_SZ && type != REG_EXPAND_SZ)) {
        if (success != NULL)
            *success = false;
        return L"";
    }
    if (valueSize <= 0) {
        if (success != NULL) {
            *success = true;
        }
        return L"";
    }
    wchar_t *valueData = new wchar_t[valueSize];
    bool res = (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    std::wstring value = res ? valueData : L"";
    delete[] valueData;
    return value;
}

DWORD RegKey::getDwordValue(const std::wstring &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

QWORD RegKey::getQwordValue(const std::wstring &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    QWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

std::list<std::wstring> RegKey::getMultiStringValue(const std::wstring &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return {};
    }

    DWORD valueSize = 0;
    DWORD type = REG_NONE;
    if (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS
        || type != REG_MULTI_SZ) {
        if (success != NULL)
            *success = false;
        return {};
    }

    std::list<std::wstring> values;

    if (valueSize <= 0) {
        if (success != NULL)
            *success = true;
        return values;
    }

    wchar_t *valueData = new wchar_t[valueSize];
    wchar_t *p = valueData;
    bool res = (setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) == ERROR_SUCCESS);
    if (res) {
        while (p < valueData + valueSize - 1) {
            values.push_back(p);
            p += wcslen(p) + 1;
        }
    }
    delete[] valueData;
    if (success != NULL)
        *success = res;

    return values;
}

std::list<ValueInfo> RegKey::getValues()
{
    if (_hKey == NULL)
        return {};

    DWORD maxName = 0, maxValue = 0;
    if (setLastStatus(
            RegQueryInfoKeyW(_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxName, &maxValue, NULL, NULL)
            != ERROR_SUCCESS)) {
        return {};
    }
    maxName++;

    std::list<ValueInfo> values;
    wchar_t *valueName = new wchar_t[maxName];
    byte *valueData = new byte[maxValue];

    for (DWORD index = 0; ; index++) {
        ValueInfo valueInfo;
        DWORD valueNameSize = maxName;
        valueInfo.type = REG_NONE;
        DWORD valueSize = 0;
        if (setLastStatus(RegEnumValueW(_hKey, index, valueName, &valueNameSize, NULL,
                &valueInfo.type, NULL, &valueSize)) != ERROR_SUCCESS) {
            break;
        }
        valueInfo.name = valueName;

        if (valueSize > 0) {
            if (setLastStatus(RegQueryValueExW(_hKey, valueInfo.name.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) != ERROR_SUCCESS) {
                continue;
            }
            valueInfo.data.assign(valueData, valueData + valueSize);
        }
        values.push_back(valueInfo);
    }

    delete[] valueData;
    delete[] valueName;

    return values;
}

std::list<std::wstring> RegKey::getValueNames()
{
    if (_hKey == NULL)
        return std::list<std::wstring>();

    std::list<std::wstring> valueNames;
    wchar_t valueName[MAX_VALUE_NAME];
    DWORD valueNameSize;
    for (DWORD index = 0; ; index++) {
        valueNameSize = MAX_VALUE_NAME;
        if (setLastStatus(RegEnumValueW(_hKey, index, valueName, &valueNameSize, nullptr, nullptr, nullptr, nullptr))
                != ERROR_SUCCESS)
            break;
        valueNames.push_back(valueName);
    }

    return valueNames;
}

bool RegKey::putValue(const ValueInfo &value)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExW(_hKey, value.name.c_str(), 0,
        value.type, value.data.data(), DWORD(value.data.size()))) == ERROR_SUCCESS;
}

bool RegKey::setStringValue(const std::wstring &valueName, const std::wstring &value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type, 
        reinterpret_cast<const BYTE *>(value.c_str()), DWORD(value.size() + 1))) == ERROR_SUCCESS;
}

bool RegKey::setBinaryValue(const std::wstring &valueName, const void *value, size_t size, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        static_cast<const BYTE *>(value), DWORD(size))) == ERROR_SUCCESS;
}

bool RegKey::setDwordValue(const std::wstring &valueName, DWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(DWORD))) == ERROR_SUCCESS;
}

bool RegKey::setQwordValue(const std::wstring &valueName, QWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(QWORD))) == ERROR_SUCCESS;
}

bool RegKey::setMultiStringValue(const std::wstring &valueName, const std::list<std::wstring> &value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    size_t size = 1;
    for (auto it = value.begin(); it != value.end(); it++) {
        size += it->size() + 1;
    }
    wchar_t *valueData = new wchar_t[size];
    wchar_t *p = valueData;
    for (auto it = value.begin(); it != value.end(); it++) {
        wcscpy_s(p, it->size() + 1, it->c_str());
        p += it->size() + 1;
    }
    *p = '\0';
    bool res = (setLastStatus(RegSetValueExW(_hKey, valueName.c_str(), 0, type, LPBYTE(valueData), DWORD(size))) == ERROR_SUCCESS);
    delete[] valueData;

    return res;
}

int RegKey::putValues(const std::list<ValueInfo> &values)
{
    if (_hKey == NULL)
        return 0;

    int successCount = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        if (setLastStatus(RegSetValueExW(_hKey, it->name.c_str(), 0, it->type,
                it->data.data(), DWORD(it->data.size()))) == ERROR_SUCCESS) {
            successCount++;
        }
    }
    return successCount;
}

DWORD RegKey::getValueType(const std::wstring &valueName)
{
    if (_hKey == NULL)
        return REG_NONE;

    DWORD type = 0;
    setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, &type, NULL, NULL));
    return type;
}

bool RegKey::deleteKey()
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteTreeW(_hKey, NULL)) == ERROR_SUCCESS;
}

HKEY RegKey::openSubkey(const std::wstring &subkeyName, REGSAM access)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (access && setLastStatus(RegOpenKeyExW(_hKey, subkeyName.c_str(), 0, access, &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!access && setLastStatus(RegOpenKeyW(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

HKEY RegKey::createSubkey(const std::wstring &subkeyName, REGSAM access)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (access && setLastStatus(RegCreateKeyExW(_hKey, subkeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, access, NULL, &hKey, NULL)) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!access && setLastStatus(RegCreateKeyW(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

bool RegKey::deleteSubkey(const std::wstring &subkeyName)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteTreeW(_hKey, subkeyName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::hasValue(const std::wstring &valueName)
{
    if (_hKey == NULL)
        return false;

    DWORD valueSize = 0;
    return setLastStatus(RegQueryValueExW(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) == ERROR_SUCCESS;
}

bool RegKey::isWritable()
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (setLastStatus(RegOpenKeyExW(_hKey, NULL, 0, KEY_WRITE, &hKey)) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

bool RegKey::deleteValue(const std::wstring &valueName)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteValueW(_hKey, valueName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::hasSubkey(const std::wstring &subkeyName)
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (setLastStatus(RegOpenKeyW(_hKey, subkeyName.c_str(), &hKey)) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

std::list<std::wstring> RegKey::getSubkeyNames()
{
    if (_hKey == NULL)
        return std::list<std::wstring>();

    std::list<std::wstring> subkeyNames;
    wchar_t subkeyName[MAX_VALUE_NAME];
    for (DWORD index = 0; ; index++) {
        if (setLastStatus(RegEnumKeyW(_hKey, index, subkeyName, MAX_VALUE_NAME)) != ERROR_SUCCESS)
            break;
        subkeyNames.push_back(subkeyName);
    }
    return subkeyNames;
}
