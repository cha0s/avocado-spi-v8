#include "main/avocado-global.h"

#include "v8TimingService.h"

#include "SPI/Abstract/SpiiLoader.h"

#include "v8Counter.h"

#ifdef AVOCADO_NODE
#include <node.h>
#endif

using namespace v8;

namespace avo {

avo::SpiiLoader<avo::TimingService> timingServiceSpiiLoader;

v8TimingService::v8TimingService(Handle<Object> wrapper)
{
	timingService = TimingService::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8TimingService::~v8TimingService() {
	delete timingService;
}

void v8TimingService::initialize(Handle<Object> target) {
	HandleScope scope;

	Handle<FunctionTemplate> constructor_template = FunctionTemplate::New(v8TimingService::New);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("TimingService"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "close", v8TimingService::Close);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%sleep", v8TimingService::Sleep);

	V8_SET_METHOD(constructor_template, "implementSpi", v8TimingService::ImplementSpi);

	target->Set(String::NewSymbol("TimingService"), constructor_template->GetFunction());

	v8Counter::initialize(target);
}

v8::Handle<v8::Value> v8TimingService::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8TimingService(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

v8::Handle<v8::Value> v8TimingService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	boost::filesystem::path spiiPath = args[1]->IsUndefined() ?
		FS::exePath()
	:
		V8::stringToStdString(args[1]->ToString())
	;

#ifdef AVOCADO_NODE
	dlopen(
		(spiiPath.string() + "/SPII/Timing.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);
#endif

	try {

		// Attempt to load the SPII.
		timingServiceSpiiLoader.implementSpi(
			V8::stringToStdString(args[0]->ToString()),
			spiiPath
		);
	}
	catch (SpiiLoader<TimingService>::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
}

v8::Handle<v8::Value> v8TimingService::Close(const v8::Arguments &args) {
	HandleScope scope;

	v8TimingService *timingServiceWrapper = ObjectWrap::Unwrap<v8TimingService>(args.Holder());

	if (NULL == timingServiceWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"TimingService::close(): NULL Holder."
		)));
	}

	timingServiceWrapper->timingService->close();

	return v8::Undefined();
}

v8::Handle<v8::Value> v8TimingService::Sleep(const v8::Arguments &args) {
	HandleScope scope;

	v8TimingService *timingServiceWrapper = ObjectWrap::Unwrap<v8TimingService>(args.Holder());

	timingServiceWrapper->timingService->sleep(
		args[0]->Int32Value()
	);

	return v8::Undefined();
}

}

#ifdef AVOCADO_NODE
NODE_MODULE(Timing, avo::v8TimingService::initialize)
#endif
