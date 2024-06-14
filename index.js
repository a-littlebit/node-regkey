import bindings from "bindings"

let regkey

if (process.platform === "win32") {
  regkey = bindings("regkey")
} else {
  regkey = {}
  console.error("regkey is only supported on Windows")
}

export default regkey
