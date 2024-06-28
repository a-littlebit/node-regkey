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

  // 0 specifies use RegCreateKeyA
  RegKey(HKEY baseKey = NULL,
        const std::string &subKeyName = "",
        const std::string &hostname = "",
        REGSAM access = 0);

  ~RegKey() {
    close();
  }

  RegKey(const RegKey &) = delete;
  RegKey &operator=(const RegKey &) = delete;

  // old handle will be closed
  HKEY open(HKEY baseKey, const std::string &subKeyName, REGSAM access = 0);

  HKEY create(HKEY baseKey, const std::string &subKeyName, REGSAM access = 0);

  HKEY connect(HKEY baseKey, const std::string &hostname);

  HKEY reset(HKEY baseKey, const std::string &subKeyName, const std::string &hostname, REGSAM access = 0);

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

  // returns -1 if failed
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

  // returns 0 if failed
  DWORD getValueType(const std::string &valueName) const;

  bool deleteKey() const;

  HKEY openSubkey(const std::string &subkeyName, REGSAM access = 0);

  HKEY createSubkey(const std::string &subkeyName, REGSAM access = 0);

  bool deleteSubkey(const std::string &subkeyName);

  bool hasValue(const std::string &valueName) const;

  bool isWritable() const;

  bool deleteValue(const std::string &valueName);

  bool hasSubkey(const std::string &subkeyName) const;

  std::list<std::string> getSubkeyNames() const;

private:
  HKEY _hKey;
};
