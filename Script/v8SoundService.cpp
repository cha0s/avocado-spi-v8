#include "main/avocado-global.h"

#include "v8SoundService.h"

#include "SPI/Abstract/SpiiLoader.h"

#include "v8Music.h"
#include "v8Sample.h"

#ifdef AVOCADO_NODE
#include <node.h>
#endif

using namespace v8;

namespace avo {

avo::SpiiLoader<avo::SoundService> soundServiceSpiiLoader;

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

	V8_SET_METHOD(constructor_template, "implementSpi", v8SoundService::ImplementSpi);

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

v8::Handle<v8::Value> v8SoundService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	boost::filesystem::path spiiPath = args[1]->IsUndefined() ?
		FS::exePath()
	:
		V8::stringToStdString(args[1]->ToString())
	;

#ifdef AVOCADO_NODE
	dlopen(
		(spiiPath.string() + "/SPII/Sound.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);
#endif

	try {

		// Attempt to load the SPII.
		soundServiceSpiiLoader.implementSpi(
			V8::stringToStdString(args[0]->ToString()),
			spiiPath
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
NODE_MODULE(Sound, avo::v8SoundService::initialize)
#endif
