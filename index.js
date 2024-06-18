const build = require("node-gyp-build")

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

let regkey

if (process.platform === "win32") {
  regkey = build(__dirname)

  const RegKey = regkey.RegKey

  RegKey.prototype.getValue = function getValue(name, options) {
    if (!options) {
      options = {}
    }

    options.type = options.type || Buffer

    const value = {
      name,
      type: this.getValueType(name)
    }
    switch (options.type) {
      case String:
        value.data = this.getStringValue(name)
        break
      case Number:
        value.data = this.getNumberValue(name)
        break
      case Array:
        value.data = this.getMultiStringValue(name)
        break
      default:
        value.data = this.getBufferValue(name)
        break
    }

    return value
  }

  RegKey.prototype.getValues = function getValues(options) {
    if (!options) {
      options = {}
    }

    options.type = options.type || Buffer
    options.mapByName = options.mapByName || false
    let values

    switch (options.type) {
      case String: {
        values = this.getStringValues()
        break
      }
        
      case Number: {
        values = this.getNumberValues()
        break
      }
        
      case Array: {
        values = this.getMultiStringValues()
        break
      }
        
      default: {
        values = this.getBufferValues()
        break
      }
    }
    
    if (options.mapByName) {
      values = values.reduce((acc, value) => {
        acc[value.name] = value
        return acc
      }, {})
    }
    return values
  }

} else {
  regkey = {}
  console.error("regkey is only supported on Windows")
}

module.exports = {
  ...regkey,
  RegValueType
}
