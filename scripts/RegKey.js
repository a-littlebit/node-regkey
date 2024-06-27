const path = require("path")
const util = require('util');

const regkey = require("node-gyp-build")(path.join(__dirname, ".."))
const RegKey = regkey.RegKey
const { throwRegKeyError } = require("./Error")
const { RegValue } = require("./RegValue")

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
