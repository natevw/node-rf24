#include <node.h>
#include <v8.h>

#include "RF24.h"

using namespace v8;

class Wrapper : public node::ObjectWrap {
public:
    static void Init(Handle<Object> exports);
    
private:
    Wrapper(uint8_t ce, uint8_t cs);
    ~Wrapper();
    
    RF24 radio;
    uv_mutex_t radioAccess;
    
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Begin(const Arguments& args);
    static Handle<Value> Listen(const Arguments& args);
    static Handle<Value> Write(const Arguments& args);
    static Handle<Value> Available(const Arguments& args);
    static Handle<Value> Read(const Arguments& args);
};

// HT http://kkaefer.github.io/node-cpp-modules/#calling-async
struct Baton {
    uv_work_t request;
    Persistent<Function> callback;
    
    Wrapper* obj;
    uint8_t custom[64];
    
    enum customType {VOID_CUSTOM,BOOL_CUSTOM,DATA_CUSTOM,};
    customType resultType;
};

extern "C" {
    void RadioBegin(uv_work_t* req) {
        Baton* baton = static_cast<Baton*>(req->data);
        uv_mutex_lock(baton->obj->radioAccess);
        baton->obj->radio.begin();
        uv_mutex_unlock(baton->obj->radioAccess);
        baton->resultType = VOID_CUSTOM;
    }
    
    void RadioListen(uv_work_t* req) {
        Baton* baton = static_cast<Baton*>(req->data);
        uv_mutex_lock(baton->obj->radioAccess);
        if (baton->custom[0]) baton->obj->radio.startListening();
        else baton->obj->radio.stopListening();
        uv_mutex_unlock(baton->obj->radioAccess);
        baton->resultType = VOID_CUSTOM;
    }
    
    void RadioWrite(uv_work_t* req) {
        Baton* baton = static_cast<Baton*>(req->data);
        uv_mutex_lock(baton->obj->radioAccess);
        baton->custom[0] = baton->obj->radio.write(baton->custom + 1, baton->custom[0]);
        uv_mutex_unlock(baton->obj->radioAccess);
        baton->resultType = BOOL_CUSTOM;
    }
    
    void RadioAvailable(uv_work_t* req) {
        Baton* baton = static_cast<Baton*>(req->data);
        uv_mutex_lock(baton->obj->radioAccess);
        baton->custom[0] = baton->obj->radio.available();
        uv_mutex_unlock(baton->obj->radioAccess);
        baton->resultType = BOOL_CUSTOM;
    }
    
    void RadioRead(uv_work_t* req) {
        Baton* baton = static_cast<Baton*>(req->data);
        uv_mutex_lock(baton->obj->radioAccess);
        baton->custom[0] = baton->obj->radio.getPayloadSize();
        (void)baton->obj->radio.read(baton->custom+1, sizeof(baton->custom)-1);
        uv_mutex_unlock(baton->obj->radioAccess);
        baton->resultType = DATA_CUSTOM;
    }
    
    void FinishRadioCall(uv_work_t* req) {
        HandleScope scope;
        Baton* baton = static_cast<Baton*>(req->data);
        
        Local<Value> e = Null();
        Local<Value> d = Null();        // TODO: set based on baton->resultType
        Local<Value> argv[] = {e,d};
        TryCatch try_catch;
        baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }
        
        baton->callback.Dispose();
        delete baton;
    }
}

Wrapper::Wrapper(uint8_t ce, uint8_t cs) : radio(ce,cs) {
    uv_mutex_init(&radioAccess);
};
Wrapper::~Wrapper() {
    uv_mutex_destroy(&radioAccess);
};

void Wrapper::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("Wrapper"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("begin"), FunctionTemplate::New(Begin)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("listen"), FunctionTemplate::New(Listen)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("write"), FunctionTemplate::New(Write)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("available"), FunctionTemplate::New(Available)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("read"), FunctionTemplate::New(Read)->GetFunction());
    
    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("Wrapper"), constructor);
}

Handle<Value> Wrapper::New(const Arguments& args) {
    HandleScope scope;
    
    // TODO: check arguments like http://nodejs.org/api/addons.html#addons_function_arguments
    Wrapper* obj = new Wrapper(args[0]->NumberValue(), args[1]->NumberValue());
    obj->Wrap(args.This());
    
    return args.This();
}

Handle<Value> Wrapper::Begin(const Arguments& args) {
    HandleScope scope;
    
    assert(args.Length() == 1);
    assert(args[0]->IsFunction());
    
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->callback = Persistent<Function>::New(args[0]);
    baton->obj = ObjectWrap::Unwrap<Wrapper>(args.This());
    uv_queue_work(uv_default_loop(), &baton->request, RadioBegin, FinishRadioCall);
    
    return scope.Close(Undefined());
}

Handle<Value> Wrapper::Listen(const Arguments& args) {
    HandleScope scope;
    
    assert(args.Length() == 2);
    assert(args[0]->IsBoolean());
    assert(args[1]->IsFunction());
    
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->callback = Persistent<Function>::New(args[1]);
    baton->obj = ObjectWrap::Unwrap<Wrapper>(args.This());
    baton->custom[0] = args[0]->BooleanValue();
    uv_queue_work(uv_default_loop(), &baton->request, RadioListen, FinishRadioCall);
    
    return scope.Close(Undefined());
}

Handle<Value> Wrapper::Write(const Arguments& args) {
    HandleScope scope;
    
    assert(args.Length() == 2);
    assert(Buffer::HasInstance(args[0]));
    assert(Buffer::Length(args[0]) < sizeof Baton().custom);        // http://stackoverflow.com/a/3718950/179583
    assert(args[1]->IsFunction());
    
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->callback = Persistent<Function>::New(args[1]);
    baton->obj = ObjectWrap::Unwrap<Wrapper>(args.This());
    baton->custom[0] = Buffer::Length(args[0]);
    memcpy(baton->custom+1, Buffer::Data(args[0]), baton->custom[0]);
    uv_queue_work(uv_default_loop(), &baton->request, RadioWrite, FinishRadioCall);
    
    return scope.Close(Undefined());
}

Handle<Value> Wrapper::Available(const Arguments& args) {
    HandleScope scope;
    
    assert(args.Length() == 1);
    assert(args[0]->IsFunction());
    
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->callback = Persistent<Function>::New(args[1]);
    baton->obj = ObjectWrap::Unwrap<Wrapper>(args.This());
    uv_queue_work(uv_default_loop(), &baton->request, RadioAvailable, FinishRadioCall);
    
    return scope.Close(Undefined());
}

Handle<Value> Wrapper::Read(const Arguments& args) {
    HandleScope scope;
    
    assert(args.Length() == 1);
    assert(args[0]->IsFunction());
    
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->callback = Persistent<Function>::New(args[1]);
    baton->obj = ObjectWrap::Unwrap<Wrapper>(args.This());
    uv_queue_work(uv_default_loop(), &baton->request, RadioRead, FinishRadioCall);
    
    return scope.Close(Undefined());
}


void init(Handle<Object> exports) {
    Wrapper::Init(exports);
}

NODE_MODULE(rf24, init)
