#include "core/avocado-global.h"

#include "v8Canvas.h"
#include "v8Image.h"

using namespace v8;

namespace avo {

v8Image::v8Image(Handle<Object> wrapper)
	: image(NULL)
	, owns(false)
{
	this->image = Image::factoryManager.instance()->create();

	::V8::AdjustAmountOfExternalAllocatedMemory(this->image->sizeInBytes());

	owns = true;

	Wrap(wrapper);
}

v8Image::~v8Image() {
	releaseImage();
}

void v8Image::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		FunctionTemplate::New(v8Image::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Image"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%height"       , v8Image::Height       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%render"       , v8Image::Render       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%width"        , v8Image::Width        );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%uri"          , v8Image::Uri          );

	V8_SET_METHOD(constructor_template, "%load", v8Image::Load);

	target->Set(v8::String::NewSymbol("Image"), constructor_template->GetFunction());
}

Image *v8Image::wrappedImage() {
	return image;
}

void v8Image::releaseImage() {

	unsigned int size = image->sizeInBytes();

	if (owns) {

		::V8::AdjustAmountOfExternalAllocatedMemory(-size);

		delete image;
	}
	else {

		if (Image::manager.release(image->uri())) {

			::V8::AdjustAmountOfExternalAllocatedMemory(-size);
		}
	}
}

v8::Handle<v8::Value> v8Image::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Image(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

Handle<Object> v8Image::New(Image *image) {
	HandleScope scope;

	Handle<Object> instance = constructor_template->GetFunction()->NewInstance();

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(instance);

	imageWrapper->releaseImage();

	imageWrapper->owns = false;

	imageWrapper->image = image;

	return scope.Close(instance);
}

v8::Handle<v8::Value> v8Image::Load(const v8::Arguments &args) {
	HandleScope scope;

	Handle<Function> fn = args[1].As<Function>();

	try {

		Handle<Object> image;

		image = v8Image::New(
			Image::manager.load(
				V8::stringToStdString(args[0]->ToString())
			)
		);

		Handle<Value> argv[] = {
			Null(),
			image
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

v8::Handle<v8::Value> v8Image::Height(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::height(): NULL Holder."
		)));
	}

	return scope.Close(
		Integer::New(imageWrapper->image->height())
	);
}

v8::Handle<v8::Value> v8Image::Render(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::render(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[4].As<Array>();
	Handle<Array> position = args[0].As<Array>();

	v8Canvas *destination = ObjectWrap::Unwrap<v8Canvas>(args[1]->ToObject());

	if (NULL == destination) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::render(): NULL destination."
		)));
	}

	imageWrapper->image->render(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		destination->wrappedCanvas(),
		args[2]->Int32Value(),
		static_cast<Image::DrawMode>(args[3]->Int32Value()),
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::Width(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::width(): NULL Holder."
		)));
	}

	return scope.Close(Integer::New(imageWrapper->image->width()));
}

v8::Handle<v8::Value> v8Image::Uri(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::uri(): NULL Holder."
		)));
	}

	return scope.Close(String::New(
		FS::unqualifyPath(
			FS::resourceRoot(),
			imageWrapper->image->uri()
		).c_str()
	));
}

Persistent<FunctionTemplate> v8Image::constructor_template;

}

