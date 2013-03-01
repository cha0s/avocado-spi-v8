#include "main/avocado-global.h"

#include "v8GraphicsService.h"

#include "SPI/Abstract/SpiiLoader.h"

#include "v8Font.h"
#include "v8Image.h"
#include "v8Window.h"

#ifdef AVOCADO_NODE
#include <node.h>
#endif

using namespace v8;

namespace avo {

#define SPECIAL_KEY(Keys, keys, key)                \
    Keys->Set(                   \
        String::New(#key),              \
        Integer::New(keys.key) \
    );

avo::SpiiLoader<avo::GraphicsService> graphicsServiceSpiiLoader;

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
		FunctionTemplate::New(v8GraphicsService::New)
	);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("GraphicsService"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "close", v8GraphicsService::Close);

	V8_SET_METHOD(constructor_template, "implementSpi", v8GraphicsService::ImplementSpi);

	target->Set(String::NewSymbol("GraphicsService"), constructor_template->GetFunction());

	avo::v8Font::initialize(target);
	avo::v8Image::initialize(target);
	avo::v8Window::initialize(target);
}

v8::Handle<v8::Value> v8GraphicsService::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		v8GraphicsService *graphicsServiceWrapper = new v8GraphicsService(args.Holder());

		Handle<Object> SpecialKeyCodes = Object::New();
		GraphicsService::SpecialKeyCodes specialKeys = graphicsServiceWrapper->graphicsService->specialKeyCodes();

		SPECIAL_KEY(SpecialKeyCodes, specialKeys, UpArrow);
		SPECIAL_KEY(SpecialKeyCodes, specialKeys, RightArrow);
		SPECIAL_KEY(SpecialKeyCodes, specialKeys, DownArrow);
		SPECIAL_KEY(SpecialKeyCodes, specialKeys, LeftArrow);

		args.Holder()->Set(String::New("SpecialKeyCodes"), SpecialKeyCodes);

	}
	catch (std::exception &e) {

		ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

v8::Handle<v8::Value> v8GraphicsService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	boost::filesystem::path spiiPath = args[1]->IsUndefined() ?
		FS::exePath()
	:
		V8::stringToStdString(args[1]->ToString())
	;

#ifdef AVOCADO_NODE
	dlopen(
		(spiiPath.string() + "/SPII/Graphics.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);
#endif

	try {

		// Attempt to load the SPII.
		graphicsServiceSpiiLoader.implementSpi(
			V8::stringToStdString(args[0]->ToString()),
			spiiPath
		);
	}
	catch (SpiiLoader<GraphicsService>::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
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
NODE_MODULE(Graphics, avo::v8GraphicsService::initialize)
#endif
