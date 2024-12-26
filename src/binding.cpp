#ifdef WIN32
#include "RegKeyWrap.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports = RegKeyWrap::Init(env, exports);

    exports.Set("hkcr", RegKeyWrap::NewInstance(env, HKEY_CLASSES_ROOT,        STR("HKEY_CLASS_ROOT")));
    exports.Set("hkcu", RegKeyWrap::NewInstance(env, HKEY_CURRENT_USER,        STR("HKEY_CURRENT_USER")));
    exports.Set("hklm", RegKeyWrap::NewInstance(env, HKEY_LOCAL_MACHINE,       STR("HKEY_LOCAL_MACHINE")));
    exports.Set("hku" , RegKeyWrap::NewInstance(env, HKEY_USERS,               STR("HKEY_USERS")));
    exports.Set("hkcc", RegKeyWrap::NewInstance(env, HKEY_CURRENT_CONFIG,      STR("HKEY_CURRENT_CONFIG")));
    exports.Set("hkpd", RegKeyWrap::NewInstance(env, HKEY_PERFORMANCE_DATA,    STR("HKEY_PERFORMANCE_DATA")));
    exports.Set("hkpt", RegKeyWrap::NewInstance(env, HKEY_PERFORMANCE_TEXT,    STR("HKEY_PERFORMANCE_TEXT")));
    exports.Set("hkpn", RegKeyWrap::NewInstance(env, HKEY_PERFORMANCE_NLSTEXT, STR("HKEY_PERFORMANCE_NLSTEXT")));

    Napi::Object access = Napi::Object::New(env);

    access.Set("Default",                   Napi::Number::New(env, 0));
    access.Set("Read",                      Napi::Number::New(env, KEY_READ));
    access.Set("Write",                     Napi::Number::New(env, KEY_WRITE));
    access.Set("Execute",                   Napi::Number::New(env, KEY_EXECUTE));
    access.Set("AllAccess",                 Napi::Number::New(env, KEY_ALL_ACCESS));

    access.Set("ia32",                      Napi::Number::New(env, KEY_WOW64_32KEY));
    access.Set("x64",                       Napi::Number::New(env, KEY_WOW64_64KEY));

    access.Set("KEY_QUERY_VALUE",           Napi::Number::New(env, KEY_WOW64_RES));
    access.Set("KEY_SET_VALUE",             Napi::Number::New(env, KEY_SET_VALUE));
    access.Set("KEY_CREATE_SUB_KEY",        Napi::Number::New(env, KEY_CREATE_SUB_KEY));
    access.Set("KEY_ENUMERATE_SUB_KEYS",    Napi::Number::New(env, KEY_ENUMERATE_SUB_KEYS));
    access.Set("KEY_NOTIFY",                Napi::Number::New(env, KEY_NOTIFY));
    access.Set("KEY_CREATE_LINK",           Napi::Number::New(env, KEY_CREATE_LINK));
    access.Set("KEY_WOW64_32KEY",           Napi::Number::New(env, KEY_WOW64_32KEY));
    access.Set("KEY_WOW64_64KEY",           Napi::Number::New(env, KEY_WOW64_64KEY));
    access.Set("KEY_WOW64_RES",             Napi::Number::New(env, KEY_WOW64_RES));

    access.Set("STANDARD_RIGHTS_REQUIRED",  Napi::Number::New(env, STANDARD_RIGHTS_REQUIRED));
    access.Set("STANDARD_RIGHTS_READ",      Napi::Number::New(env, STANDARD_RIGHTS_READ));
    access.Set("STANDARD_RIGHTS_WRITE",     Napi::Number::New(env, STANDARD_RIGHTS_WRITE));
    access.Set("STANDARD_RIGHTS_EXECUTE",   Napi::Number::New(env, STANDARD_RIGHTS_EXECUTE));
    access.Set("STANDARD_RIGHTS_ALL",       Napi::Number::New(env, STANDARD_RIGHTS_ALL));

    access.Set("KEY_READ",                  Napi::Number::New(env, KEY_READ));
    access.Set("KEY_WRITE",                 Napi::Number::New(env, KEY_WRITE));
    access.Set("KEY_EXECUTE",               Napi::Number::New(env, KEY_EXECUTE));
    access.Set("KEY_ALL_ACCESS",            Napi::Number::New(env, KEY_ALL_ACCESS));

    exports.Set("RegKeyAccess", access);
    return exports;
}

#else
#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("RegKey", Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                              { return info.Env().Undefined(); }));

    exports.Set("hkcr", env.Null());
    exports.Set("hkcu", env.Null());
    exports.Set("hklm", env.Null());
    exports.Set("hku" , env.Null());
    exports.Set("hkcc", env.Null());
    exports.Set("hkpd", env.Null());
    exports.Set("hkpt", env.Null());
    exports.Set("hkpn", env.Null());

    exports.Set("RegKeyAccess", Napi::Object::New(env));

    return exports;
}

#endif

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
