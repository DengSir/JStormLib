#include "CreateError.h"

v8::Local<v8::Object> CreateError(int code) {
    char * message = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, code,
        MAKELANGID(0x0409, 0x01),
        (LPTSTR)&message, 0, NULL
    );

    v8::Local<v8::Object> obj = Nan::New<v8::Object>();
    int size = strlen(message);
    if (message[size-1] == '\n') {
        message[size-2] = '\0';
    }
    Nan::Set(obj, Nan::New("code").ToLocalChecked(), Nan::New(code));
    Nan::Set(obj, Nan::New("message").ToLocalChecked(), Nan::New(message).ToLocalChecked());
    delete [] message;
    return obj;
}

