const reg = require('../../index.js')

const ms = reg.hkcu.openSubkey('Software/Microsoft')
if (!ms) {
  console.log('Microsoft registry key not found')
  process.exit(1)
}

console.log('ms subkeys:\n', ms.getSubkeyNames())
ms.close()

const testKey = reg.hkcu.createSubkey('Software/TestKey')
if (!testKey) {
  console.log('Failed to create TestKey')
  process.exit(1)
}

for (let i = 0; i < 10; i++) {
  testKey.setStringValue('test-str-' + i, 'test-val-' + i)
}

for (let i = 0; i < 10; i++) {
  testKey.setDwordValue('test-dword-' + i, 10 - i)
}

const newVal = testKey.newValue('newVal')
newVal.set('test-str-buffer', reg.RegValueType.REG_BINARY)

console.log(testKey.values())

reg.disableRegKeyErrors()
testKey.getStringValue('nonexisten')

if (!testKey.delete()) {
  console.log('Failed to delete TestKey')
  console.warn('Try delete it manually!')
}
