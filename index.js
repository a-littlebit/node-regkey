const bindings = require("bindings")

let regkey

if (process.platform === "win32") {
  regkey = bindings("regkey")
} else {
  regkey = {}
  console.error("regkey is only supported on Windows")
}

module.exports = regkey
