#ifndef AVOCADO_V8MUSIC_H
#define AVOCADO_V8MUSIC_H

#include "main/avocado-global.h"

#include "SPI/Abstract/Sound/Music.h"
#include "ObjectWrap.h"
#include "avocado-v8.h"

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * %v8Music provides the Music SPI to V8 JavaScript.
 *
 * @ingroup Sound
 * @ingroup SPI
 * @ingroup V8
 */
class v8Music : public ObjectWrap {

public:

	~v8Music();

	static void initialize(v8::Handle<v8::Object> target);

	static v8::Handle<v8::Object> New(Music *music);

private:

	v8Music(v8::Handle<v8::Object> wrapper, Music *music = NULL);

	static v8::Handle<v8::Value> New(const v8::Arguments &args);

	void releaseMusic();

	static v8::Handle<v8::Value> FadeIn(const v8::Arguments &args);
	static v8::Handle<v8::Value> FadeOut(const v8::Arguments &args);
	static v8::Handle<v8::Value> Load(const v8::Arguments &args);
	static v8::Handle<v8::Value> Play(const v8::Arguments &args);
	static v8::Handle<v8::Value> Stop(const v8::Arguments &args);

	static v8::Persistent<v8::FunctionTemplate> constructor_template;

	Music *music;
	bool owns;
};

/**
 * @}
 */

}

#endif // AVOCADO_V8MUSIC_H

