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

#### Get the name of the subkeys

```
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())
```

#### Get infomation of values

```
const values = ms.getValues()
for (const value of values) {
  console.log('name: ', value.name)
  console.log('type: ', value.type)
  console.log('data: ', value.data, '\n')
}
```

The data field defaults to buffer. To get string data, use getStringValues

```
const values = ms.getStringValues()
for (const value of values) {
  console.log('name: ', value.name)
  console.log('type: ', value.type)
  console.log('data: ', value.data, '\n')
}
```

Or you can use 'options' arg of function getValues

```
const values = ms.getValues({
  type: String,
  mapByName: true
})

Object.keys(values).forEach(name => {
  console.log('name: ', name)
  console.log('type: ', values[name].type)
  console.log('data: ', values[name].data, '\n')
})
```

The 'mapByName' option means to turn the result into a map from a key's name to the key

The 'type' option supports String, Number, Buffer and Array(for REG_MULTI_SZ)

#### Close a key

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

#### Setting values

```
// setting one
myKey.setStringValue('myValName', 'myValData')
// setting an array
myKey.applyValues([
  {
    name: 'val-1',
    data: 'a string' // As REG_SZ  
  },
  {
    name: 'val-2',
    data: 123 // As REG_QWORD
  }
])
```

Delete the key

```
if (!myKey.deleteKey()) {
  console.log('Delete HKCU/Software/myKey Failed!')
  console.warn('Try delete it manually!')
}
```
