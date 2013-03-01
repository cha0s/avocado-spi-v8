#ifndef AVOCADO_V8WINDOW_H
#define AVOCADO_V8WINDOW_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Graphics/Window.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Window provides the Window SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8Window : public ObjectWrap {

public:

	~v8Window();

	static void initialize(v8::Handle<v8::Object> target);

	Window *window;

private:

	v8Window(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> Display(const v8::Arguments &args);
	static v8::Handle<v8::Value> PollEvents(const v8::Arguments &args);
	static v8::Handle<v8::Value> Render(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetFlags(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetSize(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetMouseVisibility(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetWindowTitle(const v8::Arguments &args);
	static v8::Handle<v8::Value> Size(const v8::Arguments &args);

};

/**
 * @}
 */

}

#endif // AVOCADO_V8WINDOW_H
