#include "RegKey.h"
#include <codecvt>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

RegKey::RegKey(HKEY baseKey, const std::string &subKeyName)
{
    if (RegOpenKeyA(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS)
        return;

    _hKey = NULL;
}

HKEY RegKey::open(HKEY baseKey, const std::string &subKeyName)
{
    close();

    if (RegOpenKeyA(baseKey, subKeyName.c_str(), &_hKey) == ERROR_SUCCESS)
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

size_t RegKey::getValue(const std::string &valueName, void *value, size_t size)
{
    if (_hKey == NULL)
        return false;

    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, (LPBYTE)value, (LPDWORD)&size) == ERROR_SUCCESS) {
        return size;
    }
    return 0;
}

size_t RegKey::getValueSize(const std::string &valueName)
{
    if (_hKey == NULL)
        return 0;

    DWORD size = 0;
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &size) == ERROR_SUCCESS)
        return size;
    else
        return 0;
}

std::string RegKey::getStringValue(const std::string &valueName) const
{
    if (_hKey == NULL)
        return "";
        
    char value[MAX_VALUE_NAME];
    DWORD valueSize = sizeof(value);
    if (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, (LPBYTE)value, &valueSize) != ERROR_SUCCESS)
        return "";
    else
        return value;
}

DWORD RegKey::getDwordValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL)
        return 0;

    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, (LPBYTE)&value, &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

QWORD RegKey::getQwordValue(const std::string &valueName, bool *success) const
{
    if (_hKey == NULL)
        return 0;

    QWORD value = 0;
    DWORD valueSize = sizeof(value);
    bool res = (RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, (LPBYTE)&value, &valueSize) == ERROR_SUCCESS);
    if (success != NULL)
        *success = res;

    return value;
}

std::list<ValueInfo> RegKey::getValues() const
{
    if (_hKey == NULL)
        return {};

    std::list<ValueInfo> values;
    for (DWORD index = 0; ; index++) {
        ValueInfo valueInfo;
        char valueName[MAX_VALUE_NAME];
        DWORD valueNameSize = MAX_VALUE_NAME;
        DWORD valueSize = 0;
        if (RegEnumValueA(_hKey, index, valueName, &valueNameSize, NULL, NULL, NULL, &valueSize) != ERROR_SUCCESS)
            break;
        valueInfo.name = valueName;

        if (valueSize > 0) {
            byte* valueData = new byte[valueSize];
            DWORD type = REG_NONE;
            if (RegQueryValueExA(_hKey, valueName, NULL, &type, (LPBYTE)valueData, &valueSize) == ERROR_SUCCESS) {
                valueInfo.type = type;
                valueInfo.data.assign(valueData, valueData + valueSize);
            }
            delete[] valueData;
        }
        values.push_back(valueInfo);
    }
    return values;
}

std::list<std::string> RegKey::getValueNames() const
{
    if (_hKey == NULL)
        return std::list<std::string>();
        
    std::list<std::string> valueNames;
    for (DWORD index = 0; ; index++) {
        char valueName[MAX_VALUE_NAME];
        DWORD valueNameSize = MAX_VALUE_NAME;
        if (RegEnumValueA(_hKey, index, valueName, &valueNameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
            break;
        valueNames.push_back(valueName);
    }

    return valueNames;
}

bool RegKey::setStringValue(const std::string &valueName, const std::string &value)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, REG_SZ, (LPBYTE)value.c_str(), value.size() + 1) == ERROR_SUCCESS;
}

bool RegKey::setBinaryValue(const std::string &valueName, const void *value, size_t size)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, REG_BINARY, (LPBYTE)value, size) == ERROR_SUCCESS;
}

bool RegKey::setDwordValue(const std::string &valueName, DWORD value)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD)) == ERROR_SUCCESS;
}

bool RegKey::setQwordValue(const std::string &valueName, QWORD value)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, REG_QWORD, (LPBYTE)&value, sizeof(QWORD)) == ERROR_SUCCESS;
}

bool RegKey::setValue(const std::string &valueName, const void *value, size_t size, DWORD type)
{
    if (_hKey == NULL)
        return false;

    return RegSetValueExA(_hKey, valueName.c_str(), 0, type, (LPBYTE)value, size) == ERROR_SUCCESS;
}

int RegKey::applyValues(const std::list<ValueInfo> &values)
{
    if (_hKey == NULL)
        return 0;

    int successCount = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        const ValueInfo &valueInfo = *it;
        if (setValue(valueInfo.name, valueInfo.data.data(), valueInfo.data.size(), valueInfo.type)) {
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

HKEY RegKey::openSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (RegOpenKeyA(_hKey, subkeyName.c_str(), &hKey) == ERROR_SUCCESS)
        return hKey;

    return NULL;
}

HKEY RegKey::createSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return NULL;

    HKEY hKey = NULL;
    if (RegCreateKeyA(_hKey, subkeyName.c_str(), &hKey) == ERROR_SUCCESS)
        return hKey;

    return NULL;
}

bool RegKey::deleteSubkey(const std::string &subkeyName)
{
    if (_hKey == NULL)
        return false;

    return RegDeleteKeyA(_hKey, subkeyName.c_str()) == ERROR_SUCCESS;
}

bool RegKey::hasValue(const std::string &valueName) const
{
    if (_hKey == NULL)
        return false;

    DWORD valueSize = 0;
    return RegQueryValueExA(_hKey, valueName.c_str(), NULL, NULL, NULL, &valueSize) == ERROR_SUCCESS;
}

bool RegKey::isSubkeyWriteable(const std::string &valueName) const
{
    if (_hKey == NULL)
        return false;

    HKEY hKey = NULL;
    if (RegOpenKeyExA(_hKey, valueName.c_str(), 0, KEY_WRITE | KEY_WOW64_64KEY, &hKey) != ERROR_SUCCESS) {
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
    for (DWORD index = 0; ; index++) {
        char subkeyName[MAX_VALUE_NAME];
        if (RegEnumKeyA(_hKey, index, subkeyName, MAX_VALUE_NAME) != ERROR_SUCCESS)
            break;
        subkeyNames.push_back(subkeyName);
    }
    return subkeyNames;
}
