#include "main/avocado-global.h"

#include "v8Sample.h"

using namespace v8;

namespace avo {

v8Sample::v8Sample(Handle<Object> wrapper, Sample *sample)
	: sample(sample)
	, owns(false)
{
	if (NULL == this->sample) {

		this->sample = Sample::factoryManager.instance()->create();

		::V8::AdjustAmountOfExternalAllocatedMemory(
			this->sample->sizeInBytes()
		);

		owns = true;
	}

	Wrap(wrapper);
}

v8Sample::~v8Sample() {
	releaseSample();
}

void v8Sample::releaseSample() {

	unsigned int size = sample->sizeInBytes();

	if (owns) {

		::V8::AdjustAmountOfExternalAllocatedMemory(
			-size
		);

		delete sample;
	}
	else {

		if (Sample::manager.release(sample->uri())) {

			::V8::AdjustAmountOfExternalAllocatedMemory(
				-size
			);
		}
	}
}

void v8Sample::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		FunctionTemplate::New(v8Sample::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Sample"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%play", v8Sample::Play);

	V8_SET_METHOD(constructor_template, "%load", v8Sample::Load);

	target->Set(String::NewSymbol("Sample"), constructor_template->GetFunction());
}

v8::Handle<v8::Value> v8Sample::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Sample(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

Handle<Object> v8Sample::New(Sample *sample) {
	HandleScope scope;

	Handle<Object> instance = constructor_template->GetFunction()->NewInstance();

	v8Sample *sampleWrapper = ObjectWrap::Unwrap<v8Sample>(instance);

	sampleWrapper->releaseSample();

	sampleWrapper->owns = false;

	sampleWrapper->sample = sample;

	return scope.Close(instance);
}

v8::Handle<v8::Value> v8Sample::Load(const v8::Arguments &args) {
	HandleScope scope;

	Handle<Function> fn = args[1].As<Function>();

	try {

		Handle<Object> sample = v8Sample::New(
			Sample::manager.load(
				V8::stringToStdString(args[0]->ToString())
			)
		);

		Handle<Value> argv[] = {
			sample
		};
		fn->Call(fn, 1, argv);
	}
	catch (std::exception &e) {

		return ThrowException(
			v8::Exception::Error(String::New(e.what()))
		);
	}

	return Undefined();
}

v8::Handle<v8::Value> v8Sample::Play(const v8::Arguments &args) {
	HandleScope scope;

	v8Sample *sampleWrapper = ObjectWrap::Unwrap<v8Sample>(args.Holder());

	if (NULL == sampleWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sample::play(): NULL Holder."
		)));
	}

	sampleWrapper->sample->play(
		args[0]->Int32Value()
	);

	return v8::Undefined();
}

Persistent<FunctionTemplate> v8Sample::constructor_template;

}

