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

export declare class RegKey {
  constructor(...paths: string[])

  readonly path: string
  name: string
  readonly open: boolean
  
  copy(src: RegKey): boolean
  close(): void

  value(name: string): RegValue
  values(): RegValue[]

  // returns null if setting new value fails
  newValue(name: string,
           val: string | string[] | number | Buffer,
           type: RegValueType): RegValue | null
           
  getBinaryValue(name: string): Buffer
  getStringValue(name: string): string
  getMultiStringValue(name: string): string[]
  getDwordValue(name: string): number
  getQwordValue(name: string): number

  getValueType(name: string): RegValueType
  hasValue(name: string): boolean
  getValueNames(): string[]

  setBinaryValue(name: string, val: Buffer): boolean
  setStringValue(name: string, val: string): boolean
  setMultiStringValue(name: string, val: string[]): boolean
  setDwordValue(name: string, val: number): boolean
  setQwordValue(name: string, val: number): boolean
  deleteValue(name: string): boolean

  delete(): boolean
  openSubkey(name: string): RegKey | null
  createSubkey(name: string): RegKey | null
  deleteSubkey(name: string): boolean
  getSubkeyNames(): string[]
  hasSubkey(name: string): boolean
  isWritable(): boolean
}

export declare class RegValue {
  constructor(key: RegKey, name: string)

  key: RegKey
  name: string

  readonly type: RegValueType
  readonly exists: boolean

  // read as string
  value: string
  // read as buffer
  data: Buffer

  // resultType shuold be one of String, Array, Number, Buffer
  // the function may throw RegKeyError if disableRegKeyErrors() was not called
  get(resultType: Function | undefined): string | string[] | number | Buffer

  // if type is not specified, it will be inferred from val
  set(val: string | string[] | number | Buffer, type: RegValueType | undefined): boolean

  delete(): boolean
  rename(newName: string): boolean
}

declare class RegKeyError extends Error {
  constructor(message: string,
              key: RegKey,
              value: string,
              lastErrorCode: number)

  key: RegKey
  value: string
  lastErrorCode: number
}

export declare function disableRegKeyErrors(disabled: boolean | undefined): void

// Equals to Windows API GetLastError()
export declare function getLastError(): number

export declare const hkcr: RegKey
export declare const hkcu: RegKey
export declare const hklm: RegKey
export declare const hku : RegKey
export declare const hkcc: RegKey
export declare const hkpd: RegKey
export declare const hkpt: RegKey
export declare const hkpn: RegKey
