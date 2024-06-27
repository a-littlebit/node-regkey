/**
 * Registry value types
 */
export declare enum RegValueType {
  REG_SZ                          = 'REG_SZ',
  REG_EXPAND_SZ                   = 'REG_EXPAND_SZ',
  REG_BINARY                      = 'REG_BINARY',
  REG_DWORD                       = 'REG_DWORD',
  REG_DWORD_LITTLE_ENDIAN         = 'REG_DWORD_LITTLE_ENDIAN',
  REG_DWORD_BIG_ENDIAN            = 'REG_DWORD_BIG_ENDIAN',
  REG_QWORD                       = 'REG_QWORD',
  REG_QWORD_LITTLE_ENDIAN         = 'REG_QWORD_LITTLE_ENDIAN',
  REG_MULTI_SZ                    = 'REG_MULTI_SZ',
  REG_RESOURCE_LIST               = 'REG_RESOURCE_LIST',
  REG_FULL_RESOURCE_DESCRIPTOR    = 'REG_FULL_RESOURCE_DESCRIPTOR',
  REG_RESOURCE_REQUIREMENTS_LIST  = 'REG_RESOURCE_REQUIREMENTS_LIST',
  REG_NONE                        = 'REG_NONE'
}

/**
 * RegKey class
 * An object that represents a registry key.
 */
export declare class RegKey {

  /**
   * Create a new RegKey object with the given path.
   * A native registry key will be created with the given path.
   * 
   * @param paths - The path to the registry key.
   * @example
   * // specify full path
   * const key = new RegKey('HKEY_CURRENT_USER/Software/MyApp')
   * // specify path parts
   * const key = new RegKey('HKEY_CURRENT_USER', 'Software', 'MyApp')
   * // combine paths
   * const key = new RegKey('HKEY_CURRENT_USER', 'Software/MyApp')
   */
  constructor(...paths: string[])

  /**
   * @readonly The full path to the registry key.
   */
  readonly path: string

  /**
   * The name of the registry key.
   * You can rename the key by setting this property.
   */
  name: string

  /**
   * @readonly The key is open or not.
   */
  readonly open: boolean
  
  /**
   * Copy the subkey tree from the source key to the current key.
   * 
   * @param src - The source key.
   * @returns - True if the copy is successful.
   */
  copy(src: RegKey): boolean

  /**
   * Close the registry key.
   * The key will be automatically closed when the Javascript object is garbage collected.
   */
  close(): void

  /**
   * Get a RegValue object with the given name.
   * 
   * @param name - The name of the value.
   */
  value(name: string): RegValue

  /**
   * Get all RegValue objects in the key.
   * 
   * @returns - An array containing all RegValue objects in the key.
   */
  values(): RegValue[]

  /**
   * Create a registry value with the given name and value.
   * and return a RegValue object related to it.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   * @returns - A RegValue object related to the created value.
   */
  newValue(name: string,
           val?: string | string[] | number | Buffer,
           type?: RegValueType): RegValue | null
           
  /**
   * Get the binary value of the given name.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - The binary value in a buffer.
   */
  getBinaryValue(name: string): Buffer

  /**
   * Get the string value of the given name.
   * The target value type must be REG_SZ or REG_EXPAND_SZ.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - The string value.
   */
  getStringValue(name: string): string

  /**
   * Get the multi-string value of the given name.
   * The target value type must be REG_MULTI_SZ.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - An array containing the string values.
   */
  getMultiStringValue(name: string): string[]

  /**
   * Get the DWORD value of the given name.
   * The target value type must be REG_DWORD or REG_DWORD_LITTLE_ENDIAN.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - A number equaling to the DWORD value.
   */
  getDwordValue(name: string): number

  /**
   * Get the QWORD value of the given name.
   * The target value type must be REG_QWORD or REG_QWORD_LITTLE_ENDIAN.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - A number equaling to the QWORD value.
   */
  getQwordValue(name: string): number

  /**
   * Get the type of the value of the given name.
   * If failed, the function may throw a RegKeyError.
   * 
   * @param name - The name of the value.
   * @returns - The type of the value. It is a value in enum RegValueType.
   */
  getValueType(name: string): RegValueType

  /**
   * Check if the key has the given value.
   * 
   * @param name - The name of the value.
   * @returns - True if the key has the given value.
   */
  hasValue(name: string): boolean

  /**
   * Get all value names in the key.
   * 
   * @returns - An array containing all value names.
   */
  getValueNames(): string[]

  /**
   * Set the binary value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_BINARY.
   * @returns - True if the value is set successfully.
   */
  setBinaryValue(name: string, val: Buffer): boolean

  /**
   * Set the string value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_SZ.
   * @returns - True if the value is set successfully.
   */
  setStringValue(name: string, val: string, type?: RegValueType): boolean

  /**
   * Set the multi-string value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_MULTI_SZ.
   * @returns - True if the value is set successfully.
   */
  setMultiStringValue(name: string, val: string[], type?: RegValueType): boolean

  /**
   * Set the DWORD value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_DWORD.
   * @returns - True if the value is set successfully.
   */
  setDwordValue(name: string, val: number, type?: RegValueType): boolean

  /**
   * Set the QWORD value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_QWORD.
   * @returns - True if the value is set successfully.
   */
  setQwordValue(name: string, val: number, type?: RegValueType): boolean

  /**
   * Delete the value of the given name.
   * 
   * @param name - The name of the value.
   * @returns - True if the value is deleted successfully.
   */
  deleteValue(name: string): boolean

  /**
   * Delete the key.
   * 
   * @returns - True if the key is deleted successfully.
   */
  delete(): boolean

  /**
   * Open the subkey of the given name.
   * If failed, the function will return null.
   * 
   * @param name - The name of the subkey.
   * @returns - A RegKey object related to the subkey.
   */
  openSubkey(name: string): RegKey | null

  /**
   * Create the subkey of the given name.
   * If failed, the function will return null.
   * 
   * @param name - The name of the subkey.
   * @returns - A RegKey object related to the subkey.
   */
  createSubkey(name: string): RegKey | null

  /**
   * Delete the subkey of the given name.
   * 
   * @param name - The name of the subkey.
   * @returns - True if the subkey is deleted successfully.
   */
  deleteSubkey(name: string): boolean

  /**
   * Get all subkey names in the key.
   * 
   * @returns - An array containing all subkey names.
   */
  getSubkeyNames(): string[]

  /**
   * Check if the key has the given subkey.
   * 
   * @param name - The name of the subkey.
   * @returns - True if the key has the given subkey.
   */
  hasSubkey(name: string): boolean

  /**
   * Check if the key is writable.
   * 
   * @returns - True if the key is writable.
   */
  isWritable(): boolean
}

/**
 * RegValue class
 * An object representing a registry value.
 */
export declare class RegValue {

  /**
   * Create a RegValue object from a RegKey object and the specified value name.
   * 
   * @param key - The RegKey object.
   * @param name - The name of the value.
   */
  constructor(key: RegKey, name: string)

  /**
   * The RegKey object.
   */
  key: RegKey

  /**
   * The name of the value.
   */
  name: string

  /**
   * @readonly - The type of the value. It is a value in enum RegValueType.
   *           - If the value does not exist, the type will be REG_NONE.
   */
  readonly type: RegValueType

  /**
   * @readonly True if the value exists.
   */
  readonly exists: boolean

  /**
   * - The value. Setting this property will change the value.
   * - The type of this property is determined by the type of the value.
   */
  value: string | string[] | number | Buffer
  
  /**
   * - The raw data of the value.
   * - Setting this property will change the value. The effect is the same as setting the value property.
   * - The type of this property is Buffer.
   */
  data: Buffer

  /**
   * Get the value. The function may throw a RegKeyError.
   * 
   * @param resultType - The type of the value. It must be one of Number, String, Array, Buffer.
   *                     If not specified, the type will be inferred from the value.
   * @returns - The value with specified type.
   */
  get(resultType?: Function): string | string[] | number | Buffer

  /**
   * Set the value. The function may throw a RegKeyError.
   * 
   * @param val - The value.
   * @param type - The type of the value. It is a value in enum RegValueType.
   *               If not specified, the type will be inferred from 'val' param.
   * @returns - True if the value is set successfully.
   */
  set(val: string | string[] | number | Buffer, type?: RegValueType): boolean

  /**
   * Delete the value.
   * 
   * @returns - True if the value is deleted successfully.
   */
  delete(): boolean

  /**
   * Rename the value.
   * 
   * @param newName - The new name of the value.
   * @returns - True if the value is renamed successfully.
   */
  rename(newName: string): boolean
}

/**
 * RegKeyError class
 * An error occurred when accessing a registry key or value.
 */
export declare class RegKeyError extends Error {

  /**
   * Create a RegKeyError object.
   * 
   * @param message - The error message.
   * @param key - The RegKey object.
   * @param value - The name of the value.
   * @param lastErrorCode - The last error code. Call getLastError() to get it.
   */
  constructor(message: string,
              key: RegKey,
              value: string,
              lastErrorCode: number)

  /**
   * The RegKey object.
   */
  key: RegKey

  /**
   * The name of the value.
   */
  value: string

  /**
   * The last error code.
   */
  lastErrorCode: number
}

/**
 * Throw a RegKeyError if disabledRegKeyErrors() was not called.
 * 
 * @param message - The error message.
 * @param key - The RegKey object.
 * @param value - The name of the value.
 * @param lastErrorCode - The last error code. Call getLastError() to get it.
 */
export declare function throwRegKeyError(message: string,
                                          key: RegKey,
                                          value: string,
                                          lastErrorCode: number): void
                                                                                 
/**
 * Disable RegKeyError.
 * If disabled, throwRegKeyError() will not throw an error.
 * 
 * @param disabled - If not set to false, RegKeyError will be disabled.
 */
export declare function disableRegKeyErrors(disabled?: boolean): void

/**
 * Get the last error code.
 * It is a wrapper of GetLastError() function in Windows API.
 * 
 * @returns - The last error code.
 */
export declare function getLastError(): number

/**
 * A RegKey object related to HKEY_CLASS_ROOT.
 */
export declare const hkcr: RegKey

/**
 * A RegKey object related to HKEY_CURRENT_USER.
 */
export declare const hkcu: RegKey

/**
 * A RegKey object related to HKEY_LOCAL_MACHINE.
 */
export declare const hklm: RegKey

/**
 * A RegKey object related to HKEY_USERS.
 */
export declare const hku: RegKey

/**
 * A RegKey object related to HKEY_CURRENT_CONFIG.
 */
export declare const hkcc: RegKey

/**
 * A RegKey object related to HKEY_PERFORMANCE_DATA.
 */
export declare const hkpd: RegKey

/**
 * A RegKey object related to HKEY_PERFORMANCE_TEXT.
 */
export declare const hkpt: RegKey

/**
 * A RegKey object related to HKEY_PERFORMANCE_NLSTEXT.
 */
export declare const hkpn: RegKey
