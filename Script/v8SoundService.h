#ifndef AVOCADO_V8SOUNDSERVICE_H
#define AVOCADO_V8SOUNDSERVICE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Sound/SoundService.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8SoundService provides the SoundService SPI to V8 JavaScript.
 *
 * @ingroup Sound
 * @ingroup SPI
 * @ingroup V8
 */
class v8SoundService : public ObjectWrap {

public:

	~v8SoundService();

	static void initialize(v8::Handle<v8::Object> target);

	SoundService *soundService;

private:

	v8SoundService(v8::Handle<v8::Object> wrapper);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	static v8::Handle<v8::Value> ImplementSpi(const v8::Arguments &args);

	static v8::Handle<v8::Value> Close(const v8::Arguments &args);
};

/**
 * @}
 */

}

#endif // AVOCADO_V8SOUNDSERVICE_H
