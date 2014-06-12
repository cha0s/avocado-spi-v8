
#include <iostream>

#include "avocado-global.h"

#include "v8CoreService.h"

#include "FS.h"

#ifdef AVOCADO_NODE

#include <node.h>
#include "SpiiLoader.h"

avo::SpiiLoader spiiLoader;

#endif

using namespace v8;

namespace avo {

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

#ifdef AVOCADO_NODE
	V8_SET_METHOD(constructor_template, "implementSpi", v8CoreService::ImplementSpi);
#endif

	V8_SET_METHOD(constructor_template, "%readResource", v8CoreService::ReadResource);
	V8_SET_METHOD(constructor_template, "setExePath", v8CoreService::SetExePath);
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

#ifdef AVOCADO_NODE

v8::Handle<v8::Value> v8CoreService::ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	dlopen(
		(FS::exePath().string() + "/avocado-node/SPII/__core.node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);

	try {

		// Attempt to load the SPII.
		spiiLoader.implementSpi<avo::CoreService>(
			V8::stringToStdString(args[0]->ToString()),
			FS::exePath().string() + "/avocado-node"
		);
	}
	catch (SpiiLoader::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
}

#endif

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
			Null(),
			resource
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

v8::Handle<v8::Value> v8CoreService::SetExePath(const v8::Arguments& args) {
	HandleScope scope;

	FS::setExePath(V8::stringToStdString(args[0].As<String>()));

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
NODE_MODULE(__core, avo::v8CoreService::initialize)
#endif
