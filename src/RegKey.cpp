#include "RegKey.h"
#include <codecvt>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

RegKey::RegKey(HKEY baseKey, const std::string &subKeyName, const std::string &hostname, REGSAM samDesired)
{
    _hKey = baseKey;
    if (!hostname.empty()) {
        connect(_hKey, hostname);
    }
    if (!subKeyName.empty()) {
        create(_hKey, subKeyName, samDesired);
    }
}

HKEY RegKey::open(HKEY baseKey, const std::string &subKeyName, REGSAM samDesired)
{
    close();

    if (samDesired && RegOpenKeyExA(baseKey, subKeyName.c_str(), samDesired, 0, &_hKey) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!samDesired && RegOpenKeyA(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::create(HKEY baseKey, const std::string &subKeyName, REGSAM samDesired)
{
    close();

    if (samDesired && RegCreateKeyExA(baseKey, subKeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, samDesired, NULL, &_hKey, NULL) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!samDesired && RegCreateKeyA(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::connect(HKEY baseKey, const std::string &hostname)
{
    close();

    if (RegConnectRegistryA(hostname.c_str(), baseKey, &_hKey) == ERROR_SUCCESS)
        return _hKey;
    _hKey = NULL;
    return NULL;
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
        RegCloseKey(_hKey);
        _hKey = NULL;
    }
}

bool RegKey::copyTree(HKEY hSrc)
{
    if (_hKey == NULL)
        return false;

    return RegCopyTreeA(hSrc, NULL, _hKey) == ERROR_SUCCESS;
}

bool RegKey::rename(const std::string &newName)
{
    if (_hKey == NULL)
        return false;

    // 转换成 wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return RegRenameKey(_hKey, NULL, converter.from_bytes(newName).c_str()) == ERROR_SUCCESS;
}

ValueInfo RegKey::getValue(const std::string &valueName, bool *success)
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
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, &info.type, NULL, &size) != ERROR_SUCCESS) {
        if (success != NULL)
            *success = false;
        return info;
    }
    if (size > 0) {
        info.data.resize(size);
        if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, info.data.data(), &size) != ERROR_SUCCESS) {
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

int RegKey::getValueSize(const std::string &valueName)
{
    if (_hKey == NULL)
        return -1;

    DWORD size = 0;
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &size) == ERROR_SUCCESS)
        return size;
    else
        return -1;
}

ByteArray RegKey::getBinaryValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return ByteArray();
    }

    DWORD valueSize = 0;
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize) != ERROR_SUCCESS) {
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
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, value.data(), &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;
    
    return value;
}

std::string RegKey::getStringValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return "";
    }
        
    DWORD valueSize = 0;
    DWORD type = 0;
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize) != ERROR_SUCCESS
        || (type != REG_SZ && type != REG_EXPAND_SZ)) {
        if (success != NULL)
            *success = false;
        return "";
    }
    if (valueSize <= 0) {
        if (success != NULL) {
            *success = true;
        }
        return "";
    }
    char *valueData = new char[valueSize];
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    std::string value = res ? valueData : "";
    delete[] valueData;
    return value;
}

DWORD RegKey::getDwordValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

QWORD RegKey::getQwordValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    QWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

std::list<std::string> RegKey::getMultiStringValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return {};
    }

    DWORD valueSize = 0;
    DWORD type = REG_NONE;
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize) != ERROR_SUCCESS
        || type != REG_MULTI_SZ) {
        if (success != NULL)
            *success = false;
        return {};
    }

    std::list<std::string> values;

    if (valueSize <= 0) {
        if (success != NULL)
            *success = true;
        return values;
    }

    char *valueData = new char[valueSize];
    char *p = valueData;
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize) == ERROR_SUCCESS);
    if (res) {
        while (p < valueData + valueSize - 1) {
            values.push_back(p);
            p += strlen(p) + 1;
        }
    }
    delete[] valueData;
    if (success != NULL)
        *success = res;

    return values;
}

std::list<ValueInfo> RegKey::getValues() const
{
    if (_hKey == NULL)
        return {};

    DWORD maxName = 0, maxValue = 0;
    RegQueryInfoKeyA(_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxName, &maxValue, NULL, NULL);
    maxName++;

    std::list<ValueInfo> values;
    char *valueName = new char[maxName];
    byte *valueData = new byte[maxValue];

    for (DWORD index = 0; ; index++) {
        ValueInfo valueInfo;
        DWORD valueNameSize = maxName;
        valueInfo.type = REG_NONE;
        DWORD valueSize = 0;
        if (RegEnumValueA(_hKey, index, valueName, &valueNameSize, NULL, &valueInfo.type, NULL, &valueSize) != ERROR_SUCCESS) {
            break;
        }
        valueInfo.name = valueName;

        if (valueSize > 0) {
            if (RegQueryValueExA(_hKey, valueInfo.name.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize) != ERROR_SUCCESS) {
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

std::list<std::string> RegKey::getValueNames() const
{
    if (_hKey == NULL)
        return std::list<std::string>();

    std::list<std::string> valueNames;
    char valueName[MAX_VALUE_NAME];
    DWORD valueNameSize;
    for (DWORD index = 0; ; index++) {
        valueNameSize = MAX_VALUE_NAME;
        if (RegEnumValueA(_hKey, index, valueName, &valueNameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
            break;
        valueNames.push_back(valueName);
    }

    return valueNames;
}

bool RegKey::putValue(const ValueInfo &value)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, value.name.c_str(), 0,
        value.type, value.data.data(), DWORD(value.data.size())) == ERROR_SUCCESS;
}

bool RegKey::setStringValue(const std::string &valueName, const std::string &value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, type, 
        reinterpret_cast<const BYTE *>(value.c_str()), DWORD(value.size() + 1)) == ERROR_SUCCESS;
}

bool RegKey::setBinaryValue(const std::string &valueName, const void *value, size_t size, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        static_cast<const BYTE *>(value), DWORD(size)) == ERROR_SUCCESS;
}

bool RegKey::setDwordValue(const std::string &valueName, DWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(DWORD)) == ERROR_SUCCESS;
}

bool RegKey::setQwordValue(const std::string &valueName, QWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(QWORD)) == ERROR_SUCCESS;
}

bool RegKey::setMultiStringValue(const std::string &valueName, const std::list<std::string> &value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    size_t size = 1;
    for (auto it = value.begin(); it != value.end(); it++) {
        size += it->size() + 1;
    }
    char *valueData = new char[size];
    char *p = valueData;
    for (auto it = value.begin(); it != value.end(); it++) {
        strcpy_s(p, it->size() + 1, it->c_str());
        p += it->size() + 1;
    }
    *p = '\0';
    bool res = (RegSetValueExA(_hKey, valueName.c_str(), 0, type, LPBYTE(valueData), DWORD(size)) == ERROR_SUCCESS);
    delete[] valueData;

    return res;
}

int RegKey::putValues(const std::list<ValueInfo> &values)
{
    if (_hKey == NULL)
        return 0;

    int successCount = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        if (RegSetValueExA(_hKey, it->name.c_str(), 0, it->type,
            it->data.data(), DWORD(it->data.size())) == ERROR_SUCCESS) {
            successCount++;
        }
    }
    return successCount;
}

DWORD RegKey::getValueType(const std::string &valueName) const
{
    if (_hKey == NULL)
        return REG_NONE;

    DWORD type = 0;
    RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, NULL);
    return type;
}

bool RegKey::deleteKey() const
{
    if (_hKey == NULL)
        return false;

    return RegDeleteTreeA(_hKey, NULL) == ERROR_SUCCESS;
}

HKEY RegKey::openSubkey(const std::string &subkeyName, REGSAM samDesired)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (samDesired && RegOpenKeyExA(_hKey, subkeyName.c_str(), 0, samDesired, &hKey) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!samDesired && RegOpenKeyA(_hKey, subkeyName.c_str(), &hKey) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

HKEY RegKey::createSubkey(const std::string &subkeyName, REGSAM samDesired)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (samDesired && RegCreateKeyExA(_hKey, subkeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, samDesired, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!samDesired && RegCreateKeyA(_hKey, subkeyName.c_str(), &hKey) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

bool RegKey::deleteSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return false;

    return RegDeleteTreeA(_hKey, subkeyName.c_str()) == ERROR_SUCCESS;
}

bool RegKey::hasValue(const std::string &valueName) const
{
    if (_hKey == NULL)
        return false;

    DWORD valueSize = 0;
    return RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize) == ERROR_SUCCESS;
}

bool RegKey::isWritable() const
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (RegOpenKeyExA(_hKey, NULL, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

bool RegKey::deleteValue(const std::string &valueName)
{
    if (_hKey == NULL)
        return false;

    return RegDeleteValueA(_hKey, valueName.c_str()) == ERROR_SUCCESS;
}

bool RegKey::hasSubkey(const std::string &subkeyName) const
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (RegOpenKeyA(_hKey, subkeyName.c_str(), &hKey) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

std::list<std::string> RegKey::getSubkeyNames() const
{
    if (_hKey == NULL)
        return std::list<std::string>();

    std::list<std::string> subkeyNames;
    char subkeyName[MAX_VALUE_NAME];
    for (DWORD index = 0; ; index++) {
        if (RegEnumKeyA(_hKey, index, subkeyName, MAX_VALUE_NAME) != ERROR_SUCCESS)
            break;
        subkeyNames.push_back(subkeyName);
    }
    return subkeyNames;
}
