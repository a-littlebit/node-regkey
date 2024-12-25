/**
 * Registry key access rights
 * - See https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
 */
export declare enum RegKeyAccess {
  Default = 0,
  Read = 0x20019,
  Write = 0x20006,
  Execute = 0x20019,
  AllAccess = 0xf003f,

  /**
   * Access 32-bit registry keys on 64-bit systems
   */
  ia32 = 0x0200,

  /**
   * Access 64-bit registry keys on 64-bit systems
   */
  x64 = 0x0100,

  KEY_QUERY_VALUE = 0x0001,
  KEY_SET_VALUE = 0x0002,
  KEY_CREATE_SUB_KEY = 0x0004,
  KEY_ENUMERATE_SUB_KEYS = 0x0008,
  KEY_NOTIFY = 0x0010,
  KEY_CREATE_LINK = 0x0020,
  KEY_WOW64_32KEY = 0x0200,
  KEY_WOW64_64KEY = 0x0100,
  KEY_WOW64_RES = 0x0300,

  STANDARD_RIGHTS_REQUIRED = 0x000F0000,
  STANDARD_RIGHTS_READ = 0x00020000,
  STANDARD_RIGHTS_WRITE = 0x00020000,
  STANDARD_RIGHTS_EXECUTE = 0x00020000,
  STANDARD_RIGHTS_ALL = 0x001F0000,

  KEY_READ = 0x20019,
  KEY_WRITE = 0x20006,
  KEY_EXECUTE = 0x20019,
  KEY_ALL_ACCESS = 0xF003F
}


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

export declare interface RegKeyOptions {
  /**
   * The host name of the remote registry key.
   * If not specified, the key is local.
   */
  host?: string


  /**
   * The base key of the registry key.
   * 
   * @example
   * 'HKEY_LOCAL_MACHINE'
   * 'HKEY_CURRENT_USER'
   */
  baseKey: string

  /**
   * The subkey of the registry key.
   * 
   * @example
   * 'Software/MyApp'
   */
  subkey?: string

  /**
   * The desired access rights.
   */
  access?: RegKeyAccess | RegKeyAccess[]
}

/**
 * RegKey class
 * An object that represents a registry key.
 */
export declare class RegKey {

  /**
   * Create a new registry key with the given options.
   * 
   * @param options - The options for the registry key.
   */
  constructor(options: RegKeyOptions)

  /**
   * Create a new registry key with the given path.
   * 
   * @param path - The path to the registry key.
   * @param access - The desired access rights.
   */
  constructor(path: string, access: RegKeyAccess | RegKeyAccess[])

  /**
   * @readonly The full path to the registry key.
   */
  readonly path: string

  /**
   * @readonly The hostname of the remote registry key.
   *           If the key is local, this property will be empty.
   */
  readonly host: string

  /**
   * The name of the registry key.
   * You can rename the key by setting this property.
   */
  name: string

  /**
   * @readonly The key is valid or not.
   */
  readonly valid: boolean

  /**
   * Get or set the status code of the last API call.
   */
  lastStatus: number

  /**
   * Check if the key is writable.
   * 
   * @returns True if the key is writable.
   */
  isWritable(): boolean

  /**
   * Flushes the key to disk.
   * 
   * @returns True if the key was flushed.
   * @throws {RegistryError} if failed.
   */
  flush(): boolean

  /**
   * Get the last error message
   * 
   * 
   * @returns An error message formatted by the Windows API from the error code of last API call
   */
  getLastError(): string
  
  /**
   * Copy the subkey tree from the source key to the current key.
   * 
   * @param src - The source key.
   * @returns True if the copy is successful.
   */
  copyTree(src: RegKey): boolean

  /**
   * Close the registry key.
   * The key will be automatically closed when the Javascript object is garbage collected.
   */
  close(): void

  /**
   * Delete all subkeys and values of the specified key.
   * 
   * @param subkey - The specified subkey. Ommit it to specify the current key.
   * @returns True if deleted successfully.
   */
  deleteTree(subkey?: string): boolean

  /**
   * Open the subkey of the given name.
   * If failed, the function will return null.
   * 
   * @param name - The name of the subkey.
   * @param access - The desired access rights.
   * @returns A RegKey object related to the subkey.
   */
  openSubkey(name: string, access?: RegKeyAccess | RegKeyAccess[]): RegKey | null

  /**
   * Create the subkey of the given name.
   * If failed, the function will return null.
   * 
   * @param name - The name of the subkey.
   * @param access - The desired access rights
   * @returns A RegKey object related to the subkey.
   */
  createSubkey(name: string, access?: RegKeyAccess | RegKeyAccess[]): RegKey | null

  /**
   * Delete the subkey of the given name.
   * 
   * @param name - The name of the subkey.
   * @returns True if the subkey is deleted successfully.
   */
  deleteSubkey(name: string): boolean

  /**
   * Get all subkey names in the key.
   * 
   * @returns An array containing all subkey names.
   */
  getSubkeyNames(): string[]

  /**
   * Check if the key has the given subkey.
   * 
   * @param name - The name of the subkey.
   * @returns True if the key has the given subkey.
   */
  hasSubkey(name: string): boolean

  /**
   * Get a RegValue object with the given name.
   * 
   * @param name - The name of the value.
   */
  value(name: string): RegValue

  /**
   * Get all RegValue objects in the key.
   * 
   * @returns An array containing all RegValue objects in the key.
   */
  values(): RegValue[]

  /**
   * Create a registry value with the given name and value.
   * and return a RegValue object related to it.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   * @returns A RegValue object related to the created value.
   */
  newValue(name: string,
           val?: string | string[] | number | bigint | Buffer,
           type?: RegValueType): RegValue | null
           
  /**
   * Get the binary value of the given name.
   * 
   * @param name - The name of the value.
   * @returns The binary value in a buffer.
   * @throws {RegKeyError} if failed.
   */
  getBinaryValue(name: string): Buffer

  /**
   * Get the string value of the given name.
   * The target value type must be REG_SZ or REG_EXPAND_SZ.
   * 
   * @param name - The name of the value.
   * @returns The string value.
   * @throws {RegKeyError} if failed.
   */
  getStringValue(name: string): string

  /**
   * Get the multi-string value of the given name.
   * The target value type must be REG_MULTI_SZ.
   * 
   * @param name - The name of the value.
   * @returns An array containing the string values.
   * @throws {RegKeyError} if failed.
   */
  getMultiStringValue(name: string): string[]

  /**
   * Get the DWORD value of the given name.
   * The target value type must be REG_DWORD or REG_DWORD_LITTLE_ENDIAN.
   * 
   * @param name - The name of the value.
   * @returns A number equaling to the DWORD value.
   * @throws {RegKeyError} if failed.
   */
  getDwordValue(name: string): number

  /**
   * Get the QWORD value of the given name.
   * The target value type must be REG_QWORD or REG_QWORD_LITTLE_ENDIAN.
   * 
   * @param name - The name of the value.
   * @returns A bigint equaling to the QWORD value.
   * @throws {RegKeyError} if failed.
   */
  getQwordValue(name: string): bigint

  /**
   * Get the type of the value of the given name.
   * 
   * @param name - The name of the value.
   * @returns The type of the value. It is a value in enum RegValueType.
   * @throws {RegKeyError} if failed.
   */
  getValueType(name: string): RegValueType

  /**
   * Check if the key has the given value.
   * 
   * @param name - The name of the value.
   * @returns True if the key has the given value.
   */
  hasValue(name: string): boolean

  /**
   * Get all value names in the key.
   * 
   * @returns An array containing all value names.
   */
  getValueNames(): string[]

  /**
   * Set the binary value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_BINARY.
   * @returns True if the value is set successfully.
   */
  setBinaryValue(name: string, val: Buffer): boolean

  /**
   * Set the string value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_SZ.
   * @returns True if the value is set successfully.
   */
  setStringValue(name: string, val: string, type?: RegValueType): boolean

  /**
   * Set the multi-string value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_MULTI_SZ.
   * @returns True if the value is set successfully.
   */
  setMultiStringValue(name: string, val: string[], type?: RegValueType): boolean

  /**
   * Set the DWORD value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_DWORD.
   * @returns True if the value is set successfully.
   */
  setDwordValue(name: string, val: number, type?: RegValueType): boolean

  /**
   * Set the QWORD value of the given name.
   * 
   * @param name - The name of the value.
   * @param val - The value.
   * @param type - The type of the value.
   *               If not specified, the type will be REG_QWORD.
   * @returns True if the value is set successfully.
   */
  setQwordValue(name: string, val: number | bigint, type?: RegValueType): boolean

  /**
   * Delete the value of the given name.
   * 
   * @param name - The name of the value.
   * @returns True if the value is deleted successfully.
   */
  deleteValue(name: string): boolean
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
  value: string | string[] | number | bigint | Buffer
  
  /**
   * - The raw data of the value.
   * - Setting this property will change the value. The effect is the same as setting the value property.
   * - The type of this property is Buffer.
   */
  data: Buffer

  /**
   * Get the value.
   * Note: It's hard to predict the return type of this function.
   *       If you are sure about the type of the value, use RegKey.getXxxValue instead.
   * 
   * @param resultType - The type of the value. It must be one of Number, String, Array, Buffer.
   *                     If not specified, the type will be inferred from the value.
   * @returns The value with specified type.
   * @throws {RegKeyError}
   */
  get<T>(resultType?: new (...args: any) => T): T

  /**
   * Get the value.
   * Note: It's hard to predict the return type of this function.
   *       If you are sure about the type of the value, use RegKey.getXxxValue instead.
   * 
   * @param valueType - The type of the registry value. e.g. 'REG_SZ'.
   * @return The value.
   * @throws {RegKeyError}
   */
  get(valueType: RegValueType): string | string[] | number | bigint | Buffer

  /**
   * Set the value.
   * 
   * @param val - The value.
   * @param type - The type of the value. It is a value in enum RegValueType.
   *               If not specified, the type will be inferred from 'val' param.
   * @returns True if the value is set successfully.
   * @throws {RegKeyError}
   */
  set(val: string | string[] | number | bigint | Buffer, type?: RegValueType): boolean

  /**
   * Delete the value.
   * 
   * @returns True if the value is deleted successfully.
   */
  delete(): boolean

  /**
   * Rename the value.
   * 
   * @param newName - The new name of the value.
   * @returns True if the value is renamed successfully.
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
   * @param lastError - The last error message. Call RegKey.getLastError() to get it.
   */
  constructor(message: string,
              key: RegKey,
              value: string,
              lastError: string)

  /**
   * The RegKey object.
   */
  key: RegKey

  /**
   * The name of the value.
   */
  value: string

  /**
   * The last error message.
   */
  lastError: string
}

/**
 * Throw a RegKeyError if disabledRegKeyErrors() was not called.
 * 
 * @param message - The error message.
 * @param key - The RegKey object.
 * @param value - The name of the value.
 * @param lastError - The last error message. Call RegKey.getLastError() to get it.
 */
export declare function throwRegKeyError(message: string,
                                          key: RegKey,
                                          value: string,
                                          lastError: string): void
                                                                                 
/**
 * Disable RegKeyError.
 * If disabled, throwRegKeyError() will not throw an error.
 * 
 * @param disabled - If not set to false, RegKeyError will be disabled.
 */
export declare function disableRegKeyErrors(disabled?: boolean): void

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
