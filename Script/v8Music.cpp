#include "main/avocado-global.h"

#include "v8Music.h"

using namespace v8;

namespace avo {

v8Music::v8Music(Handle<Object> wrapper, Music *music)
	: music(music)
	, owns(false)
{
	if (NULL == this->music) {

		this->music = Music::factoryManager.instance()->create();

		::V8::AdjustAmountOfExternalAllocatedMemory(
			this->music->sizeInBytes()
		);

		owns = true;
	}

	Wrap(wrapper);
}

v8Music::~v8Music() {
	releaseMusic();
}

void v8Music::releaseMusic() {

	unsigned int size = music->sizeInBytes();

	if (owns) {

		::V8::AdjustAmountOfExternalAllocatedMemory(
			-size
		);

		delete music;
	}
	else {

		if (Music::manager.release(music->uri())) {

			::V8::AdjustAmountOfExternalAllocatedMemory(
				-size
			);
		}
	}
}

void v8Music::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		FunctionTemplate::New(v8Music::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Music"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%fadeIn", v8Music::FadeIn);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%fadeOut", v8Music::FadeOut);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%play", v8Music::Play);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%stop", v8Music::Stop);

	V8_SET_METHOD(constructor_template, "%load", v8Music::Load);

	target->Set(String::NewSymbol("Music"), constructor_template->GetFunction());
}

v8::Handle<v8::Value> v8Music::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Music(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

Handle<Object> v8Music::New(Music *music) {
	HandleScope scope;

	Handle<Object> instance = constructor_template->GetFunction()->NewInstance();

	v8Music *musicWrapper = ObjectWrap::Unwrap<v8Music>(instance);

	musicWrapper->releaseMusic();

	musicWrapper->owns = false;

	musicWrapper->music = music;

	return scope.Close(instance);
}

v8::Handle<v8::Value> v8Music::Load(const v8::Arguments &args) {
	HandleScope scope;

	Handle<Function> fn = args[1].As<Function>();

	try {

		Handle<Object> music = v8Music::New(
			Music::manager.load(
				V8::stringToStdString(args[0]->ToString())
			)
		);

		Handle<Value> argv[] = {
			music
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

v8::Handle<v8::Value> v8Music::FadeIn(const v8::Arguments &args) {
	HandleScope scope;

	v8Music *musicWrapper = ObjectWrap::Unwrap<v8Music>(args.Holder());

	if (NULL == musicWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Music::fadeIn(): NULL Holder."
		)));
	}

	musicWrapper->music->fadeIn(
		args[0]->Int32Value(),
		args[1]->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Music::FadeOut(const v8::Arguments &args) {
	HandleScope scope;

	v8Music *musicWrapper = ObjectWrap::Unwrap<v8Music>(args.Holder());

	if (NULL == musicWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Music::fadeOut(): NULL Holder."
		)));
	}

	musicWrapper->music->fadeOut(
		args[0]->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Music::Play(const v8::Arguments &args) {
	HandleScope scope;

	v8Music *musicWrapper = ObjectWrap::Unwrap<v8Music>(args.Holder());

	if (NULL == musicWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Music::play(): NULL Holder."
		)));
	}

	musicWrapper->music->play(
		args[0]->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Music::Stop(const v8::Arguments &args) {
	HandleScope scope;

	v8Music *musicWrapper = ObjectWrap::Unwrap<v8Music>(args.Holder());

	if (NULL == musicWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Music::stop(): NULL Holder."
		)));
	}

	musicWrapper->music->stop();

	return v8::Undefined();
}

Persistent<FunctionTemplate> v8Music::constructor_template;

}
