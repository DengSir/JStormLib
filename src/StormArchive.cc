#include <nan.h>
#include "StormArchive.h" 
#include "../StormLib/src/StormLib.h"
#include "CreateError.h"

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

class FileWriter: public AsyncWorker {
public:
    FileWriter(Callback *callback, const HANDLE hArchive, v8::Local<v8::String> &filename, v8::Local<v8::Object> &filebuff, 
        int lcid, int flags, int compression)
        : AsyncWorker(callback), hArchive_(hArchive), compression_(compression), flags_(flags) {
        nError_ = 0;
        lcid_ = lcid;
        SaveToPersistent("filebuff", filebuff);
        SaveToPersistent("filename", filename);
    }
    ~FileWriter() {

    }

    void Execute() {
        

    }

    void HandleOKCallback() {
        HANDLE pFile;
        v8::Local<v8::Value> filebuff = GetFromPersistent("filebuff");
        v8::Local<v8::Value> filename = GetFromPersistent("filename");
        Nan::Utf8String strFileName(filename);

        int length = node::Buffer::Length(filebuff);
        char * data = node::Buffer::Data(filebuff);
        char * szFileName = *strFileName;

        if (!SFileCreateFile(hArchive_, szFileName, 0, length, lcid_, flags_, &pFile)) {
            nError_ = GetLastError();
        }

        if (nError_ == ERROR_SUCCESS) {
            if (!SFileWriteFile(pFile, (void *)(data), length, compression_)) 
                nError_ = GetLastError();
        }

     
        SFileFinishFile(pFile);
        SFileFlushArchive(hArchive_);  

        HandleScope scope;
		Local<Value> argv[] = {
			Null(),
			Nan::New(nError_)
		};

        if(nError_ != ERROR_SUCCESS) {
            v8::Local<v8::Object> obj = CreateError(nError_);
            argv[0] = obj;
        }
        
        callback->Call(2, argv, async_resource);
    }
private:
    char * szFileName_;
    const HANDLE hArchive_;
    int size_;
    int nError_;
    int lcid_;
    int compression_;
    int flags_;
};


class FileWorker : public AsyncWorker {
public:
	FileWorker(Callback *callback, const HANDLE pArchive, const char * szFileName)
		: AsyncWorker(callback), pArchive_(pArchive), error_(0), data_(NULL), bufferSize_(0) {
            size_t len = strlen(szFileName);
            szFileName_ = new char[len+1];
            memcpy(szFileName_, szFileName,len+1);
        }
	~FileWorker() {
        delete [] szFileName_;
    }

	void Execute() {
		HANDLE hArchive = pArchive_;
        HANDLE pFile;
        long long size = 0;
        size_t sizeH = 0;
        size_t sizeL = 0;
       
        if (!SFileOpenFileEx(hArchive, szFileName_, 0, &pFile)) 
            error_ = GetLastError();
        
        if (error_ == ERROR_SUCCESS) {
            sizeL = SFileGetFileSize(pFile, (LPDWORD)&sizeH);
            if (sizeL == SFILE_INVALID_SIZE)
                error_ = GetLastError();
            else
                size = ((long long)sizeH << 32) + sizeL;
        }

        if (size > 0) {
            data_= new char[size];
            bufferSize_ = size;
            char * buffer = data_;
            uint32_t readBytes = size;
            while (readBytes > 0) {
                SFileReadFile(pFile, buffer, size, (LPDWORD)&readBytes, NULL);
                buffer += readBytes;
            }
        }
        SFileCloseFile(pFile);
	}

	void HandleOKCallback() {
        HandleScope scope;
		Local<Value> argv[] = {
			Null(),
			Nan::NewBuffer(data_, bufferSize_).ToLocalChecked()
		};

        if(error_ != ERROR_SUCCESS) {
            v8::Local<v8::Object> obj = CreateError(error_);
            argv[0] = obj;
        }
        
        callback->Call(2, argv, async_resource);
    }
private:
    char * szFileName_;
    const HANDLE pArchive_;
    int error_;
    char * data_;
    uint32_t bufferSize_;
};

Nan::Persistent<v8::Function> StormArchive::constructor;

NAN_MODULE_INIT(StormArchive::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("StormArchive").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "getFileList", GetFileList);
    Nan::SetPrototypeMethod(tpl, "readFile", ReadFile);
    Nan::SetPrototypeMethod(tpl, "writeFile", WriteFile);
    Nan::SetPrototypeMethod(tpl, "getLocale", GetLocale);
    Nan::SetPrototypeMethod(tpl, "setLocale", SetLocale);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target, Nan::New("StormArchive").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

StormArchive::StormArchive(long long hArchive)  {
    hArchive_ = (HANDLE)hArchive;
}

StormArchive::~StormArchive() {
    SFileCloseArchive(hArchive_);
}


NAN_METHOD(StormArchive::New) {
    int ver = 0;
    if (info.Length() == 2) 
        ver = Nan::To<int>(info[1]).FromJust();

    if (ver != -19280) 
        Nan::ThrowTypeError("The StormArchive object can be constructed by user.");

    if (info.IsConstructCall()) {
        double c = info[0]->IsUndefined() ? 0 : Nan::To<double>(info[0]).FromJust();
        StormArchive *obj = new StormArchive((long long)c);
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    }
}

NAN_METHOD(StormArchive::GetFileList) {
    StormArchive* obj = Nan::ObjectWrap::Unwrap<StormArchive>(info.This());
    HANDLE hArchive = obj->hArchive_;
	HANDLE searchHandle = NULL;
	int nError = ERROR_SUCCESS;

	SFILE_FIND_DATA findData;
	std::vector<std::string> fileNames;

	searchHandle = SFileFindFirstFile(hArchive, "*", &findData, NULL);
	if (!searchHandle) 
		nError = GetLastError();
	else 
		fileNames.push_back(std::string(findData.cFileName));

	while (0 != searchHandle && nError == ERROR_SUCCESS) {
		if (!SFileFindNextFile(searchHandle, &(findData))) 
            nError = GetLastError();
		else
			fileNames.push_back(std::string(findData.cFileName));
	}
	v8::Local<v8::Array> result = Nan::New<v8::Array>(fileNames.size());
	int i = 0;
	for (std::vector<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		std::string pStr = (std::string)*it;
        Nan::Set(result, i++, Nan::New(pStr.c_str()).ToLocalChecked());
	}
    info.GetReturnValue().Set(result);
}

NAN_METHOD(StormArchive::ReadFile) {
    StormArchive* obj = Nan::ObjectWrap::Unwrap<StormArchive>(info.This());
    Nan::MaybeLocal<v8::String> tmp = Nan::To<v8::String>(info[0]);
    Local<v8::String> local_string = tmp.ToLocalChecked();
    Nan::Utf8String val(local_string);
    char * szFileName = *val;
    Callback *callback = new Callback(To<Function>(info[1]).ToLocalChecked());
	AsyncQueueWorker(new FileWorker(callback, obj->hArchive_, szFileName));
}


NAN_METHOD(StormArchive::WriteFile) {
    StormArchive* obj = Nan::ObjectWrap::Unwrap<StormArchive>(info.This());
    Nan::MaybeLocal<v8::String> tmp = Nan::To<v8::String>(info[1]);
    Local<v8::String> local_string = tmp.ToLocalChecked();
    Nan::Utf8String strFileName(local_string);

    v8::Local<v8::Object> bufferHandle = To<v8::Object>(info[0]).ToLocalChecked();
    
    int lcid = info[2]->IsUndefined() ? 0 : Nan::To<int>(info[2]).FromJust();

    int flags =  Nan::To<int>(info[3]).FromJust();

    int compression =  Nan::To<int>(info[4]).FromJust();

    Callback *callback = new Callback(To<Function>(info[5]).ToLocalChecked());
	AsyncQueueWorker(new FileWriter(callback, obj->hArchive_, local_string, bufferHandle, lcid, flags, compression));
}

NAN_METHOD(StormArchive::GetLocale) {
    LCID lcid = SFileGetLocale();
    info.GetReturnValue().Set(Nan::New((int)lcid));
}

NAN_METHOD(StormArchive::SetLocale) {
    int lcid = Nan::To<int>(info[0]).FromJust();
    SFileSetLocale(lcid);
}


NAN_METHOD(StormArchive::ToString) {
    info.GetReturnValue().Set(Nan::New("StormArchive").ToLocalChecked());
}

