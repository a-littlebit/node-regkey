#include <Windows.h>
#include <string>
#include <list>
#include <vector>

typedef unsigned long long QWORD;
typedef std::vector<byte> ByteArray;

struct ValueInfo {
  std::wstring name;
  DWORD type;
  ByteArray data;
};

class RegKey
{
public:

  // 0 specifies use RegCreateKeyA
  RegKey(HKEY baseKey = NULL,
        const std::wstring &subKeyName = L"",
        const std::wstring &hostname = L"",
        REGSAM access = 0);

  ~RegKey() {
    close();
  }

  RegKey(const RegKey &) = delete;
  RegKey &operator=(const RegKey &) = delete;

  // old handle will be closed
  HKEY open(HKEY baseKey, const std::wstring &subKeyName, REGSAM access = 0);

  HKEY create(HKEY baseKey, const std::wstring &subKeyName, REGSAM access = 0);

  HKEY connect(HKEY baseKey, const std::wstring &hostname);

  HKEY reset(HKEY baseKey, const std::wstring &subKeyName, const std::wstring &hostname, REGSAM access = 0);

  HKEY attach(HKEY hKey);

  HKEY detach() {
    return attach(NULL);
  }

  bool isOpen() const {
    return _hKey != NULL;
  }

  void close();

  HKEY getHandle() const {
    return _hKey;
  }

  LSTATUS getLastStatus() const {
    return _lastStatus;
  }

  LSTATUS setLastStatus(LSTATUS status) {
    return _lastStatus = status;
  }

  bool copyTree(HKEY hSrc);

  bool rename(const std::wstring &newName);

  ValueInfo getValue(const std::wstring &valueName, bool *success = nullptr);

  // returns -1 if failed
  int getValueSize(const std::wstring &valueName);

  ByteArray getBinaryValue(const std::wstring &valueName, bool *success = nullptr);

  std::wstring getStringValue(const std::wstring &valueName, bool *success = nullptr);

  DWORD getDwordValue(const std::wstring &valueName, bool *success = nullptr);

  QWORD getQwordValue(const std::wstring &valueName, bool *success = nullptr);

  std::list<std::wstring> getMultiStringValue(const std::wstring &valueName, bool *success = nullptr);

  std::list<ValueInfo> getValues();

  std::list<std::wstring> getValueNames();

  bool putValue(const ValueInfo &value);

  bool setStringValue(const std::wstring &valueName, const std::wstring &value, DWORD type = REG_SZ);

  bool setBinaryValue(const std::wstring &valueName, const void* value, size_t size, DWORD type = REG_BINARY);

  bool setDwordValue(const std::wstring &valueName, DWORD value, DWORD type = REG_DWORD);

  bool setQwordValue(const std::wstring &valueName, QWORD value, DWORD type = REG_QWORD);

  bool setMultiStringValue(const std::wstring &valueName, const std::list<std::wstring> &value, DWORD type = REG_MULTI_SZ);

  int putValues(const std::list<ValueInfo> &values);

  // returns 0 if failed
  DWORD getValueType(const std::wstring &valueName);

  bool deleteKey();

  HKEY openSubkey(const std::wstring &subkeyName, REGSAM access = 0);

  HKEY createSubkey(const std::wstring &subkeyName, REGSAM access = 0);

  bool deleteSubkey(const std::wstring &subkeyName);

  bool hasValue(const std::wstring &valueName);

  bool isWritable();

  bool deleteValue(const std::wstring &valueName);

  bool hasSubkey(const std::wstring &subkeyName);

  std::list<std::wstring> getSubkeyNames();

private:
  HKEY _hKey;
  LSTATUS _lastStatus;
};
