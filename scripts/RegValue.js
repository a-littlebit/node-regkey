const util = require('util')

const { throwRegKeyError } = require('./Error')

const RegValueType = {
  REG_SZ                          : 'REG_SZ',
  REG_EXPAND_SZ                   : 'REG_EXPAND_SZ',
  REG_BINARY                      : 'REG_BINARY',
  REG_DWORD                       : 'REG_DWORD',
  REG_DWORD_LITTLE_ENDIAN         : 'REG_DWORD_LITTLE_ENDIAN',
  REG_DWORD_BIG_ENDIAN            : 'REG_DWORD_BIG_ENDIAN',
  REG_QWORD                       : 'REG_QWORD',
  REG_QWORD_LITTLE_ENDIAN         : 'REG_QWORD_LITTLE_ENDIAN',
  REG_MULTI_SZ                    : 'REG_MULTI_SZ',
  REG_RESOURCE_LIST               : 'REG_RESOURCE_LIST',
  REG_FULL_RESOURCE_DESCRIPTOR    : 'REG_FULL_RESOURCE_DESCRIPTOR',
  REG_RESOURCE_REQUIREMENTS_LIST  : 'REG_RESOURCE_REQUIREMENTS_LIST',
  REG_NONE                        : 'REG_NONE'
}

class RegValue {
  constructor(key, name) {
    this.key = key
    this.name = name
  }

  get(resultType) {
    // if resultType is not specified, get the value type from the key
    switch (resultType || this.key.getValueType(this.name)) {
      case Number:
      case BigInt:
        const type = this.key.getValueType(this.name)

        switch (type) {
          case RegValueType.REG_DWORD:
            return resultType(this.key.getDwordValue(this.name))
          
          case RegValueType.REG_QWORD:
            return resultType(this.key.getQwordValue(this.name))
          
          case RegValueType.REG_SZ:
          case RegValueType.REG_EXPAND_SZ: {
            return parseFloat(this.key.getStringValue(this.name))
          }
            
          default:
            return NaN
        }
      
      case RegValueType.REG_DWORD:
        return this.key.getDwordValue(this.name)
      case RegValueType.REG_QWORD:
        return this.key.getQwordValue(this.name)

      case String:
      case RegValueType.REG_SZ:
      case RegValueType.REG_EXPAND_SZ:
        return this.key.getStringValue(this.name)
      
      case Array:
      case RegValueType.REG_MULTI_SZ:
        return this.key.getMultiStringValue(this.name)
      
      case Buffer:
        return this.key.getBinaryValue(this.name)
      
      default:
        if (resultType) {
          throwRegKeyError('Invalid result type', this.key, this.name, this.key.getLastError())
          return null
        } else {
          return this.key.getBinaryValue(this.name)
        }
    }
  }

  set(val, type) {
    switch (typeof val) {
      case 'number':
        if (type === RegValueType.REG_DWORD || (Number.isInteger(val) && val >= 0 && val <= 0xFFFFFFFF)) {
          return this.key.setDwordValue(this.name, val, type || RegValueType.REG_DWORD)
        } else if (type === RegValueType.REG_QWORD || (Number.isInteger(val) && val >= 0)) {
          return this.key.setQwordValue(this.name, val, type || RegValueType.REG_QWORD)
        } else {
          return this.key.setStringValue(this.name, val.toString(), type || RegValueType.REG_SZ)
        }
      case 'bigint':
        if (type === RegValueType.REG_DWORD) {
          return this.key.setDwordValue(this.name, Number(val))
        } else if (type === RegValueType.REG_SZ || type === RegValueType.REG_EXPAND_SZ) {
          return this.key.setStringValue(this.name, val.toString(), type)
        } else {
          return this.key.setQwordValue(this.name, val, type || RegValueType.REG_QWORD)
        }
      case 'string':
        return this.key.setStringValue(this.name, val, type || RegValueType.REG_SZ)
      case 'object':
        if (val instanceof Buffer) {
          return this.key.setBinaryValue(this.name, val, type || RegValueType.REG_BINARY)
        }
        if (Array.isArray(val)) {
          return this.key.setMultiStringValue(this.name, val, type || RegValueType.REG_MULTI_SZ)
        }
      default:
        throwRegKeyError('Invalid value type', this.key, this.name, this.key.getLastError())
        return false
    }
  }

  get type() {
    try {
      return this.key.getValueType(this.name)
    } catch {
      return RegValueType.REG_NONE
    }
  }

  get exists() {
    return this.key?.open && this.key.hasValue(this.name)
  }

  get value() {
    return this.get()
  }

  set value(val) {
    this.set(val)
  }

  get data() {
    return this.key.getBinaryValue(this.name)
  }

  set data(val) {
    this.set(val)
  }
  
  delete() {
    return this.key.deleteValue(this.name)
  }

  rename(newName) {
    if (newName === this.name) {
      return true
    }
    
    if (this.key.hasValue(newName)) {
      return false
    }

    if (this.key.setBinaryValue(newName, this.data, this.type)) {
      this.key.deleteValue(this.name)
      this.name = newName
      return true
    }
    return false
  }

  [util.inspect.custom](depth, options) {
    options.depth = depth
    return util.inspect({
      ...this,
      value: this.get(),
      type: this.type,
      exists: this.exists
    }, options)
  }
}

module.exports = {
  RegValue,
  RegValueType
}
