#include "avocado-global.h"

#include "v8Canvas.h"
#include "v8Font.h"
#include "v8Image.h"

using namespace v8;

namespace avo {

v8Font::v8Font(Handle<Object> wrapper, Font *font)
	: font(font)
	, owns(false)
{
	if (NULL == this->font) {

		this->font = Font::factoryManager.instance()->create();

		::V8::AdjustAmountOfExternalAllocatedMemory(this->font->sizeInBytes());

		owns = true;
	}

	Wrap(wrapper);
}

v8Font::~v8Font() {
	releaseFont();
}

void v8Font::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		V8::avocadoIsolate,
		FunctionTemplate::New(v8Font::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Font"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%render"     , v8Font::Render    );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setSize"    , v8Font::SetSize   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setStyle"   , v8Font::SetStyle  );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%textHeight" , v8Font::TextHeight);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%textWidth"  , v8Font::TextWidth );

	V8_SET_METHOD(constructor_template, "%load", v8Font::Load);

	target->Set(v8::String::NewSymbol("Font"), constructor_template->GetFunction());
}

Font *v8Font::wrappedFont() {
	return font;
}

void v8Font::releaseFont() {

	unsigned int size = font->sizeInBytes();

	if (owns) {

		::V8::AdjustAmountOfExternalAllocatedMemory(-size);

		delete font;
	}
	else {

		if (Font::manager.release(font->uri())) {

			::V8::AdjustAmountOfExternalAllocatedMemory(-size);
		}
	}
}

v8::Handle<v8::Value> v8Font::New(const v8::Arguments &args) {
	HandleScope scope;

	try {

		new v8Font(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

Handle<Object> v8Font::New(Font *font) {
	HandleScope scope;

	Handle<Object> instance = constructor_template->GetFunction()->NewInstance();

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(instance);

	fontWrapper->releaseFont();

	fontWrapper->owns = false;

	fontWrapper->font = font;

	return scope.Close(instance);
}

v8::Handle<v8::Value> v8Font::Load(const v8::Arguments &args) {
	HandleScope scope;

	Handle<Function> fn = args[1].As<Function>();

	try {

		Handle<Object> font = v8Font::New(
			Font::manager.load(
				V8::stringToStdString(args[0]->ToString())
			)
		);

		Handle<Value> argv[] = {
			Null(),
			font
		};
		fn->Call(fn, 2, argv);
	}
	catch (std::exception &e) {

		Handle<Value> argv[] = {
			v8::Exception::Error(String::New(e.what())),
			Null()
		};
		fn->Call(fn, 2, argv);
	}

	return Undefined();
}

v8::Handle<v8::Value> v8Font::Render(const v8::Arguments &args) {
	HandleScope scope;

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(args.Holder());

	if (NULL == fontWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::render(): NULL Holder."
		)));
	}

	v8Canvas *destination = ObjectWrap::Unwrap<v8Canvas>(args[2]->ToObject());

	if (NULL == destination) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::render(): NULL destination."
		)));
	}

	Handle<Array> position = args[0].As<Array>();

	fontWrapper->font->render(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		V8::stringToStdString(args[1].As<String>()),
		destination->wrappedCanvas(),
		args[3]->Int32Value(),
		args[4]->Int32Value(),
		args[5]->Int32Value(),
		args[6]->NumberValue()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Font::SetSize(const v8::Arguments &args) {
	HandleScope scope;

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(args.Holder());

	if (NULL == fontWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::setSize(): NULL Holder."
		)));
	}

	fontWrapper->font->setSize(
		args[0]->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Font::SetStyle(const v8::Arguments &args) {
	HandleScope scope;

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(args.Holder());

	if (NULL == fontWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::drawFilledBox(): NULL Holder."
		)));
	}

	fontWrapper->font->setStyle(
		static_cast<Font::FontStyle>(args[0]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Font::TextHeight(const v8::Arguments &args) {
	HandleScope scope;

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(args.Holder());

	if (NULL == fontWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::textHeight(): NULL Holder."
		)));
	}

	return Integer::New(
		fontWrapper->font->textHeight(
			V8::stringToStdString(args[0].As<String>())
		)
	);
}

v8::Handle<v8::Value> v8Font::TextWidth(const v8::Arguments &args) {
	HandleScope scope;

	v8Font *fontWrapper = ObjectWrap::Unwrap<v8Font>(args.Holder());

	if (NULL == fontWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Font::textWidth(): NULL Holder."
		)));
	}

	return Integer::New(
		fontWrapper->font->textWidth(
			V8::stringToStdString(args[0].As<String>())
		)
	);
}

Persistent<FunctionTemplate> v8Font::constructor_template;

}

