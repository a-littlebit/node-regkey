#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

typedef wchar_t Char;
typedef std::wstring String;
typedef unsigned long long QWORD;
typedef std::vector<byte> ByteArray;

#define STR(x) L##x
#define STRLEN(x) wcslen(x)
#define STRCOPY(x, x_size, y) wcscpy_s(x, x_size, y)

struct RegValue
{
    String name;
    DWORD type;
    ByteArray data;
};

class RegKey
{
public:
    RegKey(HKEY baseKey = NULL,
           const String &subKeyName = STR(""),
           const String &hostname = STR(""),
           REGSAM access = 0);

    ~RegKey()
    {
        Close();
    }

    RegKey(RegKey &&r)
    {
        _hKey = r.Detach();
        _lastStatus = r._lastStatus;
    }

    RegKey &operator=(RegKey &&r)
    {
        _hKey = r.Detach();
        _lastStatus = r._lastStatus;
        return *this;
    }

    RegKey(const RegKey &) = delete;

    RegKey &operator=(const RegKey &) = delete;

    HKEY Open(HKEY baseKey,
              const String &subKeyName,
              REGSAM access = 0);

    HKEY Create(HKEY baseKey,
                const String &subKeyName,
                REGSAM access = 0);

    HKEY Connect(const String &hostname,
                 HKEY baseKey);

    HKEY ConnectAndCreate(HKEY baseKey,
                          const String &subKeyName,
                          const String &hostname,
                          REGSAM access = 0);

    HKEY Attach(HKEY hKey);

    HKEY Detach()
    {
        return Attach(NULL);
    }

    bool IsValid() const
    {
        return _hKey != NULL;
    }

    bool Close();

    HKEY GetHandle() const
    {
        return _hKey;
    }

    LSTATUS GetLastStatus() const
    {
        return _lastStatus;
    }

    LSTATUS SetLastStatus(LSTATUS status)
    {
        return _lastStatus = status;
    }

    bool IsWritable();

    bool Flush();

    bool CopyTree(HKEY hSrc);

    bool Rename(const String &newName);

    HKEY OpenSubkey(const String &subkeyName,
                    REGSAM access = 0);

    HKEY CreateSubkey(const String &subkeyName,
                      REGSAM access = 0);

    bool DeleteTree();
    bool DeleteTree(const String &subkeyName);

    bool DeleteSubkey(const String &subkeyName);

    bool HasSubkey(const String &subkeyName);

    std::vector<String> GetSubkeyNames();

    RegValue GetValue(const String &valueName,
                      bool *success = nullptr);

    DWORD GetValueType(const String &valueName);

    DWORD GetValueSize(const String &valueName);

    ByteArray GetBinaryValue(const String &valueName,
                             bool *success = nullptr);

    String GetStringValue(const String &valueName,
                          bool *success = nullptr);

    DWORD GetDwordValue(const String &valueName,
                        bool *success = nullptr);

    QWORD GetQwordValue(const String &valueName,
                        bool *success = nullptr);

    std::vector<String> GetMultiStringValue(const String &valueName,
                                            bool *success = nullptr);

    std::vector<RegValue> GetValues();

    std::vector<String> GetValueNames();

    bool HasValue(const String &valueName);

    bool PutValue(const RegValue &value);

    size_t PutValues(const std::vector<RegValue> &values);

    bool SetStringValue(const String &valueName,
                        const String &value,
                        DWORD type = REG_SZ);

    bool SetBinaryValue(const String &valueName,
                        const void *value,
                        size_t size,
                        DWORD type = REG_BINARY);

    bool SetDwordValue(const String &valueName,
                       DWORD value,
                       DWORD type = REG_DWORD);

    bool SetQwordValue(const String &valueName,
                       QWORD value,
                       DWORD type = REG_QWORD);

    bool SetMultiStringValue(const String &valueName,
                             const std::vector<String> &values,
                             DWORD type = REG_MULTI_SZ);

    bool DeleteValue(const String &valueName);

private:
    HKEY _hKey;
    LSTATUS _lastStatus;
};
