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
export interface RegValue {
  name: string;
  type?: RegValueType;
  data: Buffer | string | number | Array<string>;
}

export interface RegBufferValue {
  name: string;
  type?: RegValueType | undefined;
  data: Buffer;
}

export interface RegStringValue {
  name: string;
  type?: RegValueType | undefined;
  data: string;
}

export interface RegNumberValue {
  name: string;
  type?: RegValueType | undefined;
  data: number;
}

export interface RegMultiStringValue {
  name: string;
  type?: RegValueType | undefined;
  data: Array<string>;
}

export interface GetValueOptions {
  type?: Function;
  mapByName?: boolean;
}

export interface ValueMap {
  [key: string]: RegValue;
}

export declare class RegKey {
  readonly path: string;
  name: string;

  constructor(...paths: string);

  // Basic attributes
  isValid(): boolean;
  copyTree(source: string): boolean;
  close(): void;

  // Value Reading Operations
  getValue(name: string, options?: GetValueOptions): RegValue;
  getBufferValue(name: string): Buffer;
  getStringValue(name: string): string;
  getNumberValue(name: string): number;
  getMultiStringValue(name: string): Array<string>;

  // Multiple Value Reading Operations
  getValues(options?: GetValueOptions): RegValue[] | ValueMap;
  getBufferValues(): RegBufferValue[];
  getStringValues(): RegStringValue[];
  getNumberValues(): RegNumberValue[];
  getMultiStringValues(): RegMultiStringValue[];
  getValueType(name: string): RegValueType;
  hasValue(name: string): boolean;

  // Value Writing Operations
  setBufferValue(name: string, data: Buffer, type?: RegValueType | undefined): boolean;
  setStringValue(name: string, value: string): boolean;
  setNumberValue(name: string, value: number): boolean;
  setMultiStringValue(name: string, value: Array<string>): boolean;
  putValues(values: RegValue[] | ValueMap): Number;
  deleteValue(name: string): boolean;
  
  // Key Operations
  deleteKey(): boolean;
  openSubkey(name: string): RegKey | null;
  createSubkey(name: string): RegKey | null;
  deleteSubkey(name: string): boolean;
  getSubkeyNames(): string[];
  hasSubkey(name: string): boolean;
  isSubkeyWritable(name: string): boolean;
}

export declare function disableRegKeyErrors(): undefined;

export declare const hkcr: RegKey;
export declare const hkcu: RegKey;
export declare const hklm: RegKey;
export declare const hku:  RegKey;
export declare const hkcc: RegKey;
export declare const hkpd: RegKey;
export declare const hkpt: RegKey;
export declare const hkpn: RegKey;
