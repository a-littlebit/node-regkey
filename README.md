# node-regkey

## Introduction

Provide fast access to Windows registry for Node.js using native Windows API.

## Install

```bash
npm install regkey
```

## Usage

#### Import a base key

```javascript
const { hkcu } = require('regkey')
```

#### Open an existing key

```javascript
try {
  const ms = hkcu.openSubKey('Software/Microsoft')
  ... // do something with ms
} catch (e) {
  if (e instanceof RegKeyError) {
    console.error('Error operating registry: ', e.lastError)
  } else {
    console.error(e.message)
  }
}
```

#### Create a new key

```javascript
const myKey = hkcu.createSubKey('Software/myKey')
```

If the key already exists, it will be directly opened.

You can also call the `RegKey` constructor to create a registry key.

```javascript
const { RegKey, RegKeyAccess } = require('regkey')

const key = new RegKey('HKEY_LOCAL_MACHINE/Software/MyApp')
// specify a remote host and access rights
const key = new RegKey('//MyPC/HKCU/Software/MyApp', RegKeyAccess.Read)
```

#### Work with access rights

The `RegAccessKey` is an enum that specifies the access rights of the key.

You can find the definition of the enum in `index.d.ts`.

To specify multiple access rights, use `|` to combine them
or put them into an array.

```javascript
const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp', RegKeyAccess.Read | RegKeyAccess.ia32)
//or
const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp', [RegKeyAccess.Read, RegKeyAccess.ia32])
```

#### Get the name of the subkeys

```javascript
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubKeyNames())
```

#### Close a key

When a `RegKey` object is released, it automatically closes the attached handle.
You can also call `close()` to manually release the handle.

```javascript
ms.close()
```

Use `RegKey.valid` to check if the key hasn't been closed yet.

```javascript
if (ms.valid) {
  ... // do something with ms
}
```

#### Read values

```javascript
const values = myKey.values()
for (const value of values) {
  console.log('name: ', value.name)
  console.log('type: ', value.type)
  console.log('value: ', value.value)
  console.log('data: ', value.data, '\n')
}
```

The `value` property reads the registry item according to its value type, while `data` property reads it as a buffer.

Assignments to both of them have the same effect.

You can also call `getStringValue` to directly get the value as a string.

```javascript
const value = myKey.getStringValue('some-value')
console.log(value)
```

Or you can use `get` function to specify the result type you expect.

```javascript
const value = myKey.value('some-value').get(String)
```

The type of `value` and return value of `get` function is hard to predict. If you are sure about the type you want, use `RegKey.getXxxValue` instead. They are faster and more predictable.

#### Setting values

```javascript
// directly set
myKey.setStringValue('myValName', 'myValData')
// through RegValue object
myKey.value('myValName').set('myValData')
// specify the type
myKey.value('myValName').set('myValData', RegValueType.REG_SZ)
```

#### Delete the key

```javascript
myKey.deleteTree() // clear values and subkeys
myKey.close() // close the handle (optional, key won't be actually deleted before closed)
parentKey.deleteSubKey(myKey.name) // delete the key
```
