#ifndef AVOCADO_V8FONT_H
#define AVOCADO_V8FONT_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Graphics/Font.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Font handles providing the Font SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8Font : public ObjectWrap {

public:

	~v8Font();

	static void initialize(v8::Handle<v8::Object> target);

	static v8::Handle<v8::Object> New(Font *font);

	Font *wrappedFont();

private:

	v8Font(v8::Handle<v8::Object> wrapper, Font *font = NULL);

	void releaseFont();

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> Load(const v8::Arguments &args);
	static v8::Handle<v8::Value> Render(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetSize(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetStyle(const v8::Arguments &args);
	static v8::Handle<v8::Value> TextHeight(const v8::Arguments &args);
	static v8::Handle<v8::Value> TextWidth(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Font *font;
	bool owns;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8FONT_H
