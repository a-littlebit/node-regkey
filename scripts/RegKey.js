const path = require("path")
const util = require('util');

const regkey = require("node-gyp-build")(path.join(__dirname, ".."))
const RegKey = regkey.RegKey
const { throwRegKeyError } = require("./Error")
const { RegValue } = require("./RegValue")

regkey.RegKeyAccess = {
  Default: 0,
  Read: 0x20019,
  Write: 0x20006,
  Execute: 0x20019,
  AllAccess: 0xF003F,

  KEY_QUERY_VALUE: 0x0001,
  KEY_SET_VALUE: 0x0002,
  KEY_CREATE_SUB_KEY: 0x0004,
  KEY_ENUMERATE_SUB_KEYS: 0x0008,
  KEY_NOTIFY: 0x0010,
  KEY_CREATE_LINK: 0x0020,
  KEY_WOW64_32KEY: 0x0200,
  KEY_WOW64_64KEY: 0x0100,
  KEY_WOW64_RES: 0x0300,

  STANDARD_RIGHTS_REQUIRED: 0x000F0000,
  STANDARD_RIGHTS_READ: 0x00020000,
  STANDARD_RIGHTS_WRITE: 0x00020000,
  STANDARD_RIGHTS_EXECUTE: 0x00020000,
  STANDARD_RIGHTS_ALL: 0x001F0000,

  KEY_READ: 0x20019,
  KEY_WRITE: 0x20006,
  KEY_EXECUTE: 0x20019,
  KEY_ALL_ACCESS: 0xF003F
}

// Provide access to throw a RegKeyError for native code
RegKey.prototype.__throwRegKeyError__ = throwRegKeyError

// Print basic information in console.log
RegKey.prototype[util.inspect.custom] = function (depth, options) {
  options.depth = depth
  return util.inspect({
    path: this.path,
    name: this.name,
    open: this.open,
    ...this
  }, options)
}

// get specific value object
RegKey.prototype.value = function getValue(name) {
  return new RegValue(this, name)
}

// get all value objects
RegKey.prototype.values = function getValues() {
  return this.getValueNames().map(name => new RegValue(this, name))
}

RegKey.prototype.newValue = function newValue(name, val, type) {
  const value = new RegValue(this, name)
  if (val === undefined || val === null) {
    val = Buffer.alloc(0)
  }
  if (value.set(val, type)) {
    return value
  }
  return null
}

module.exports = regkey
