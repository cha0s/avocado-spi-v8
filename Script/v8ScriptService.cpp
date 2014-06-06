#include "avocado-global.h"

#include <boost/regex.hpp>
#include <map>

#include "v8ScriptService.h"

#include "FS.h"
#include "Script/Script.h"
#include "v8Script.h"

#include "SpiiLoader.h"

#include "v8CoreService.h"
#include "v8GraphicsService.h"
#include "v8SoundService.h"
#include "v8TimingService.h"

using namespace boost;
using namespace v8;
using namespace std;

namespace avo {

AbstractFactory<v8ScriptService> *v8ScriptService::factory = new AbstractFactory<v8ScriptService>;

avo::SpiiLoader spiiLoader;

v8::Handle<v8::Value> ImplementSpi(const v8::Arguments &args) {
	HandleScope scope;

	std::string implementation = V8::stringToStdString(args[0]->ToString());
	std::string type = V8::stringToStdString(args[1]->ToString());

#ifdef AVOCADO_NODE
	dlopen(
		(spiiPath.string() + "/SPII/" + type + ".node").c_str(), RTLD_NOW | RTLD_GLOBAL
	);
#endif

	try {

		// Attempt to load the SPII.
		if ("Core" == type) {
			spiiLoader.implementSpi<CoreService>(implementation);
		}
		else if ("Graphics" == type) {
			spiiLoader.implementSpi<GraphicsService>(implementation);
		}
		else if ("Sound" == type) {
			spiiLoader.implementSpi<SoundService>(implementation);
		}
		else if ("Timing" == type) {
			spiiLoader.implementSpi<TimingService>(implementation);
		}
	}
	catch (SpiiLoader::spi_implementation_error &e) {

		// If it couldn't be loaded, throw an error.
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return Undefined();
}

#define INITIALIZE_SPI(name, Service) {                                                 \
    Handle<Object> name = Object::New();                                       \
    Handle<Object> name ## Module = Object::New();                             \
    Handle<Object> name ## Exports = Object::New();                            \
    name ## Module->Set(String::NewSymbol("exports"), name ## Exports);        \
    name->Set(String::NewSymbol("module"), name ## Module);                    \
    Service::initialize(name ## Exports);                        \
    requires_->Set(String::NewSymbol((string("%") + #name).c_str()), name); }

v8ScriptService::v8ScriptService()
	: ScriptService()
{

	Script::factoryManager.setInstance(v8Script::factory);

	HandleScope scope;

	V8::avocadoIsolate = Isolate::GetCurrent();

	Handle<ObjectTemplate> global = ObjectTemplate::New();

	global->Set(String::New("requires_"), ObjectTemplate::New());

	V8_SET_METHOD(global, "__implementSpi", ImplementSpi);

	context = Persistent<Context>::New(
		V8::avocadoIsolate,
		Context::New(V8::avocadoIsolate, NULL, global)
	);

	context->Enter();

	Handle<Object> requires_ = context->Global()->Get(
		String::NewSymbol("requires_")
	).As<Object>();

	INITIALIZE_SPI(core, v8CoreService);
	INITIALIZE_SPI(graphics, v8GraphicsService);
	INITIALIZE_SPI(sound, v8SoundService);
	INITIALIZE_SPI(timing, v8TimingService);

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
