const path = require("path")
const util = require('util')

const regkey = process.platform == 'win32' ?
  require('node-gyp-build')(path.resolve(__dirname, '..')) : 
  { RegKey: () => { } }
const RegKey = regkey.RegKey
const { throwRegKeyError } = require("./Error")
const { RegValue } = require("./RegValue")

// Provide access to throw a RegKeyError for native code
RegKey.prototype.__throwRegKeyError__ = throwRegKeyError

// Inspect properties
RegKey.prototype[util.inspect.custom] = function (depth, options) {
  options.depth = depth
  return util.inspect({
    path: this.path,
    name: this.name,
    valid: this.valid,
    lastStatus: this.lastStatus,
    ...this
  }, options)
}

RegKey.prototype.value = function value(name) {
  return new RegValue(this, name)
}

RegKey.prototype.values = function values() {
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
