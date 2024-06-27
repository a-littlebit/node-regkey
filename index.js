const RegKey = require('./scripts/RegKey')
const RegValue = require('./scripts/RegValue')
const Error = require('./scripts/Error')

module.exports = {
  ...RegKey,
  ...RegValue,
  ...Error
}
