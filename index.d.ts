// Type definitions for node-regkey

// Registry value types
export enum RegValueType {
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

// Registry key value info
export interface RegValueInfo {
  name: string;
  type?: RegValueType;
  data: Buffer | string | number;
}

export interface RegStringValueInfo {
  name: string;
  type?: RegValueType | undefined;
  data: string;
}

export interface RegNumberValueInfo {
  name: string;
  type?: RegValueType | undefined;
  data: number;
}

declare class RegKey {
  readonly path: string;

  // Basic attributes
  isValid(): boolean;
  copyTree(source: string): boolean;
  close(): void;

  // Value Reading Operations
  getValue(name: string): Buffer;
  getStringValue(name: string): string;
  getNumberValue(name: string): number;

  // Multiple Value Reading Operations
  getValues(): RegValueInfo[];
  getStringValues(): RegStringValueInfo[];
  getNumberValues(): RegNumberValueInfo[];
  getValueType(name: string): RegValueType;
  hasValue(name: string): boolean;

  // Value Writing Operations
  setValue(name: string, data: Buffer, type?: RegValueType | undefined): boolean;
  applyValues(values: RegValueInfo[]): Number;
  setStringValue(name: string, value: string): boolean;
  setNumberValue(name: string, value: number): boolean;
  deleteValue(name: string): boolean;
  
  // Key Operations
  rename(name: string): boolean;
  deleteKey(): boolean;
  openSubkey(name: string): RegKey | null;
  createSubkey(name: string): RegKey | null;
  deleteSubkey(name: string): boolean;
  getSubkeyNames(): string[];
  hasSubkey(name: string): boolean;
  isSubkeyWritable(name: string): boolean;
}

export declare const hkcr: RegKey;
export declare const hkcu: RegKey;
export declare const hklm: RegKey;
export declare const hku:  RegKey;
export declare const hkcc: RegKey;
export declare const hkpd: RegKey;
export declare const hknt: RegKey;
export declare const hkpn: RegKey;
