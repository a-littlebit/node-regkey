const bindings = require("bindings")
const path = require("path")
const fs = require("fs")
const { execSync } = require("child_process")

const moduleName = 'regkey'

try {
  bindings(moduleName)
} catch {

  const prebuildPath = path.join(__dirname,
                                "prebuilds",
                                `${ process.platform }-${ process.arch }`,
                                `${ moduleName }.node`)
                                
  if (fs.existsSync(prebuildPath)) {
    
    fs.mkdirSync(path.join(__dirname, 'build', 'Release'), { recursive: true })
    fs.copyFileSync(prebuildPath, path.join(__dirname, 'build', 'Release', `${moduleName}.node`))

  } else {

    try {

      console.log(`Prebuild for ${ moduleName } not found, building...`)
      execSync(`npm run build:release`)
      console.log(`Build complete`)

    } catch (e) {
      console.error(e)
      throw new Error(`Failed to build mudule ${ moduleName }`)
    }

  }
}
