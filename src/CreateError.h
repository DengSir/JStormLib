#ifndef CREATE_ERROR_H
#define CREATE_ERROR_H

#include <nan.h>
v8::Local<v8::Object> CreateError(int code);
#endif