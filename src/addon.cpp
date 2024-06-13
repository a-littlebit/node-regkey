#include "RegKeyWrap.h"

// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  RegKeyWrap::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
