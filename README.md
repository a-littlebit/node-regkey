# node-regkey

## Introduction

使用原生 Windows API 为 NodeJS 提供 Windows 注册表操作

Provide Windows registry key access for Node.js using the native Windows API.

## Install

```
npm install regkey
```

## Example

#### Import base keys

```
const { hkcu } = require('regkey')
```

Currently supported base keys: 

```
export const hkcr: RegKey; // HKEY_CLASS_ROOT
export const hkcu: RegKey; // HKEY_CURRENT_USER
export const hklm: RegKey; // HKEY_LOCAL_MACHINE
export const hku:  RegKey; // HKEY_USERS
export const hkcc: RegKey; // HKEY_CURRENT_CONFIG
export const hkpd: RegKey; // HKEY_PERFORMANCE_DATA
export const hkpt: RegKey; // HKEY_PERFORMANCE_TEXT
export const hkpn: RegKey; // HKEY_PERFORMALCE_NLSTEXT
```

#### Opening an existing key

```
const ms = hkcu.openSubkey('Software/Microsoft')
if (!ms) {
  // Failed
  console.log('Opening HKCU/Software/Microsoft Failed!')
  process.exit(1)
}
console.log(`Opening ${ ms.path } success`)
```

The function 'openSubkey' returns null if opening failed

#### Getting names of the subkeys

```
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())
```

The result is an array of string containing names of all the subkeys

#### Closing a key

The key will be automatically closed when the JavaScript object is  garbage coll

You can also close it manually

```
ms.close()
```

#### Creating a new key

```
const myKey = hkcu.createSubkey('Software/myKey')
```

If the key already exists, it will be directly opened

You can also call the RegKey constructor to create a registry key

```
const { RegKey } = require('regkey')
// specify full path
const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp')
// specify path parts
const key = new RegKey('HKEY_CURRENT_USER', 'Software', 'MyApp')
// specify a remote host
const key = new RegKey('//hostname', 'HKCU/Software/MyApp')
// specify a remote host and access rights
const key = new RegKey('//hostname/HKCU/Software/MyApp', RegKeyAccess.Read)
```

The RegAccessKey is an enum that specifies the access rights of the key

You can find the definition of the enum in index.d.ts

To specify multiple access rights, use bitwise OR to combine them

or put them in an array

```
const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp', RegKeyAccess.Read | RegKeyAccess.ia32)
//or
const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp', [RegKeyAccess.Read, RegKeyAccess.ia32])
```

For more details, see the [MSDN](https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights)

#### Reading values

```
const values = myKey.values()
for (const value of values) {
  console.log('name: ', value.name)
  console.log('type: ', value.type)
  console.log('value: ', value.value)
  console.log('data: ', value.data, '\n')
}
```

The value property reads the registry item according to its value type, while data reads it as a buffer

Assignments to both of them have the same effect

You can also call getStringValue to directly get the value as a string

```
const value = myKey.getStringValue('some-value')
console.log(value)
```

Or you can use 'get' function to specify the result type you expect

```
const value = myKey.value('some-value').get(String)
```

The type could be one of String, Buffer, Number, Array(for REG_MULTI_SZ)

If resultType was not specified, it will be determined by the type of the value

#### Handling an error

When necessary, a RegKey function may throw a RegKeyError:

```
const errorVal = myKey.getStringValue('A-nonexistent-value')
```

You may get an error like the following

```
RegKeyError: Failed to get value
    at Object.<anonymous> (path\to\your\source\index.js:4:21)
    at ... {
  key: RegKey {...},
  value: 'A-nonexistent-value',
  lastErrorCode: 203
}
```

The 'lastErrorCode' field is the value returned by GetLastError()

If you don't want to receive a RegKeyError even if a function failed, use disableRegKeyErrors

```
const { disableRegKeyErrors } = require('regkey')
disableRegKeyErrors()
```

#### Setting values

```
// directly set
myKey.setStringValue('myValName', 'myValData')
// through RegValue object
myKey.value('myValName').set('myValName', 'myValData')
```

You can specify a RegValueType after 'myValData'

If you do not do so, the type is determined by typeof 'myValData'

#### Delete the key

```
if (!myKey.delete()) {
  console.log('Delete HKCU/Software/myKey Failed!')
  console.warn('Try delete it manually!')
}
```
