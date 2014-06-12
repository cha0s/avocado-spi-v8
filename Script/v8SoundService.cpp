#include "avocado-global.h"

#include "v8SoundService.h"

#include "v8Music.h"
#include "v8Sample.h"

#ifdef AVOCADO_NODE

#include <node.h>
#include "SpiiLoader.h"

avo::SpiiLoader<avo::SoundService> spiiLoader;

#endif

using namespace v8;

namespace avo {

v8SoundService::v8SoundService(Handle<Object> wrapper)
{
	soundService = SoundService::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8SoundService::~v8SoundService() {
	delete soundService;
}

void v8SoundService::initialize(Handle<Object> target) {
	HandleScope scope;

	Handle<FunctionTemplate> constructor_template = FunctionTemplate::New(v8SoundService::New);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("SoundService"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "close", v8SoundService::Close);

#ifdef AVOCADO_NODE
	V8_SET_METHOD(constructor_template, "implementSpi", v8SoundService::ImplementSpi);
#endif

	target->Set(String::NewSymbol("SoundService"), constructor_template->GetFunction());

	v8Music::initialize(target);
	v8Sample::initialize(target);
}

v8::Handle<v8::Value> v8SoundService::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8SoundService(args.Holder());
	}
	catch (std::exception &e) {

		ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

#ifdef AVOCADO_NODE

v8::Handle<v8::Value> v8SoundService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	dlopen(
		(FS::exePath().string() + "/SPII/sound.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);

	try {

		// Attempt to load the SPII.
		spiiLoader.implementSpi(
			V8::stringToStdString(args[0]->ToString())
		);
	}
	catch (SpiiLoader<SoundService>::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
}

#endif

v8::Handle<v8::Value> v8SoundService::Close(const v8::Arguments &args) {
	HandleScope scope;

	v8SoundService *soundServiceWrapper = ObjectWrap::Unwrap<v8SoundService>(args.Holder());

	if (NULL == soundServiceWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"SoundService::close(): NULL Holder."
		)));
	}

	soundServiceWrapper->soundService->close();

	return v8::Undefined();
}

}


#ifdef AVOCADO_NODE

extern "C" {
	NODE_MODULE_EXPORT node::node_module_struct sound_module = {
		NODE_STANDARD_MODULE_STUFF,
		(node::addon_register_func) avo::v8SoundService::initialize,
		"%sound"
	};
}

#endif
