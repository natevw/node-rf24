#include <node.h>
#include <v8.h>

using namespace v8;

class Wrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  Wrapper();
  ~Wrapper();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> PlusOne(const v8::Arguments& args);
  double counter_;
};

Wrapper::Wrapper() {};
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

  Wrapper* obj = new Wrapper();
  obj->counter_ = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> Wrapper::PlusOne(const Arguments& args) {
  HandleScope scope;

  Wrapper* obj = ObjectWrap::Unwrap<Wrapper>(args.This());
  obj->counter_ += 1;

  return scope.Close(Number::New(obj->counter_));
}


void init(Handle<Object> exports) {
    Wrapper::Init(exports);
}

NODE_MODULE(rf24, init)