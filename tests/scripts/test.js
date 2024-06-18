const reg = require('../../index.js')

const ms = new reg.RegKey('HKCU', 'Software/Microsoft')

if (!ms) {
  console.log('Opening HKCU/Software/Microsoft Failed!')
  process.exit(1)
}

console.log('Values of HKCU/Software/Microsoft:\n', ms.getBufferValues())
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())

ms.close()

const testKey = reg.hkcu.createSubkey('Software/testKey')
if (!testKey) {
  console.log('Creating HKCU/Software/testKey Failed!')
  process.exit(1)
}

for (let i = 0; i < 5; i++) {
  testKey.setBufferValue('testName' + i, Buffer.from('testVal'), reg.RegValueType.REG_SZ)
}

const values = testKey.getValues({
  type: String,
  mapByName: true
})
console.log('before editing: ', values)

Object.keys(values).forEach((v, i) => {
  values[v].data = values[v].data + i
})

values['test'] = { name: 'testStringList', data: ['testS1', 'testS2', 'testS3'] }

testKey.putValues(values)
console.log('after editing: ', testKey.getStringValues())
console.log('testStringList: ', testKey.getMultiStringValue('testStringList'))

if (testKey.deleteKey()) {
  console.log('Delete HKCU/Software/testKey Success!')
} else {
  console.log('Delete HKCU/Software/testKey Failed!')
  console.warn('Try delete it manually!')
}
