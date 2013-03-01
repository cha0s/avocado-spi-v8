#ifndef AVOCADO_V8COUNTER_H
#define AVOCADO_V8COUNTER_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Timing/Counter.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Counter handles providing the Counter SPI to V8 JavaScript.
 *
 * @ingroup SPI
 * @ingroup Timing
 * @ingroup V8
 */
class v8Counter : public ObjectWrap {

public:

	~v8Counter();

	static void initialize(v8::Handle<v8::Object> target);

	Counter *wrappedCounter();

private:

	v8Counter(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> Current(const v8::Arguments &args);
	static v8::Handle<v8::Value> Since(const v8::Arguments &args);

	Counter *counter;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8COUNTER_H
