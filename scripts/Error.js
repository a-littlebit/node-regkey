class RegKeyError extends Error {
  constructor(message, key, value, lastErrorCode) {
    super(message)
    this.name = 'RegKeyError'
    this.key = key
    this.path = key.path
    this.value = value
    this.lastErrorCode = lastErrorCode

    Error.captureStackTrace(this, RegKeyError)
  }
}

let regkeyErrorEnabled = true

module.exports = {
  disableRegKeyErrors(disabled) {
    regkeyErrorEnabled = (disabled === false) ? true : false
  },
  throwRegKeyError(message, key, value, lastErrorCode) {
    if (regkeyErrorEnabled) {
      throw new RegKeyError(message, key, value, lastErrorCode)
    }
  }
}
