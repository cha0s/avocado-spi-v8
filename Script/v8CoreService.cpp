#include "main/avocado-global.h"

#include "v8CoreService.h"

#include "SPI/Abstract/SpiiLoader.h"

#ifdef AVOCADO_NODE
#include <node.h>
#endif

using namespace v8;

namespace avo {

avo::SpiiLoader<avo::CoreService> coreServiceSpiiLoader;

v8CoreService::v8CoreService(Handle<Object> wrapper)
{
	coreService = CoreService::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8CoreService::~v8CoreService() {
	delete coreService;
}

void v8CoreService::initialize(Handle<Object> target) {
	HandleScope scope;

	Handle<FunctionTemplate> constructor_template = FunctionTemplate::New(v8CoreService::New);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("CoreService"));

	// Set methods.
	V8_SET_PROTOTYPE_METHOD(constructor_template, "close", v8CoreService::Close);

	V8_SET_METHOD(constructor_template, "implementSpi", v8CoreService::ImplementSpi);
	V8_SET_METHOD(constructor_template, "%readResource", v8CoreService::ReadResource);
	V8_SET_METHOD(constructor_template, "setExePath", v8CoreService::SetExePath);
	V8_SET_METHOD(constructor_template, "setEngineRoot", v8CoreService::SetEngineRoot);
	V8_SET_METHOD(constructor_template, "setResourceRoot", v8CoreService::SetResourceRoot);
	V8_SET_METHOD(constructor_template, "%writeStderr", v8CoreService::WriteStderr);

	// Register.
	target->Set(String::NewSymbol("CoreService"), constructor_template->GetFunction());
}

v8::Handle<v8::Value> v8CoreService::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8CoreService(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

v8::Handle<v8::Value> v8CoreService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	boost::filesystem::path spiiPath = args[1]->IsUndefined() ?
		FS::exePath()
	:
		V8::stringToStdString(args[1]->ToString())
	;

#ifdef AVOCADO_NODE
	dlopen(
		(spiiPath.string() + "/SPII/Core.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);
#endif

	try {

		// Attempt to load the SPII.
		coreServiceSpiiLoader.implementSpi(
			V8::stringToStdString(args[0]->ToString()),
			spiiPath
		);
	}
	catch (SpiiLoader<CoreService>::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
}

v8::Handle<v8::Value> v8CoreService::ReadResource(const v8::Arguments& args) {
	HandleScope scope;

	Handle<Function> fn = args[1].As<Function>();

	try {

		Handle<String> resource = String::New(
			FS::readString(
				FS::qualifyPath(
					FS::resourceRoot(),
					V8::stringToStdString(args[0].As<String>())
				)
			).c_str()
		);

		Handle<Value> argv[] = {
			resource
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

v8::Handle<v8::Value> v8CoreService::SetExePath(const v8::Arguments& args) {
	HandleScope scope;

	FS::setExePath(V8::stringToStdString(args[0].As<String>()));

	return Undefined();
}

v8::Handle<v8::Value> v8CoreService::SetEngineRoot(const v8::Arguments& args) {
	HandleScope scope;

	FS::setEngineRoot(V8::stringToStdString(args[0].As<String>()));

	return Undefined();
}

v8::Handle<v8::Value> v8CoreService::SetResourceRoot(const v8::Arguments& args) {
	HandleScope scope;

	FS::setResourceRoot(V8::stringToStdString(args[0].As<String>()));

	return Undefined();
}

v8::Handle<v8::Value> v8CoreService::WriteStderr(const v8::Arguments& args) {
	HandleScope scope;

	for (int i = 0; i < args.Length(); i++) {

		Handle<String> argString;

		// Primitives sent verbatim.
		if (args[i]->IsString() || args[i]->IsNumber()) {

			argString = args[i]->ToString();
		}
		else {

			// Try JSON.stringify...
			Handle<Value> jsonResult = V8::toJson(args[i]);
			if (jsonResult->IsUndefined()) {

				// If it bombed, then return the result; it's an exception.
				return jsonResult;
			}

			argString = jsonResult.As<String>();
		}

		// Stream it.
		std::cerr << *String::Utf8Value(argString) << std::endl;
	}

	return Undefined();
}

v8::Handle<v8::Value> v8CoreService::Close(const v8::Arguments &args) {
	HandleScope scope;

	// Check for fishyness.
	v8CoreService *coreServiceWrapper = ObjectWrap::Unwrap<v8CoreService>(args.Holder());
	if (NULL == coreServiceWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"CoreService::close(): NULL Holder."
		)));
	}

	coreServiceWrapper->coreService->close();

	return v8::Undefined();
}

}

#ifdef AVOCADO_NODE
NODE_MODULE(Core, avo::v8CoreService::initialize)
#endif
