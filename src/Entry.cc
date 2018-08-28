#include <nan.h>
#include "openArchive.h"
#include "StormArchive.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("open").ToLocalChecked(), GetFunction(New<FunctionTemplate>(OpenArchive)).ToLocalChecked());
  Set(target, New<String>("create").ToLocalChecked(), GetFunction(New<FunctionTemplate>(CreateArchive)).ToLocalChecked());
  StormArchive::Init(target);
}
NODE_MODULE(addon, InitAll);
