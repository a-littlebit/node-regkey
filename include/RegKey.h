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

  size_t getValue(const std::string &valueName, void *value, size_t size);

  size_t getValueSize(const std::string &valueName);

  std::string getStringValue(const std::string &valueName) const;

  DWORD getDwordValue(const std::string &valueName, bool *success = nullptr) const;

  QWORD getQwordValue(const std::string &valueName, bool *success = nullptr) const;

  std::list<ValueInfo> getValues() const;

  std::list<std::string> getValueNames() const;

  bool setStringValue(const std::string &valueName, const std::string &value);

  bool setBinaryValue(const std::string &valueName, const void* value, size_t size);

  bool setDwordValue(const std::string &valueName, DWORD value);

  bool setQwordValue(const std::string &valueName, QWORD value);

  bool setValue(const std::string &valueName, const void *value, size_t size, DWORD type);

  int applyValues(const std::list<ValueInfo> &values);

  DWORD getValueType(const std::string &valueName) const;

  bool deleteKey() const;

  HKEY openSubkey(const std::string &subkeyName);

  HKEY createSubkey(const std::string &subkeyName);

  bool deleteSubkey(const std::string &subkeyName);

  bool hasValue(const std::string &valueName) const;

  bool deleteValue(const std::string &valueName);

  bool hasSubkey(const std::string &subkeyName) const;

  std::list<std::string> getSubkeyNames() const;

private:
  HKEY _hKey;
};
