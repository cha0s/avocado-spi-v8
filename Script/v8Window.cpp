#include "core/avocado-global.h"

#include "v8Window.h"

#include "v8Canvas.h"
#include "v8Image.h"

using namespace v8;

namespace avo {

v8Window::v8Window(Handle<Object> wrapper)
{
	window = Window::factoryManager.instance()->create();

	Wrap(wrapper);
}

v8Window::~v8Window() {
	delete window;
}

void v8Window::initialize(Handle<Object> target) {
	HandleScope scope;

	Handle<FunctionTemplate> constructor_template = FunctionTemplate::New(v8Window::New);
	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("Window"));

	V8_SET_PROTOTYPE_METHOD(constructor_template, "%display", v8Window::Display);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%pollEvents", v8Window::PollEvents);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%render", v8Window::Render);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setFlags", v8Window::SetFlags);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setSize", v8Window::SetSize);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setMouseVisibility", v8Window::SetMouseVisibility);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%setWindowTitle", v8Window::SetWindowTitle);
	V8_SET_PROTOTYPE_METHOD(constructor_template, "%size", v8Window::Size);

	std::map<std::string, int>::iterator i;

	Handle<Object> Mouse = Object::New();
	std::map<std::string, int> MouseButtons = Window::Event::MouseButtons();
	for (i = MouseButtons.begin(); i != MouseButtons.end(); i++) {
		Mouse->Set(
			String::NewSymbol(i->first.c_str()),
			Integer::New(i->second)
		);
	}
	constructor_template->Set(String::NewSymbol("Mouse"), Mouse);

	Handle<Object> KeyCode = Object::New();
	std::map<std::string, int> KeyCodes = Window::Event::KeyCodes();
	for (i = KeyCodes.begin(); i != KeyCodes.end(); i++) {
		KeyCode->Set(
			String::NewSymbol(i->first.c_str()),
			Integer::New(i->second)
		);
	}
	constructor_template->Set(String::NewSymbol("KeyCode"), KeyCode);

	target->Set(String::NewSymbol("Window"), constructor_template->GetFunction());
}

v8::Handle<v8::Value> v8Window::New(const v8::Arguments &args) {
	HandleScope scope;

	try {
		new v8Window(args.Holder());
	}
	catch (std::exception &e) {

		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			e.what()
		)));
	}

	return args.Holder();
}

v8::Handle<v8::Value> v8Window::Display(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());
	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::display(): NULL Holder."
		)));
	}

	windowWrapper->window->display();

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::PollEvents(const v8::Arguments &args) {
	HandleScope scope;

	Handle<Object> holder = args.Holder();
	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::pollEvents(): NULL Holder."
		)));
	}

	Window::Event event = windowWrapper->window->pollEvents();
	if (!event.empty()) {

		Handle<Function> emitFunction = args.Holder()->Get(
			String::New("emit")
		).As<Function>();

		Handle<Value> argv[2];
		Handle<Object> inputEvent;

		if (event.keyDown.size() > 0) {
			argv[0] = String::New("keyDown");
			for (unsigned int i = 0; i < event.keyDown.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("code"),
					Integer::New(event.keyDown[i].code)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.keyUp.size() > 0) {
			argv[0] = String::New("keyUp");
			for (unsigned int i = 0; i < event.keyUp.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("code"),
					Integer::New(event.keyUp[i].code)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.joyAxis.size() > 0) {
			argv[0] = String::New("joyAxis");
			for (unsigned int i = 0; i < event.joyAxis.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("stickIndex"),
					Integer::New(event.joyAxis[i].stickIndex)
				);
				inputEvent->Set(
					String::NewSymbol("axis"),
					Integer::New(event.joyAxis[i].axis)
				);
				inputEvent->Set(
					String::NewSymbol("value"),
					Number::New(event.joyAxis[i].value)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.joyButtonDown.size() > 0) {
			argv[0] = String::New("joyButtonDown");
			for (unsigned int i = 0; i < event.joyButtonDown.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("stickIndex"),
					Integer::New(event.joyButtonDown[i].stickIndex)
				);
				inputEvent->Set(
					String::NewSymbol("button"),
					Integer::New(event.joyButtonDown[i].button)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.joyButtonUp.size() > 0) {
			argv[0] = String::New("joyButtonUp");
			for (unsigned int i = 0; i < event.joyButtonUp.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("stickIndex"),
					Integer::New(event.joyButtonUp[i].stickIndex)
				);
				inputEvent->Set(
					String::NewSymbol("button"),
					Integer::New(event.joyButtonUp[i].button)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.mouseButtonDown.size() > 0) {
			argv[0] = String::New("mouseButtonDown");
			for (unsigned int i = 0; i < event.mouseButtonDown.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("button"),
					Integer::New(event.mouseButtonDown[i].button)
				);
				inputEvent->Set(
					String::NewSymbol("x"),
					Integer::New(event.mouseButtonDown[i].x)
				);
				inputEvent->Set(
					String::NewSymbol("y"),
					Integer::New(event.mouseButtonDown[i].y)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.mouseButtonUp.size() > 0) {
			argv[0] = String::New("mouseButtonUp");
			for (unsigned int i = 0; i < event.mouseButtonUp.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("button"),
					Integer::New(event.mouseButtonUp[i].button)
				);
				inputEvent->Set(
					String::NewSymbol("x"),
					Integer::New(event.mouseButtonUp[i].x)
				);
				inputEvent->Set(
					String::NewSymbol("y"),
					Integer::New(event.mouseButtonUp[i].y)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.mouseWheelMove.size() > 0) {
			argv[0] = String::New("mouseWheelMove");
			for (unsigned int i = 0; i < event.mouseWheelMove.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("delta"),
					Integer::New(event.mouseWheelMove[i].delta)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.mouseMove.size() > 0) {
			argv[0] = String::New("mouseMove");
			for (unsigned int i = 0; i < event.mouseMove.size(); i++) {
				inputEvent = Object::New();
				inputEvent->Set(
					String::NewSymbol("x"),
					Integer::New(event.mouseMove[i].x)
				);
				inputEvent->Set(
					String::NewSymbol("y"),
					Integer::New(event.mouseMove[i].y)
				);
				argv[1] = inputEvent;
				emitFunction->Call(holder, 2, argv);
			}
		}

		if (event.resize.width && event.resize.height) {
			argv[0] = String::New("resize");
			inputEvent = Object::New();
			inputEvent->Set(
				String::NewSymbol("x"),
				Integer::New(event.resize.width)
			);
			inputEvent->Set(
				String::NewSymbol("y"),
				Integer::New(event.resize.height)
			);
			argv[1] = inputEvent;
			emitFunction->Call(holder, 2, argv);
		}

		if (event.quit) {
			argv[0] = String::New("quit");
			inputEvent = Object::New();
			inputEvent->Set(
				String::NewSymbol("quit"),
				Boolean::New(true)
			);
			argv[1] = inputEvent;
			emitFunction->Call(holder, 2, argv);
		}
	}

	return scope.Close(Undefined());
}

v8::Handle<v8::Value> v8Window::Render(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::render(): NULL Holder."
		)));
	}

	// This is kinda strange, but I can't think of a simpler way to handle
	// dynamically accepting both Canvas and Image in a secure (e.g. not
	// depending on (mutable) JavaScript state to determine which to use) way.
	// We'll manually unwrap the internal pointer, and use RTTI to determine
	// which class of instance we have been passed.
	Handle<Object> instanceHandle = args[0]->ToObject();

	if (instanceHandle.IsEmpty() || 0 == instanceHandle->InternalFieldCount()) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::render(): NULL source."
		)));
	}

	ObjectWrap *instance = static_cast<ObjectWrap *>(
		instanceHandle->GetAlignedPointerFromInternalField(0)
	);

	v8Canvas *sourceCanvas = dynamic_cast<v8Canvas *>(instance);
	v8Image *sourceImage = dynamic_cast<v8Image *>(instance);

	if (!args[1]->IsArray()) {
		return ThrowException(v8::Exception::TypeError(String::NewSymbol(
			"Window::render(): second argument not array."
		)));
	}

	Handle<Array> rectangle = args[1].As<Array>();

	if (sourceImage) {
		windowWrapper->window->render(
			sourceImage->wrappedImage(),
			rectangle->Get(0)->Int32Value(),
			rectangle->Get(1)->Int32Value(),
			rectangle->Get(2)->Int32Value(),
			rectangle->Get(3)->Int32Value()
		);
	}
	else {
		windowWrapper->window->render(
			sourceCanvas->wrappedCanvas(),
			rectangle->Get(0)->Int32Value(),
			rectangle->Get(1)->Int32Value(),
			rectangle->Get(2)->Int32Value(),
			rectangle->Get(3)->Int32Value()
		);
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::SetFlags(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::setFlags(): NULL Holder."
		)));
	}

	windowWrapper->window->setFlags(
		static_cast<Window::WindowFlags>(args[0]->Int32Value())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::SetSize(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::setSize(): NULL Holder."
		)));
	}

	Handle<Array> dimensions = args[0].As<Array>();

	windowWrapper->window->setSize(
		dimensions->Get(0)->Int32Value(),
		dimensions->Get(1)->Int32Value()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::SetMouseVisibility(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::setMouseVisibility(): NULL Holder."
		)));
	}

	windowWrapper->window->setMouseVisibility(
		args[0]->BooleanValue()
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::SetWindowTitle(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::setWindowTitle(): NULL Holder."
		)));
	}

	windowWrapper->window->setWindowTitle(
		V8::stringToStdString(args[0]->ToString()),
		V8::stringToStdString(args[1]->ToString())
	);

	return v8::Undefined();
}

v8::Handle<v8::Value> v8Window::Size(const v8::Arguments &args) {
	HandleScope scope;

	v8Window *windowWrapper = ObjectWrap::Unwrap<v8Window>(args.Holder());

	if (NULL == windowWrapper) {
		return ThrowException(v8::Exception::ReferenceError(String::NewSymbol(
			"Window::size(): NULL Holder."
		)));
	}

	Handle<Array> size = Array::New();

	size->Set(0, Integer::New(windowWrapper->window->width()));
	size->Set(1, Integer::New(windowWrapper->window->height()));

	return scope.Close(size);
}

}

