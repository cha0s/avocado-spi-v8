#ifndef AVOCADO_V8IMAGE_H
#define AVOCADO_V8IMAGE_H

#include "core/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "core/Graphics/Image.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Image handles providing the Image SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8Image : public ObjectWrap {

public:

	~v8Image();

	static void initialize(v8::Handle<v8::Object> target);

	static v8::Handle<v8::Object> New(Image *image);

	Image *wrappedImage();

private:

	v8Image(v8::Handle<v8::Object> wrapper);

	void releaseImage();

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> Height(const v8::Arguments &args);
	static v8::Handle<v8::Value> Width(const v8::Arguments &args);
	static v8::Handle<v8::Value> Uri(const v8::Arguments &args);
	static v8::Handle<v8::Value> Load(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Image *image;
	bool owns;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8IMAGE_H
