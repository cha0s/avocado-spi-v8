#include "core/avocado-global.h"

#include "v8Canvas.h"
#include "v8Image.h"
#include "v8Sprite.h"
#include "v8Window.h"

using namespace v8;

namespace avo {

v8Sprite::v8Sprite(Handle<Object> wrapper)
	: sprite(NULL)
{
	if (NULL == this->sprite) {
		this->sprite = Sprite::factoryManager.instance()->create();
	}

	Wrap(wrapper);
}

v8Sprite::~v8Sprite() {
	delete sprite;
}

void v8Sprite::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		V8::avocadoIsolate,
		FunctionTemplate::New(v8Sprite::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Sprite"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%renderTo"          , v8Sprite::RenderTo          );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setAlpha"          , v8Sprite::SetAlpha          );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setBlendMode"      , v8Sprite::SetBlendMode      );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setPosition"       , v8Sprite::SetPosition       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setRotation"       , v8Sprite::SetRotation       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setScale"          , v8Sprite::SetScale          );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setSource"         , v8Sprite::SetSource         );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setSourceRectangle", v8Sprite::SetSourceRectangle);

	target->Set(v8::String::NewSymbol("Sprite"), constructor_template->GetFunction());
}

Sprite *v8Sprite::wrappedSprite() {
	return sprite;
}

v8::Handle<v8::Value> v8Sprite::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Sprite(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

v8::Handle<v8::Value> v8Sprite::RenderTo(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::renderTo(): NULL Holder."
		)));
	}

	// This is kinda strange, but I can't think of a simpler way to handle
	// dynamically accepting both Canvas and Window in a secure (e.g. not
	// depending on (mutable) JavaScript state to determine which to use) way.
	// We'll manually unwrap the internal pointer, and use RTTI to determine
	// which class of instance we have been passed.
	Handle<Object> instanceHandle = args[0]->ToObject();

	if (instanceHandle.IsEmpty() || 0 == instanceHandle->InternalFieldCount()) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::render(): NULL source."
		)));
	}

	ObjectWrap *instance = static_cast<ObjectWrap *>(
		instanceHandle->GetAlignedPointerFromInternalField(0)
	);

	v8Canvas *destinationCanvas = dynamic_cast<v8Canvas *>(instance);
	v8Window *destinationWindow = dynamic_cast<v8Window *>(instance);

	if (destinationCanvas) {
		spriteWrapper->sprite->renderTo(destinationCanvas->wrappedCanvas());
	}
	else {
		spriteWrapper->sprite->renderTo(destinationWindow->wrappedWindow());
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetAlpha(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setAlpha(): NULL Holder."
		)));
	}

	spriteWrapper->sprite->setAlpha(
		args[0]->NumberValue()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetBlendMode(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setBlendMode(): NULL Holder."
		)));
	}

	spriteWrapper->sprite->setAlpha(
		static_cast<GraphicsService::BlendMode>(args[0]->Int32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetPosition(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setPosition(): NULL Holder."
		)));
	}

	Handle<Array> position = args[0].As<Array>();

	spriteWrapper->sprite->setPosition(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetRotation(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setRotation(): NULL Holder."
		)));
	}

	spriteWrapper->sprite->setRotation(
		args[0]->NumberValue()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetScale(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setScale(): NULL Holder."
		)));
	}

	spriteWrapper->sprite->setScale(
		args[0]->NumberValue(),
		args[1]->NumberValue()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetSource(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setSource(): NULL Holder."
		)));
	}

	// This is kinda strange, but I can't think of a simpler way to handle
	// dynamically accepting both Canvas and Image in a secure (e.g. not
	// depending on (mutable) JavaScript state to determine which to use) way.
	// We'll manually unwrap the internal pointer, and use RTTI to determine
	// which class of instance we have been passed.
	Handle<Object> instanceHandle = args[0]->ToObject();

	if (instanceHandle.IsEmpty() || 0 == instanceHandle->InternalFieldCount()) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::render(): NULL source."
		)));
	}

	ObjectWrap *instance = static_cast<ObjectWrap *>(
		instanceHandle->GetAlignedPointerFromInternalField(0)
	);

	v8Canvas *destinationCanvas = dynamic_cast<v8Canvas *>(instance);
	v8Image *destinationImage = dynamic_cast<v8Image *>(instance);

	if (destinationCanvas) {
		spriteWrapper->sprite->setSource(destinationCanvas->wrappedCanvas());
	}
	else {
		spriteWrapper->sprite->setSource(destinationImage->wrappedImage());
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Sprite::SetSourceRectangle(const v8::Arguments &args) {
	HandleScope scope;

	v8Sprite *spriteWrapper = ObjectWrap::Unwrap<v8Sprite>(args.Holder());

	if (NULL == spriteWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Sprite::setSourceRectangle(): NULL Holder."
		)));
	}

	Handle<Array> rectangle = args[0].As<Array>();

	spriteWrapper->sprite->setSourceRectangle(
		rectangle->Get(0)->Int32Value(),
		rectangle->Get(1)->Int32Value(),
		rectangle->Get(2)->Int32Value(),
		rectangle->Get(3)->Int32Value()
	);

	return v8::Undefined();
}

Persistent<FunctionTemplate> v8Sprite::constructor_template;

}

