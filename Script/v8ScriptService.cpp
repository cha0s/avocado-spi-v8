#include "core/avocado-global.h"

#include <boost/regex.hpp>

#include "v8ScriptService.h"

#include "core/FS.h"
#include "core/Script/Script.h"
#include "v8Script.h"

#include "v8CoreService.h"
#include "v8GraphicsService.h"
#include "v8SoundService.h"
#include "v8TimingService.h"

using namespace boost;
using namespace v8;
using namespace std;

namespace avo {

AbstractFactory<v8ScriptService> *v8ScriptService::factory = new AbstractFactory<v8ScriptService>;

#define INITIALIZE_SPI(name) { \
    Handle<Object> name = Object::New(); \
    Handle<Object> name ## Module = Object::New(); \
    Handle<Object> name ## Exports = Object::New(); \
    name ## Module->Set(String::NewSymbol("exports"), name ## Exports); \
    name->Set(String::NewSymbol("module"), name ## Module); \
    v8 ## name ## Service::initialize(name ## Exports); \
    requires_->Set(String::NewSymbol(#name), name); }

v8ScriptService::v8ScriptService()
	: ScriptService()
{

	Script::factoryManager.setInstance(v8Script::factory);

	HandleScope scope;

	V8::avocadoIsolate = Isolate::GetCurrent();

	Handle<ObjectTemplate> global = ObjectTemplate::New();

	global->Set(String::New("requires_"), ObjectTemplate::New());

	context = Persistent<Context>::New(
#ifndef AVOCADO_NODE
		V8::avocadoIsolate,
#endif
		Context::New(V8::avocadoIsolate, NULL, global)
	);

	context->Enter();

	Handle<Object> requires_ = context->Global()->Get(
		String::NewSymbol("requires_")
	).As<Object>();

	INITIALIZE_SPI(Core);
	INITIALIZE_SPI(Graphics);
	INITIALIZE_SPI(Sound);
	INITIALIZE_SPI(Timing);

	ScriptService::initialize();
}

std::string v8ScriptService::preCompileCode(const std::string &code, const boost::filesystem::path &filename) {
	HandleScope scope;

	std::string filenameString = filename.string();
	bool isCoffee = false;
	bool isLiterateCoffee = false;

	if (boost::regex_search(
		filenameString,
		regex(".*\\.coffee$")
	)) {
		isCoffee = true;
	}

	if (boost::regex_search(
		filenameString,
		regex(".*\\.litcoffee$")
	)) {
		isCoffee = true;
		isLiterateCoffee = true;
	}

	// Compile coffeescript to JS.
	if (isCoffee) {

		TryCatch exception;

		Handle<Object> CoffeeScript = Context::GetCurrent()->Global()->Get(String::New("CoffeeScript")).As<Object>();
		Handle<Function> compile = CoffeeScript->Get(String::New("compile")).As<Function>();

		Handle<Object> options = Object::New();
		options->Set(String::New("filename"), String::New(
			filenameString.c_str()
		));
		options->Set(String::New("literate"), Boolean::New(isLiterateCoffee));
		Handle<Value> args[] = {
			String::New(code.c_str()),
			options
		};

		Handle<Value> result = compile->Call(compile, 2, args);

		if (exception.HasCaught()) {
			throw script_precompilation_error(V8::stringifyException(exception, true));
		}

		return V8::stringToStdString(result->ToString());
	}
	else {

		return code;
	}
}

Script *v8ScriptService::scriptFromCode(const std::string &code, const boost::filesystem::path &filename) {
	HandleScope scope;

	// Instantiate the v8 script.
	TryCatch exception;
	Handle<v8::Script> script = v8::Script::New(
		String::New(code.c_str()),
		String::New(filename.string().c_str())
	);
	if (exception.HasCaught()) {

		throw script_compilation_error(
			V8::stringifyException(exception),
			code
		);
	}

	// Cast and ensure the factory is correct.
	AbstractFactory<v8Script> *v8ScriptFactory;
	v8ScriptFactory = dynamic_cast<AbstractFactory<v8Script> *>(
		Script::factoryManager.instance()
	);
	if (NULL == v8ScriptFactory) {
		throw script_compilation_error("Concrete v8 factory mismatch!");
	}

	// Instantiate our script and return it.
	return v8ScriptFactory->create(script);
}

v8ScriptService::~v8ScriptService() {

	// Leave the context and dispose of it.
	context->Exit();
	context.Dispose();
}

}
