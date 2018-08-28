#include <nan.h>
#include "openArchive.h" 
#include "../StormLib/src/StormLib.h"
#include "CreateError.h"
#include "StormArchive.h"

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;


class ArchiveWorker : public AsyncWorker {
public:
	ArchiveWorker(Callback *callback, const char * szArchiveFile)
		: AsyncWorker(callback), nError_(0) {
            size_t len = strlen(szArchiveFile);
            szArchiveFile_ = new char[len+1];
            memcpy(szArchiveFile_, szArchiveFile,len+1);
        }
	~ArchiveWorker() {
        if(szArchiveFile_) {
            delete [] szArchiveFile_;
        }
    }

	void Execute() {
		HANDLE hMpq = NULL;
        
        if (!SFileOpenArchive(this->szArchiveFile_, 0, 0, &hMpq))
            nError_ = GetLastError(); 
        hArchive_ = hMpq;
	}

	void HandleOKCallback() {
        long long lhArchve = (long long)hArchive_;
		HandleScope scope;
		Local<Value> argv[] = {
			Null(),
			New<v8::String>(this->szArchiveFile_).ToLocalChecked(),
            New<Number>(lhArchve)
		};

        if(nError_ != ERROR_SUCCESS) {
            v8::Local<v8::Object> obj =CreateError(nError_);            
            argv[0] = obj;
        }

        v8::Local<v8::Value> cargv[2] = {New<Number>(lhArchve), Nan::New(-19280)};
        v8::Local<v8::Function> cons = Nan::New(StormArchive::constructor);
        argv[1] = Nan::NewInstance(cons, 2, cargv).ToLocalChecked();

		callback->Call(3, argv, async_resource);
	}

private:
	char * szArchiveFile_;
    HANDLE hArchive_;
    int nError_;
};


NAN_METHOD(OpenArchive) {
	Nan::MaybeLocal<v8::String> tmp = Nan::To<v8::String>(info[0]);
    Local<v8::String> local_string = tmp.ToLocalChecked();
    Nan::Utf8String val(local_string);

	const char * szArchiveFileName = *(val);
	Callback *callback = new Callback(To<Function>(info[1]).ToLocalChecked());
	AsyncQueueWorker(new ArchiveWorker(callback, szArchiveFileName));
}

NAN_METHOD(CreateArchive) {
	Nan::MaybeLocal<v8::String> tmp = Nan::To<v8::String>(info[0]);
    Local<v8::String> local_string = tmp.ToLocalChecked();
    Nan::Utf8String val(local_string);

	const char * szArchiveFileName = *(val);

    int flags = info[1]->IsUndefined() ?  0:Nan::To<int>(info[1]).FromJust();
    int maxFileCount = info[2]->IsUndefined() ?  1024: Nan::To<int>(info[2]).FromJust();
    

    int nError = ERROR_SUCCESS;
    HANDLE hArchive = NULL;
    if (!SFileCreateArchive(szArchiveFileName, flags, 1024, &hArchive)) {
        nError = GetLastError();
    }

    v8::Local<v8::Function> callbackHandle = info[3].As<v8::Function>();
    Nan::AsyncResource* resource = new Nan::AsyncResource(Nan::New<v8::String>("CreateArchiveCallback").ToLocalChecked());

    int argc = 2;
    v8::Local<v8::Value> argv[] = {
        Null(),
        Null()
    };

    if(nError != ERROR_SUCCESS) {
        v8::Local<v8::Object> obj =CreateError(nError);            
        argv[0] = obj;
    } else {
        long long lhArchve = (long long)hArchive;
        v8::Local<v8::Value> cargv[2] = {New<Number>(lhArchve), Nan::New(-19280)};
        v8::Local<v8::Function> cons = Nan::New(StormArchive::constructor);
        argv[1] = Nan::NewInstance(cons, 2, cargv).ToLocalChecked();
    }

    resource->runInAsyncScope(Nan::GetCurrentContext()->Global(), callbackHandle, argc, argv);
}
