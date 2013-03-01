#include "main/avocado-global.h"

#include "v8Script.h"

using namespace v8;

namespace avo {

AbstractFactory<v8Script> *v8Script::factory = new AbstractFactory<v8Script>;

v8Script::v8Script()
	: Script()
{
}

v8Script::v8Script(v8::Handle<v8::Script> script)
	: Script()
{
	HandleScope scope;

	this->script = Persistent<v8::Script>::New(script);
}


void v8Script::execute() {
	HandleScope scope;

	TryCatch exception;

	script->Run();

	if (exception.HasCaught()) {
		throw script_execution_error(V8::stringifyException(exception));
	}
}

v8Script::~v8Script() {
	script.Dispose();
}

}
