#include "core/avocado-global.h"

#include "v8Canvas.h"

using namespace v8;

namespace avo {

v8Canvas::v8Canvas(Handle<Object> wrapper, Canvas *canvas)
	: canvas(canvas)
{
	if (NULL == this->canvas) {
		this->canvas = Canvas::factoryManager.instance()->create();
		::V8::AdjustAmountOfExternalAllocatedMemory(this->canvas->sizeInBytes());
	}

	Wrap(wrapper);
}

v8Canvas::~v8Canvas() {
	releaseCanvas();
}

void v8Canvas::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
		V8::avocadoIsolate,
		FunctionTemplate::New(v8Canvas::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Canvas"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%display"      , v8Canvas::Display      );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawFilledBox", v8Canvas::DrawFilledBox);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawCircle"   , v8Canvas::DrawCircle   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawLineBox"  , v8Canvas::DrawLineBox  );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%drawLine"     , v8Canvas::DrawLine     );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%fill"         , v8Canvas::Fill         );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%height"       , v8Canvas::Height       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%pixelAt"      , v8Canvas::PixelAt      );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%render"       , v8Canvas::Render       );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%saveToFile"   , v8Canvas::SaveToFile   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setPixelAt"   , v8Canvas::SetPixelAt   );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%width"        , v8Canvas::Width        );
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%uri"          , v8Canvas::Uri          );

	target->Set(v8::String::NewSymbol("Canvas"), constructor_template->GetFunction());
}

Canvas *v8Canvas::wrappedCanvas() {
	return canvas;
}

void v8Canvas::releaseCanvas() {
	::V8::AdjustAmountOfExternalAllocatedMemory(-canvas->sizeInBytes());
	delete canvas;
}

v8::Handle<v8::Value> v8Canvas::New(const v8::Arguments &args) {
	HandleScope scope;

	try {

		Canvas *canvas = NULL;

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

			canvas = Canvas::factoryManager.instance()->create(width, height);
		}

		new v8Canvas(args.Holder(), canvas);
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.This();
}

v8::Handle<v8::Value> v8Canvas::Display(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::display(): NULL Holder."
		)));
	}

	canvasWrapper->canvas->display();

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::DrawCircle(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::drawCircle(): NULL Holder."
		)));
	}

	Handle<Array> position = args[0].As<Array>();

	canvasWrapper->canvas->drawCircle(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		args[5]->Uint32Value(),
		static_cast<Canvas::DrawMode>(args[6]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::DrawFilledBox(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::drawFilledBox(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	canvasWrapper->canvas->drawFilledBox(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Canvas::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::DrawLine(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::drawLine(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	canvasWrapper->canvas->drawLine(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Canvas::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::DrawLineBox(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::drawLineBox(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	canvasWrapper->canvas->drawLineBox(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value(),
		args[4]->Uint32Value(),
		static_cast<Canvas::DrawMode>(args[5]->Uint32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::Fill(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::fill(): NULL Holder."
		)));
	}

	canvasWrapper->canvas->fill(
		args[0]->Uint32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value(),
		args[3]->Uint32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::Height(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::height(): NULL Holder."
		)));
	}

	return scope.Close(
		Integer::New(canvasWrapper->canvas->height())
	);
}

v8::Handle<v8::Value> v8Canvas::PixelAt(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::pixelAt(): NULL Holder."
		)));
	}

	return scope.Close(Integer::New(
		canvasWrapper->canvas->pixelAt(
			args[0]->Uint32Value(),
			args[1]->Uint32Value()
		)
	));
}

v8::Handle<v8::Value> v8Canvas::Render(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::render(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[4].As<Array>();
	Handle<Array> position = args[0].As<Array>();

	v8Canvas *destination = ObjectWrap::Unwrap<v8Canvas>(args[1]->ToObject());

	if (NULL == destination) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::render(): NULL destination."
		)));
	}

	canvasWrapper->canvas->render(
		position->Get(0)->Int32Value(),
		position->Get(1)->Int32Value(),
		destination->canvas,
		args[2]->Int32Value(),
		static_cast<Canvas::DrawMode>(args[3]->Int32Value()),
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value(),
		dimensions->Get(2)->Int32Value(),
		dimensions->Get(3)->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::SaveToFile(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::saveToFile(): NULL Holder."
		)));
	}

	std::string filename = V8::stringToStdString(args[0].As<String>());

	boost::filesystem::path parentPath = FS::qualifyPath(
		FS::resourceRoot(),
		boost::filesystem::path(
			filename
		).parent_path()
	);

	canvasWrapper->canvas->saveToFile(
		parentPath / boost::filesystem::path(filename).filename()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::SetPixelAt(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::setPixelAt(): NULL Holder."
		)));
	}

	canvasWrapper->canvas->setPixelAt(
		args[0]->Uint32Value(),
		args[1]->Uint32Value(),
		args[2]->Uint32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Canvas::Width(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::width(): NULL Holder."
		)));
	}

	return scope.Close(Integer::New(canvasWrapper->canvas->width()));
}

v8::Handle<v8::Value> v8Canvas::Uri(const v8::Arguments &args) {
	HandleScope scope;

	v8Canvas *canvasWrapper = ObjectWrap::Unwrap<v8Canvas>(args.Holder());

	if (NULL == canvasWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Canvas::uri(): NULL Holder."
		)));
	}

	return scope.Close(String::New(canvasWrapper->canvas->uri().string().c_str()));
}

Persistent<FunctionTemplate> v8Canvas::constructor_template;

}

