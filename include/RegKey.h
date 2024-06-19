#include <Windows.h>
#include <string>
#include <list>
#include <vector>

typedef unsigned long long QWORD;
typedef std::vector<byte> ByteArray;

struct ValueInfo {
  std::string name;
  DWORD type;
  ByteArray data;
};

class RegKey
{
public:
  RegKey(HKEY hKey = NULL) : _hKey(hKey) {}

  RegKey(HKEY baseKey, const std::string &subKeyName);

  ~RegKey() {
    close();
  }

  RegKey(const RegKey &) = delete;
  RegKey &operator=(const RegKey &) = delete;

  HKEY open(HKEY baseKey, const std::string &subKeyName);

  HKEY create(HKEY baseKey, const std::string &subKeyName);

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

  bool copyTree(HKEY hSrc);

  bool rename(const std::string &newName);

  ValueInfo getValue(const std::string &valueName, bool *success = nullptr);

  int getValueSize(const std::string &valueName);

  ByteArray getBinaryValue(const std::string &valueName, bool *success = nullptr) const;

  std::string getStringValue(const std::string &valueName, bool *success = nullptr) const;

  DWORD getDwordValue(const std::string &valueName, bool *success = nullptr) const;

  QWORD getQwordValue(const std::string &valueName, bool *success = nullptr) const;

  std::list<std::string> getMultiStringValue(const std::string &valueName, bool *success = nullptr) const;

  std::list<ValueInfo> getValues() const;

  std::list<std::string> getValueNames() const;

  bool putValue(const ValueInfo &value);

  bool setStringValue(const std::string &valueName, const std::string &value, DWORD type = REG_SZ);

  bool setBinaryValue(const std::string &valueName, const void* value, size_t size, DWORD type = REG_BINARY);

  bool setDwordValue(const std::string &valueName, DWORD value, DWORD type = REG_DWORD);

  bool setQwordValue(const std::string &valueName, QWORD value, DWORD type = REG_QWORD);

  bool setMultiStringValue(const std::string &valueName, const std::list<std::string> &value, DWORD type = REG_MULTI_SZ);

  int putValues(const std::list<ValueInfo> &values);

  DWORD getValueType(const std::string &valueName) const;

  bool deleteKey() const;

  HKEY openSubkey(const std::string &subkeyName);

  HKEY createSubkey(const std::string &subkeyName);

  bool deleteSubkey(const std::string &subkeyName);

  bool hasValue(const std::string &valueName) const;

  bool isSubkeyWriteable(const std::string &valueName) const;

  bool deleteValue(const std::string &valueName);

  bool hasSubkey(const std::string &subkeyName) const;

  std::list<std::string> getSubkeyNames() const;

private:
  HKEY _hKey;
};
