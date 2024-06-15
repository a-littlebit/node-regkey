const build = require("node-gyp-build")

let regkey

if (process.platform === "win32") {
  regkey = build(__dirname)
} else {
  regkey = {}
  console.error("regkey is only supported on Windows")
}

module.exports = regkey
