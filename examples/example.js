const { RegKey, disableRegKeyErrors, RegKeyAccess } = require('..')

// never throw errors in any RegKey function
disableRegKeyErrors()

// create a RegKey object for the HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall key
const key = new RegKey('HKCU/SOFTWARE/Microsoft/Windows/CurrentVersion/Uninstall', RegKeyAccess.Read)
// get names of all subkeys
const subKeys = key.getSubkeyNames().map(name => key.openSubkey(name))

const apps = {}
for (const subkey of subKeys) {
  const name = subkey.name
  // read all values of the subkey
  apps[name] = subkey.values().reduce((app, value) => {
    app[value.name] = value.value
    return app
  }, {})
}

console.log('Installed apps:\n', apps)

// create a new subkey
const myKey = key.createSubkey('MyKey')

// check if the key was created successfully
if (!myKey) {
  console.log('Failed to create MyKey')
  process.exit(1)
}

// add new values
myKey.newValue('DisplayName', 'MyKey')
myKey.newValue('DisplayVersion', '1.0.0')
myKey.newValue('Publisher', 'vvcoder')
myKey.newValue('InstallDate', new Date().getDate())
// or
// myKey.setStringValue('DisplayName', 'MyKey')
// myKey.setStringValue('DisplayVersion', '1.0.0')
// myKey.setStringValue('Publisher', 'vvcoder')
// myKey.setDwordValue('InstallDate', new Date().getDate())

console.log('MyKey:', myKey.values())

// delete the key
if (myKey.delete()) {
  console.log('MyKey deleted')
} else {
  console.log('Failed to delete MyKey')
  console.warn('Try delete it manually!')
  process.exit(1)
}
