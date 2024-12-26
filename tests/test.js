const { RegKey, RegKeyAccess } = require('..')

// create a RegKey object for the HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall key
const key = new RegKey('HKCU/SOFTWARE/Microsoft/Windows/CurrentVersion/Uninstall', RegKeyAccess.Read)
// get names of all subkeys
const subKeys = key.getSubKeyNames().map(name => key.openSubKey(name))

const apps = {}
for (const subKey of subKeys) {
  const name = subKey.name
  // read all values of the subkey
  apps[name] = subKey.values().reduce((app, value) => {
    app[value.name] = value.value
    return app
  }, {})
}

console.log('Installed apps:\n', apps)
key.flush()

// create a new subkey
const myKey = key.createSubKey('MyKey')

// add new values
myKey.newValue('DisplayName', 'MyKey')
myKey.newValue('DisplayVersion', '1.0.0')
// myKey.newValue('Publisher', 'vvcoder')
// myKey.newValue('InstallDate', new Date().getDate())
// or
// myKey.setStringValue('DisplayName', 'MyKey')
// myKey.setStringValue('DisplayVersion', '1.0.0')
myKey.setStringValue('Publisher', 'vvcoder')
myKey.setDwordValue('InstallDate', new Date().getDate())

myKey.flush()
console.log('MyKey:', myKey.values())

// delete the key
myKey.deleteTree()
myKey.close()
key.deleteSubKey(myKey.name)