#include "RegKey.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

RegKey::RegKey(HKEY baseKey, const std::string &subKeyName, const std::string &hostname, REGSAM access)
    : _hKey(NULL)
    , _lastStatus(ERROR_SUCCESS)
{
    if (baseKey) {
        reset(baseKey, subKeyName, hostname, access);
    }
}

HKEY RegKey::open(HKEY baseKey, const std::string &subKeyName, REGSAM access)
{
    close();

    if (access && setLastStatus(RegOpenKeyExA(baseKey, subKeyName.c_str(), access, 0, &_hKey)) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!access && setLastStatus(RegOpenKeyA(baseKey, subKeyName.c_str(), &_hKey)) == ERROR_SUCCESS) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::create(HKEY baseKey, const std::string &subKeyName, REGSAM access)
{
    close();

    if (access && setLastStatus(RegCreateKeyExA(baseKey, subKeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, access, NULL, &_hKey, NULL)) == ERROR_SUCCESS) {
        return _hKey;
    }
    if (!access && setLastStatus(RegCreateKeyA(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS)) {
        return _hKey;
    }
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::connect(HKEY baseKey, const std::string &hostname)
{
    close();

    if (setLastStatus(RegConnectRegistryA(hostname.c_str(), baseKey, &_hKey)) == ERROR_SUCCESS)
        return _hKey;
    _hKey = NULL;
    return NULL;
}

HKEY RegKey::reset(HKEY baseKey, const std::string &subKeyName, const std::string &hostname, REGSAM access)
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

    return setLastStatus(RegCopyTreeA(hSrc, NULL, _hKey)) == ERROR_SUCCESS;
}

bool RegKey::rename(const std::string &newName)
{
    if (_hKey == NULL)
        return false;

    // 转换成 wstring
    wchar_t *newNameW = new wchar_t[newName.size() + 1];
    MultiByteToWideChar(CP_ACP, 0, newName.c_str(), -1, newNameW, int(newName.size()) + 1);
    bool success = setLastStatus(RegRenameKey(_hKey, NULL, newNameW) == ERROR_SUCCESS);
    delete[] newNameW;
    return success;
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
    if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, &info.type, NULL, &size)) != ERROR_SUCCESS) {
        if (success != NULL)
            *success = false;
        return info;
    }
    if (size > 0) {
        info.data.resize(size);
        if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, info.data.data(), &size)) != ERROR_SUCCESS) {
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
    if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &size)) == ERROR_SUCCESS)
        return size;
    else
        return -1;
}

ByteArray RegKey::getBinaryValue(const std::string &valueName, bool *success)
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return ByteArray();
    }

    DWORD valueSize = 0;
    if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) != ERROR_SUCCESS) {
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
    bool res = (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, value.data(), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;
    
    return value;
}

std::string RegKey::getStringValue(const std::string &valueName, bool *success)
{
    if (_hKey == NULL) {
        if (success != NULL)
            *success = false;
        return "";
    }
        
    DWORD valueSize = 0;
    DWORD type = 0;
    if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS
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
    bool res = (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    std::string value = res ? valueData : "";
    delete[] valueData;
    return value;
}

DWORD RegKey::getDwordValue(const std::string &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

QWORD RegKey::getQwordValue(const std::string &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return 0;
    }

    QWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(&value), &valueSize)) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

std::list<std::string> RegKey::getMultiStringValue(const std::string &valueName, bool *success)
{
    if (_hKey == NULL)
    {
        if (success != NULL)
            *success = false;
        return {};
    }

    DWORD valueSize = 0;
    DWORD type = REG_NONE;
    if (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, &valueSize)) != ERROR_SUCCESS
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
    bool res = (setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) == ERROR_SUCCESS);
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

std::list<ValueInfo> RegKey::getValues()
{
    if (_hKey == NULL)
        return {};

    DWORD maxName = 0, maxValue = 0;
    if (setLastStatus(
            RegQueryInfoKeyA(_hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxName, &maxValue, NULL, NULL)
            != ERROR_SUCCESS)) {
        return {};
    }
    maxName++;

    std::list<ValueInfo> values;
    char *valueName = new char[maxName];
    byte *valueData = new byte[maxValue];

    for (DWORD index = 0; ; index++) {
        ValueInfo valueInfo;
        DWORD valueNameSize = maxName;
        valueInfo.type = REG_NONE;
        DWORD valueSize = 0;
        if (setLastStatus(RegEnumValueA(_hKey, index, valueName, &valueNameSize, NULL,
                &valueInfo.type, NULL, &valueSize)) != ERROR_SUCCESS) {
            break;
        }
        valueInfo.name = valueName;

        if (valueSize > 0) {
            if (setLastStatus(RegQueryValueExA(_hKey, valueInfo.name.c_str(), NULL, NULL, LPBYTE(valueData), &valueSize)) != ERROR_SUCCESS) {
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

std::list<std::string> RegKey::getValueNames()
{
    if (_hKey == NULL)
        return std::list<std::string>();

    std::list<std::string> valueNames;
    char valueName[MAX_VALUE_NAME];
    DWORD valueNameSize;
    for (DWORD index = 0; ; index++) {
        valueNameSize = MAX_VALUE_NAME;
        if (setLastStatus(RegEnumValueA(_hKey, index, valueName, &valueNameSize, nullptr, nullptr, nullptr, nullptr))
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

    return setLastStatus(RegSetValueExA(_hKey, value.name.c_str(), 0,
        value.type, value.data.data(), DWORD(value.data.size()))) == ERROR_SUCCESS;
}

bool RegKey::setStringValue(const std::string &valueName, const std::string &value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExA(_hKey, valueName.c_str(), 0, type, 
        reinterpret_cast<const BYTE *>(value.c_str()), DWORD(value.size() + 1))) == ERROR_SUCCESS;
}

bool RegKey::setBinaryValue(const std::string &valueName, const void *value, size_t size, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        static_cast<const BYTE *>(value), DWORD(size))) == ERROR_SUCCESS;
}

bool RegKey::setDwordValue(const std::string &valueName, DWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(DWORD))) == ERROR_SUCCESS;
}

bool RegKey::setQwordValue(const std::string &valueName, QWORD value, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegSetValueExA(_hKey, valueName.c_str(), 0, type,
        reinterpret_cast<const BYTE*>(&value), sizeof(QWORD))) == ERROR_SUCCESS;
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
    bool res = (setLastStatus(RegSetValueExA(_hKey, valueName.c_str(), 0, type, LPBYTE(valueData), DWORD(size))) == ERROR_SUCCESS);
    delete[] valueData;

    return res;
}

int RegKey::putValues(const std::list<ValueInfo> &values)
{
    if (_hKey == NULL)
        return 0;

    int successCount = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        if (setLastStatus(RegSetValueExA(_hKey, it->name.c_str(), 0, it->type,
                it->data.data(), DWORD(it->data.size()))) == ERROR_SUCCESS) {
            successCount++;
        }
    }
    return successCount;
}

DWORD RegKey::getValueType(const std::string &valueName)
{
    if (_hKey == NULL)
        return REG_NONE;

    DWORD type = 0;
    setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, &type, NULL, NULL));
    return type;
}

bool RegKey::deleteKey()
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteTreeA(_hKey, NULL)) == ERROR_SUCCESS;
}

HKEY RegKey::openSubkey(const std::string &subkeyName, REGSAM access)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (access && setLastStatus(RegOpenKeyExA(_hKey, subkeyName.c_str(), 0, access, &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!access && setLastStatus(RegOpenKeyA(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

HKEY RegKey::createSubkey(const std::string &subkeyName, REGSAM access)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (access && setLastStatus(RegCreateKeyExA(_hKey, subkeyName.c_str(), 0, NULL,
                                      REG_OPTION_NON_VOLATILE, access, NULL, &hKey, NULL)) == ERROR_SUCCESS) {
        return hKey;
    }
    if (!access && setLastStatus(RegCreateKeyA(_hKey, subkeyName.c_str(), &hKey)) == ERROR_SUCCESS) {
        return hKey;
    }

    return NULL;
}

bool RegKey::deleteSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteTreeA(_hKey, subkeyName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::hasValue(const std::string &valueName)
{
    if (_hKey == NULL)
        return false;

    DWORD valueSize = 0;
    return setLastStatus(RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize)) == ERROR_SUCCESS;
}

bool RegKey::isWritable()
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (setLastStatus(RegOpenKeyExA(_hKey, NULL, 0, KEY_WRITE, &hKey)) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

bool RegKey::deleteValue(const std::string &valueName)
{
    if (_hKey == NULL)
        return false;

    return setLastStatus(RegDeleteValueA(_hKey, valueName.c_str())) == ERROR_SUCCESS;
}

bool RegKey::hasSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (setLastStatus(RegOpenKeyA(_hKey, subkeyName.c_str(), &hKey)) != ERROR_SUCCESS) {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

std::list<std::string> RegKey::getSubkeyNames()
{
    if (_hKey == NULL)
        return std::list<std::string>();

    std::list<std::string> subkeyNames;
    char subkeyName[MAX_VALUE_NAME];
    for (DWORD index = 0; ; index++) {
        if (setLastStatus(RegEnumKeyA(_hKey, index, subkeyName, MAX_VALUE_NAME)) != ERROR_SUCCESS)
            break;
        subkeyNames.push_back(subkeyName);
    }
    return subkeyNames;
}
