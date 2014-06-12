#include "avocado-global.h"

#include "v8GraphicsService.h"

#include "v8Canvas.h"
#include "v8Font.h"
#include "v8Image.h"
#include "v8Sprite.h"
#include "v8Window.h"

#ifdef AVOCADO_NODE
#include <node.h>
#endif

using namespace v8;

namespace avo {

v8GraphicsService::v8GraphicsService(Handle<Object> wrapper)
{
	graphicsService = GraphicsService::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8GraphicsService::~v8GraphicsService() {
	delete graphicsService;
}

void v8GraphicsService::initialize(Handle<Object> target) {
	HandleScope scope;

	constructor_template = Persistent<FunctionTemplate>::New(
#ifndef AVOCADO_NODE
		V8::avocadoIsolate,
#endif
		FunctionTemplate::New(v8GraphicsService::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("GraphicsService"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "close", v8GraphicsService::Close);

	target->Set(String::NewSymbol("GraphicsService"), constructor_template->GetFunction());

	avo::v8Canvas::initialize(target);
	avo::v8Font::initialize(target);
	avo::v8Image::initialize(target);
	avo::v8Sprite::initialize(target);
	avo::v8Window::initialize(target);
}

v8::Handle<v8::Value> v8GraphicsService::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8GraphicsService(args.Holder());
	}
	catch (std::exception &e) {

		ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

v8::Handle<v8::Value> v8GraphicsService::Close(const v8::Arguments &args) {
	HandleScope scope;

	v8GraphicsService *graphicsServiceWrapper = ObjectWrap::Unwrap<v8GraphicsService>(args.Holder());

	if (NULL == graphicsServiceWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"GraphicsSystem::close(): NULL Holder."
		)));
	}

	graphicsServiceWrapper->graphicsService->close();

	return v8::Undefined();
}

Persistent<FunctionTemplate> v8GraphicsService::constructor_template;

}

#ifdef AVOCADO_NODE

extern "C" {
	NODE_MODULE_EXPORT node::node_module_struct graphics_module = {
		NODE_STANDARD_MODULE_STUFF,
		(node::addon_register_func)regfunc,
		"%graphics"
	};
}

#endif
