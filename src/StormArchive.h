#ifndef STORM_ARCHIVE_H
#define STORM_ARCHIVE_H

#include <nan.h>

class StormArchive : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);
    static Nan::Persistent<v8::Function> constructor;

  private:
    explicit StormArchive(long long lhArchive);
    ~StormArchive();

    static NAN_METHOD(New);

    static NAN_METHOD(GetFileList);
    static NAN_METHOD(ReadFile);
    static NAN_METHOD(WriteFile);
    static NAN_METHOD(SetLocale);
    static NAN_METHOD(GetLocale);
    static NAN_METHOD(ToString);
  
    HANDLE hArchive_;
};


#endif