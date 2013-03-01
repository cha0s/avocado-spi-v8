#include "main/avocado-global.h"

#include "avocado-v8.h"

#include <iostream>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace v8;

namespace avo {

namespace V8 {

std::string stringToStdString(v8::Handle<v8::String> value) {
  String::Utf8Value utf8_value(value);

  return *utf8_value ? *utf8_value : "<string conversion failed>";
}

std::string stringifyException(const v8::TryCatch& try_catch, bool suppressBacktrace) {
	HandleScope scope;
	std::string output = "";

	Handle<Message> message = try_catch.Message();
	if (suppressBacktrace || message.IsEmpty()) {

		std::string exception_string = V8::stringToStdString(try_catch.Exception().As<String>());

		// V8 didn't provide any extra information about this error; just
		// print the exception.
		output += exception_string;
		output += "\n";

	} else {

		// filename:line
		output += V8::stringToStdString(
			message->GetScriptResourceName().As<String>()
		) + ":" + boost::lexical_cast<std::string>(
			message->GetLineNumber()
		) + "\n";

		// Print line of source code.
		output += V8::stringToStdString(message->GetSourceLine()) + "\n";

		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
			output += " ";
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
			output += "^";
		}
		output += "\n";

		// Print out the stack trace.
		std::string stackTrace = V8::stringToStdString(try_catch.StackTrace().As<String>());
		if (stackTrace.length() > 0) {
			output += stackTrace + "\n";
		}
	}

	return output;
}

v8::Handle<v8::Value> toJson(v8::Handle<v8::Value> value) {
	HandleScope scope;

	Handle<Context> context = Context::GetCurrent();
	Handle<Object> global = context->Global();
	Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	Handle<Function> JSON_stringify = Handle<Function>::Cast(JSON->Get(String::New("stringify")));

	// JSON.stringify(values, null, '\t');
	Handle<Value> argv[] = {
		value,
		Null(),
		String::New("\t")
	};

	TryCatch exception;

	Handle<Value> stringified = JSON_stringify->Call(JSON_stringify, 3, argv);

	if (exception.HasCaught()) {
		return exception.ReThrow();
	}

	return scope.Close(stringified);
}

}

}
