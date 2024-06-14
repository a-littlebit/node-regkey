import bindings from "bindings"

let reg

if (process.platform === "win32") {
  reg = bindings("node-reg")
} else {
  reg = {}
  console.error("node-reg is only supported on Windows")
}

export default reg
