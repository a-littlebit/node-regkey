import reg from '../../index.js'

const ms = reg.hkcu.openSubkey('Software/Microsoft')

if (!ms) {
  console.log('Opening HKCU/Software/Microsoft Failed!')
  process.exit(1)
}

console.log('Values of HKCU/Software/Microsoft:\n', ms.getValues())
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())

ms.close()

const testKey = reg.hkcu.createSubkey('Software/testKey')
if (!testKey) {
  console.log('Creating HKCU/Software/testKey Failed!')
  process.exit(1)
}

for (let i = 0; i < 5; i++) {
  testKey.setStringValue('testName' + i, 'testVal')
}

let values = testKey.getStringValues()
console.log('before editing: ', values)

values = values.map((v, i) => {
  v.data = v.data + i
  return v
})

testKey.applyValues(values)
console.log('after editing: ', testKey.getStringValues())

if (testKey.deleteKey()) {
  console.log('Delete HKCU/Software/testKey Success!')
} else {
  console.log('Delete HKCU/Software/testKey Failed!')
  console.warn('Try delete it manually!')
}
