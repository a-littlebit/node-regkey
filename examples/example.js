import reg from '../../index.js'

const ms = reg.hkcu.openSubkey('Software/Microsoft')

if (!ms) {
  // Failed
  console.log('Opening HKCU/Software/Microsoft Failed!')
  process.exit(1)
}

// Reading values
console.log('Values of HKCU/Software/Microsoft:\n', ms.getValues())
// Reading Subkeys
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())

// Done
ms.close()

// Creating a new key
const myKey = reg.hkcu.createSubkey('Software/myKey')
if (!myKey) {
  console.log('Creating HKCU/Software/myKey Failed!')
  process.exit(1)
}

// Writing values
for (let i = 0; i < 5; i++) {
  myKey.setStringValue('myValue' + i, 'string value')
}

// Read All String Values
let values = myKey.getStringValues()
console.log('before editing: ', values)

values = values.map((v, i) => {
  v.data = v.data + ' ' + (i + 1)
  return v
})

// Write Multiple Values
myKey.applyValues(values)
console.log('after editing: ', myKey.getStringValues())

// Delete a registry key
if (myKey.deleteKey()) {
  console.log('Delete HKCU/Software/myKey Success!')
} else {
  console.log('Delete HKCU/Software/myKey Failed!')
  console.warn('Try delete it manually!')
}

// Auto close after the environment exit
// myKey.close()
