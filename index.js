const RegKey = require('./lib/RegKey')
const RegValue = require('./lib/RegValue')
const Error = require('./lib/Error')

module.exports = {
  ...RegKey,
  ...RegValue,
  ...Error
}
