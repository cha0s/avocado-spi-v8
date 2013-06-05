#ifndef AVOCADO_V8SPRITE_H
#define AVOCADO_V8SPRITE_H

#include "core/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "core/Graphics/Sprite.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Sprite handles providing the Sprite SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8Sprite : public ObjectWrap {

public:

	~v8Sprite();

	static void initialize(v8::Handle<v8::Object> target);

	Sprite *wrappedSprite();

private:

	v8Sprite(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> RenderTo(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetAlpha(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetBlendMode(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetPosition(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetRotation(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetScale(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetSource(const v8::Arguments &args);
	static v8::Handle<v8::Value> SetSourceRectangle(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Sprite *sprite;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8SPRITE_H
