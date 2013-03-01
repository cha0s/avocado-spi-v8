#ifndef AVOCADO_V8CORESERVICE_H
#define AVOCADO_V8CORESERVICE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Core/CoreService.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8CoreService handles providing the CoreService SPI to V8 JavaScript.
 *
 * @ingroup Core
 * @ingroup V8
 * @ingroup SPI
 */
class v8CoreService : public ObjectWrap {

public:

	~v8CoreService();

	static void initialize(v8::Handle<v8::Object> target);

	CoreService *coreService;

private:

	v8CoreService(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	/**
	 * Manage the CoreService SPI implementation.
	 */
	static v8::Handle<v8::Value> ImplementSpi(const v8::Arguments &args);

	/**
	 * Read a file into a string.
	 */
	static v8::Handle<v8::Value> ReadResource(const v8::Arguments& args);

	/**
	 * Set the path of the executable.
	 */
	static v8::Handle<v8::Value> SetExePath(const v8::Arguments& args);

	/**
	 * Set the root path of engine code.
	 */
	static v8::Handle<v8::Value> SetEngineRoot(const v8::Arguments& args);

	/**
	 * Set the root path of resources.
	 */
	static v8::Handle<v8::Value> SetResourceRoot(const v8::Arguments& args);

	/**
	 * Write JS values to stderr. Primitives get written verbatim, everything else
	 * gets JSON.stringify'd and written.
	 */
	static v8::Handle<v8::Value> WriteStderr(const v8::Arguments& args);

	static v8::Handle<v8::Value> Close(const v8::Arguments &args);
};

/**
 * @}
 */

}

#endif // AVOCADO_V8CORESERVICE_H
