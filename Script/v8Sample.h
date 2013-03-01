#ifndef AVOCADO_V8SAMPLE_H
#define AVOCADO_V8SAMPLE_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include "ObjectWrap.h"
#include "SPI/Abstract/Sound/Sample.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Sample provides the Sample SPI to V8 JavaScript.
 *
 * @ingroup Sound
 * @ingroup SPI
 * @ingroup V8
 */
class v8Sample : public ObjectWrap {

public:

	~v8Sample();

	static void initialize(v8::Handle<v8::Object> target);

	static v8::Handle<v8::Object> New(Sample *sample);

private:

	v8Sample(v8::Handle<v8::Object> wrapper, Sample *sample = NULL);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	void releaseSample();

	static v8::Handle<v8::Value> Play(const v8::Arguments &args);

	static v8::Handle<v8::Value> Load(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Sample *sample;
	bool owns;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8SAMPLE_H
