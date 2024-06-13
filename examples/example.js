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
const nodeReg = reg.hkcu.createSubkey('Software/Node-Reg')
if (!nodeReg) {
  console.log('Creating HKCU/Software/Node-Reg Failed!')
  process.exit(1)
}

// Writing values
for (let i = 0; i < 5; i++) {
  nodeReg.setStringValue('testName' + i, 'testVal')
}

// Read All String Values
let values = nodeReg.getStringValues()
console.log('before editing: ', values)

values = values.map((v, i) => {
  v.data = v.data + i
  return v
})

// Write Multiple Values
nodeReg.applyValues(values)
console.log('after editing: ', nodeReg.getStringValues())

// Delete a registry key
if (nodeReg.deleteKey()) {
  console.log('Delete HKCU/Software/Node-Reg Success!')
} else {
  console.log('Delete HKCU/Software/Node-Reg Failed!')
  console.warn('Try delete it manually!')
}

// Auto close after the environment exit
// nodeReg.close()
