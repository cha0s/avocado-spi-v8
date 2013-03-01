#include "main/avocado-global.h"

#include "v8Image.h"

using namespace v8;

namespace avo {

v8Image::v8Image(Handle<Object> wrapper, Image *image)
	: image(image)
	, owns(false)
{
	if (NULL == this->image) {

		this->image = Image::factoryManager.instance()->create();

		::V8::AdjustAmountOfExternalAllocatedMemory(
			this->image->sizeInBytes()
		);

		owns = true;
	}

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

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%display"      , v8Image::Display      );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawFilledBox", v8Image::DrawFilledBox);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawCircle"   , v8Image::DrawCircle   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawLineBox"  , v8Image::DrawLineBox  );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawLine"     , v8Image::DrawLine     );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%fill"         , v8Image::Fill         );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%height"       , v8Image::Height       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%pixelAt"      , v8Image::PixelAt      );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%render"       , v8Image::Render       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%saveToFile"   , v8Image::SaveToFile   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setPixelAt"   , v8Image::SetPixelAt   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%width"        , v8Image::Width        );

	V8_SET_METHOD(constructor_template, "%load", v8Image::Load);

	target->Set(v8::String::NewSymbol("Image"), constructor_template->GetFunction());
}

Image *v8Image::wrappedImage() {
	return image;
}

void v8Image::releaseImage() {

	unsigned int size = image->sizeInBytes();

	if (owns) {

		::V8::AdjustAmountOfExternalAllocatedMemory(
			-size
		);

		delete image;
	}
	else {

		if (Image::manager.release(image->uri())) {

			::V8::AdjustAmountOfExternalAllocatedMemory(
				-size
			);
		}
	}
}

v8::Handle<v8::Value> v8Image::New(const v8::Arguments &args) {
	HandleScope scope;

	try {

		Image *image = NULL;

		if (args.Length() > 0) {

			int width, height;

			if (args[0]->IsArray()) {
				width = args[0].As<Array>()->Get(0)->Int32Value();
				height = args[0].As<Array>()->Get(1)->Int32Value();
			}
			else {
				width = args[0]->Uint32Value();
				height = args[1]->Uint32Value();
			}

			image = Image::factoryManager.instance()->create(width, height);
		}

		new v8Image(args.Holder(), image);
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

		if (args[0]->IsString()) {

			image = v8Image::New(
				Image::manager.load(
					V8::stringToStdString(args[0]->ToString())
				)
			);
		}
		else {

			unsigned int length = args[0].As<Object>()->Get(
				String::NewSymbol("length")
			)->Uint32Value();

			unsigned char data[length];
			for (unsigned int i = 0; i < length; ++i) {
				data[i] = args[0].As<Object>()->Get(i)->Int32Value();
			}

			image = v8Image::New(
				Image::factoryManager.instance()->create(data, length)
			);
		}

		Handle<Value> argv[] = {
			image
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

v8::Handle<v8::Value> v8Image::Display(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::display(): NULL Holder."
		)));
	}

	imageWrapper->image->display();

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::DrawCircle(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::drawCircle(): NULL Holder."
		)));
	}

	Handle<Array> position = args[0].As<Array>();

	imageWrapper->image->drawCircle(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		args[5]->Uint32Value(),
		static_cast<Image::DrawMode>(args[6]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::DrawFilledBox(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::drawFilledBox(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	imageWrapper->image->drawFilledBox(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Image::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::DrawLine(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::drawLine(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	imageWrapper->image->drawLine(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Image::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::DrawLineBox(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::drawLineBox(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	imageWrapper->image->drawLineBox(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Image::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::Fill(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::fill(): NULL Holder."
		)));
	}

	imageWrapper->image->fill(
		args[0]->Uint32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value()
	);

	return v8::Undefined();
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

v8::Handle<v8::Value> v8Image::PixelAt(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::pixelAt(): NULL Holder."
		)));
	}

	return scope.Close(Integer::New(
		imageWrapper->image->pixelAt(
			args[0]->Uint32Value(),
			args[1]->Uint32Value()
		)
	));
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

	v8Image *destination = ObjectWrap::Unwrap<v8Image>(args[1]->ToObject());

	if (NULL == destination) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::render(): NULL destination."
		)));
	}

	imageWrapper->image->render(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		destination->image,
		args[2]->Int32Value(),
		static_cast<Image::DrawMode>(args[3]->Int32Value()),
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::SaveToFile(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::saveToFile(): NULL Holder."
		)));
	}

	std::string filename = V8::stringToStdString(args[0].As<String>());

	boost::filesystem::path parentPath = FS::qualifyPath(
		FS::resourceRoot(),
		boost::filesystem::path(
			filename
		).parent_path()
	);

	imageWrapper->image->saveToFile(
		parentPath / boost::filesystem::path(filename).filename()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Image::SetPixelAt(const v8::Arguments &args) {
	HandleScope scope;

	v8Image *imageWrapper = ObjectWrap::Unwrap<v8Image>(args.Holder());

	if (NULL == imageWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Image::setPixelAt(): NULL Holder."
		)));
	}

	imageWrapper->image->setPixelAt(
		args[0]->Uint32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value()
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

Persistent<FunctionTemplate> v8Image::constructor_template;

}

