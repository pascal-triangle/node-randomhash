#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <stdint.h>
#include <nan.h>

#include "main.h"
#include "RandomHash.h"

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)

void callback(char* data, void* hint) {
    free(data);
}

using namespace node;
using namespace v8;
using namespace Nan;

NAN_METHOD(hashSync) {

    if (info.Length() != 1)
	return THROW_ERROR_EXCEPTION("You must provide exactly one argument.");

    Local<Object> target = info[0]->ToObject();

    if(!Buffer::HasInstance(target))
	return THROW_ERROR_EXCEPTION("Argument should be a buffer object.");

    U8* input = (U8*)node::Buffer::Data(target);
  
    U8 output[32];

    RandomHash_Simple(input, output);
  
    v8::Local<v8::Value> returnValue = Nan::CopyBuffer((char*)output, 32).ToLocalChecked();
    info.GetReturnValue().Set(returnValue);
}

class RHAsyncWorker : public Nan::AsyncWorker{
public:
    RHAsyncWorker(Nan::Callback *callback, U8 * input)
	: Nan::AsyncWorker(callback), input(input){}
    ~RHAsyncWorker() {}
  
    void Execute () {
	RandomHash_Simple(input, output);
    }

    void HandleOKCallback () {
	Nan::HandleScope scope;

	v8::Local<v8::Value> argv[] = {
	    Nan::Null()
	    , v8::Local<v8::Value>(Nan::CopyBuffer((char*)output, 32).ToLocalChecked())
	};

	callback->Call(2, argv);
    }

private:
    U8 * input;
    U8 output[32];
};

NAN_METHOD(hashAsync) {

    if (info.Length() != 2)
	return THROW_ERROR_EXCEPTION("You must provide exactly two arguments.");

    Local<Object> target = info[0]->ToObject();

    if(!Buffer::HasInstance(target))
	return THROW_ERROR_EXCEPTION("Argument should be a buffer object.");

    Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

    U8* input = (U8*)node::Buffer::Data(target);

    Nan::AsyncQueueWorker(new RHAsyncWorker(callback, input));

}

NAN_MODULE_INIT(init) {
    Nan::Set(target, Nan::New("hashSync").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(hashSync)).ToLocalChecked());
    Nan::Set(target, Nan::New("hashAsync").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(hashAsync)).ToLocalChecked());
}

NODE_MODULE(randomhash, init)
