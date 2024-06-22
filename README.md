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
export declare const hkcr: RegKey; // HKEY_CLASS_ROOT
export declare const hkcu: RegKey; // HKEY_CURRENT_USER
export declare const hklm: RegKey; // HKEY_LOCAL_MACHINE
export declare const hku:  RegKey; // HKEY_USERS
export declare const hkcc: RegKey; // HKEY_CURRENT_CONFIG
export declare const hkpd: RegKey; // HKEY_PERFORMANCE_DATA
export declare const hkpt: RegKey; // HKEY_PERFORMANCE_TEXT
export declare const hkpn: RegKey; // HKEY_PERFORMALCE_NLSTEXT
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

The function returns null if opening failed

You can also use the RegKey constructor to create a RegKey

```
const { RegKey } = require('regkey')
const ms = new RegKey('HKCU/Software/Microsoft')
// or
const ms = new RegKey('HKCU', 'Software', 'Microsoft')
```

#### Getting names of the subkeys

```
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())
```

#### Closing a key

The key will automatically close when the JavaScript object is released

You can also close it manually

```
ms.close()
```

#### Creating a subkey

```
const myKey = hkcu.createSubkey('Software/myKey')
```

If the key already exists, it will be directly opened

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

The value field reads the registry item as a string, and data reads it as a buffer

Assignments to both of them have the same effect

You can also call getStringValue to directly get the value string

```
const value = myKey.getStringValue('some-value')
console.log(value)
```

Or you can use 'get' function to specify the result type you expect

```
const value = myKey.value('some-value').get(String)
```

The type could be one of String, Buffer, Number, Array(for REG_MULTI_SZ)

If resultType was not specified, it will be decided by the type of the value

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

If you do not do so, the type is decided by typeof 'myValData'

#### Delete the key

```
if (!myKey.deleteKey()) {
  console.log('Delete HKCU/Software/myKey Failed!')
  console.warn('Try delete it manually!')
}
```
