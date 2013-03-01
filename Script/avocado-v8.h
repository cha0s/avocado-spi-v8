#ifndef AVOCADO_V8_H
#define AVOCADO_V8_H

#include "main/avocado-global.h"

#include <string>

#include <v8.h>

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * @namespace avo::V8
 *
 * @brief Utility functions and embed bindings for
 * [V8](http://code.google.com/p/v8/).
 *
 * @ingroup V8
 */
namespace V8 {

/** Take a JS value and convert it to JSON. This returns v8::Value and not
 *  v8::String because when JSON.stringify fails, we have to throw/return an
 *  exception. */
v8::Handle<v8::Value> toJson(v8::Handle<v8::Value> value);

/** Turn an exception from V8 into a string we can use to report the error. */
std::string stringifyException(const v8::TryCatch& try_catch, bool suppressBacktrace = false);

/** Convert a V8 string to a std::string. */
std::string stringToStdString(v8::Handle<v8::String> value);

}

}

#define V8_SET_METHOD(obj, name, callback)                     \
  obj->Set(v8::String::NewSymbol(name),                        \
           v8::FunctionTemplate::New(callback))

#define V8_SET_PROTOTYPE_METHOD(templ, name, callback)                    \
do {                                                                      \
  v8::Local<v8::Signature> __callback##_SIG = v8::Signature::New(templ);  \
  v8::Local<v8::FunctionTemplate> __callback##_TEM =                      \
    v8::FunctionTemplate::New(callback, v8::Handle<v8::Value>(),          \
                          __callback##_SIG);                              \
  templ->PrototypeTemplate()->Set(v8::String::NewSymbol(name),            \
                                  __callback##_TEM);                      \
} while (0)

#define V8_SET_PROPERTY(obj, name, getter, setter)              \
  obj->SetAccessor(v8::String::NewSymbol(name), getter, setter)

#define V8_SET_INTERCEPTOR(obj, getter, setter) \
  obj->SetNamedPropertyHandler(getter, setter)

#endif // AVOCADO_V8_H
