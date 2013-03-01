#ifndef AVOCADO_V8SCRIPTSERVICE_H
#define AVOCADO_V8SCRIPTSERVICE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"

#include "SPI/Abstract/Script/ScriptService.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8ScriptService embeds the
 * [V8 JavaScript engine](http://code.google.com/p/v8/).
 *
 * @ingroup SPI
 * @ingroup V8
 */
class v8ScriptService : public ScriptService {

public:

	v8ScriptService();
	~v8ScriptService();

	/**
	 * Precompile the passed in code, inferring any precompiler from the
	 * extension of the filename passed in, if any.
	 */
	std::string preCompileCode(const std::string &code, const boost::filesystem::path &filename = "<anonymous>");

	/**
	 * Compile a string of code, setting the filename, if any.
	 */
	Script *scriptFromCode(const std::string &code, const boost::filesystem::path &filename = "<anonymous>");

	/**
	 * Concrete ScriptService factory instance.
	 */
	static AbstractFactory<v8ScriptService> *factory;

private:

	v8::Persistent<v8::Context> context;

};

/**
 * @ingroup Manufacturing
 * @ingroup SPI
 * @ingroup V8
 */
template <>
class AbstractFactory<v8ScriptService> : public AbstractFactory<ScriptService> {

public:

	virtual ~AbstractFactory<v8ScriptService>() {}

	/**
	 * Create a concrete ScriptService.
	 */
	v8ScriptService *create() {
		return new v8ScriptService();
	}

};

/**
 * @}
 */

}

#endif // AVOCADO_V8SCRIPTSERVICE_H
