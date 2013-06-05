#ifndef AVOCADO_V8CANVAS_H
#define AVOCADO_V8CANVAS_H

#include "core/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "core/Graphics/Canvas.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Canvas handles providing the Canvas SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8Canvas : public ObjectWrap {

public:

	~v8Canvas();

	static void initialize(v8::Handle<v8::Object> target);

	Canvas *wrappedCanvas();

private:

	v8Canvas(v8::Handle<v8::Object> wrapper, Canvas *canvas = NULL);

	void releaseCanvas();

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> Display(const v8::Arguments &args);
	static v8::Handle<v8::Value> DrawFilledBox(const v8::Arguments &args);
	static v8::Handle<v8::Value> DrawCircle(const v8::Arguments &args);
	static v8::Handle<v8::Value> DrawLine(const v8::Arguments &args);
	static v8::Handle<v8::Value> DrawLineBox(const v8::Arguments &args);
	static v8::Handle<v8::Value> Fill(const v8::Arguments &args);
	static v8::Handle<v8::Value> Height(const v8::Arguments &args);
	static v8::Handle<v8::Value> PixelAt(const v8::Arguments &args);
	static v8::Handle<v8::Value> SaveToFile(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetPixelAt(const v8::Arguments &args);
	static v8::Handle<v8::Value> Width(const v8::Arguments &args);
	static v8::Handle<v8::Value> Uri(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Canvas *canvas;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8CANVAS_H
