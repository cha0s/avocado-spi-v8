#ifndef AVOCADO_V8GRAPHICSSERVICE_H
#define AVOCADO_V8GRAPHICSSERVICE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Graphics/GraphicsService.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8GraphicsService handles providing the GraphicsService SPI to V8
 * JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8GraphicsService : public ObjectWrap {

public:

	~v8GraphicsService();

	static void initialize(v8::Handle<v8::Object> target);

	GraphicsService *graphicsService;

private:

	v8GraphicsService(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	/**
	 * Manage the GraphicsService SPI implementation.
	 */
	static v8::Handle<v8::Value> ImplementSpi(const v8::Arguments &args);

	static v8::Handle<v8::Value> Close(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8GRAPHICSSERVICE_H
