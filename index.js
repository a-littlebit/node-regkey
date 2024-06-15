const bindings = require("bindings")

let regkey

if (process.platform === "win32") {
  try {
    regkey = bindings('regkey')
  } catch (e) {
    const prebuildPath = path.join(__dirname,
                                  "prebuilds",
                                  `${ process.platform }-${ process.arch }`,
                                  `regkey.node`)
    if (fs.existsSync(prebuildPath)) {
      regkey = require(prebuildPath)
    } else {
      throw e
    }
  }
} else {
  regkey = {}
  console.error("regkey is only supported on Windows")
}

module.exports = regkey
