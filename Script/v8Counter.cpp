#include "avocado-global.h"

#include "v8Counter.h"

using namespace v8;

namespace avo {

v8Counter::v8Counter(Handle<Object> wrapper)
{
	counter = Counter::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8Counter::~v8Counter() {
	delete counter;
}

void v8Counter::initialize(Handle<Object> target) {
	HandleScope scope;

	Handle<FunctionTemplate> constructor_template = FunctionTemplate::New(v8Counter::New);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Counter"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%current", v8Counter::Current);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%since", v8Counter::Since);

	target->Set(String::NewSymbol("Counter"), constructor_template->GetFunction());
}

Counter *v8Counter::wrappedCounter() {
	return counter;
}

v8::Handle<v8::Value> v8Counter::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Counter(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

v8::Handle<v8::Value> v8Counter::Current(const v8::Arguments &args) {
	HandleScope scope;

	v8Counter *counterWrapper = ObjectWrap::Unwrap<v8Counter>(args.Holder());

	if (NULL == counterWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Counter::current(): NULL Holder."
		)));
	}

	return scope.Close(Number::New(counterWrapper->counter->current()));
}

v8::Handle<v8::Value> v8Counter::Since(const v8::Arguments &args) {
	HandleScope scope;

	v8Counter *counterWrapper = ObjectWrap::Unwrap<v8Counter>(args.Holder());

	if (NULL == counterWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Counter::since(): NULL Holder."
		)));
	}

	return scope.Close(Number::New(counterWrapper->counter->since()));
}

}

