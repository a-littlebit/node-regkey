import reg from '../../index.js'

const ms = reg.hkcu.openSubkey('Software/Microsoft')

if (!ms) {
  console.log('Opening HKCU/Software/Microsoft Failed!')
  process.exit(1)
}

console.log('Values of HKCU/Software/Microsoft:\n', ms.getValues())
console.log('Subkeys of HKCU/Software/Microsoft:\n', ms.getSubkeyNames())

ms.close()

const nodeReg = reg.hkcu.createSubkey('Software/Node-Reg')
if (!nodeReg) {
  console.log('Creating HKCU/Software/Node-Reg Failed!')
  process.exit(1)
}

for (let i = 0; i < 5; i++) {
  nodeReg.setStringValue('testName' + i, 'testVal')
}

let values = nodeReg.getStringValues()
console.log('before editing: ', values)

values = values.map((v, i) => {
  v.data = v.data + i
  return v
})

nodeReg.applyValues(values)
console.log('after editing: ', nodeReg.getStringValues())

if (nodeReg.deleteKey()) {
  console.log('Delete HKCU/Software/Node-Reg Success!')
} else {
  console.log('Delete HKCU/Software/Node-Reg Failed!')
  console.warn('Try delete it manually!')
}
