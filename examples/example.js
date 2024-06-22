const { hkcu } = require('../../index.js')

// if the key already exists, it will be directly opened
const testKey = hkcu.createSubkey('Software/TestKey')
if (!testKey) {
  console.log('Failed to create TestKey')
  process.exit(1)
}

// directly setting values
for (let i = 0; i < 10; i++) {
  testKey.setStringValue('test-str-' + i, 'test-val-' + i)
}

for (let i = 0; i < 10; i++) {
  testKey.setDwordValue('test-dword-' + i, 10 - i)
}

// operating a value through a RegValue object
const newVal = testKey.newValue('newVal')
newVal.set('test-str-buffer', 'REG_BINARY')

console.log(testKey.values())

// delete the key
if (!testKey.delete()) {
  console.log('Failed to delete TestKey')
  console.warn('Try delete it manually!')
}
