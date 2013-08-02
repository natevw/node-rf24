#include <node.h>
#include <v8.h>

#include "RF24.h"


using namespace v8;

class Wrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  Wrapper(uint8_t ce, uint8_t cs);
  ~Wrapper();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> PlusOne(const v8::Arguments& args);
  RF24 radio;
};

Wrapper::Wrapper(uint8_t ce, uint8_t cs) : radio(ce,cs) {};
Wrapper::~Wrapper() {};

void Wrapper::Init(Handle<Object> exports) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Wrapper"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("plusOne"),
      FunctionTemplate::New(PlusOne)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("Wrapper"), constructor);
}

Handle<Value> Wrapper::New(const Arguments& args) {
  HandleScope scope;

  Wrapper* obj = new Wrapper(args[0]->NumberValue(), args[1]->NumberValue());
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> Wrapper::PlusOne(const Arguments& args) {
  HandleScope scope;

  Wrapper* obj = ObjectWrap::Unwrap<Wrapper>(args.This());
  (void)obj;

  return scope.Close(Number::New(42));
}


void init(Handle<Object> exports) {
    Wrapper::Init(exports);
}

NODE_MODULE(rf24, init)