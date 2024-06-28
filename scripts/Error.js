class RegKeyError extends Error {
  constructor(message, key, value, lastError) {
    super(message)
    this.name = 'RegKeyError'
    this.key = key
    this.path = key.path
    this.value = value
    this.lastError = lastError

    Error.captureStackTrace(this, RegKeyError)
  }
}

let regkeyErrorEnabled = true

module.exports = {
  RegKeyError,
  disableRegKeyErrors(disabled) {
    regkeyErrorEnabled = (disabled === false) ? true : false
  },
  throwRegKeyError(message, key, value, lastError) {
    if (regkeyErrorEnabled) {
      throw new RegKeyError(message, key, value, lastError)
    }
  }
}
