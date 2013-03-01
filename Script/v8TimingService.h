#ifndef AVOCADO_V8TIMINGSERVICE_H
#define AVOCADO_V8TIMINGSERVICE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Timing/TimingService.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8TimingService provides the TimingService SPI to V8 JavaScript.
 *
 * @ingroup Graphics
 * @ingroup SPI
 * @ingroup V8
 */
class v8TimingService : public ObjectWrap {

public:

	~v8TimingService();

	static void initialize(v8::Handle<v8::Object> target);

	TimingService *timingService;

private:

	v8TimingService(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	/**
	 * Manage the TimingService SPI implementation.
	 */
	static v8::Handle<v8::Value> ImplementSpi(const v8::Arguments &args);

	static v8::Handle<v8::Value> Close(const v8::Arguments &args);
	static v8::Handle<v8::Value> Sleep(const v8::Arguments &args);
};

/**
 * @}
 */

}

#endif // AVOCADO_V8TIMINGSERVICE_H
