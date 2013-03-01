#ifndef AVOCADO_OBJECTWRAP_H
#define AVOCADO_OBJECTWRAP_H

#include "main/avocado-global.h"

#include "avocado-v8.h"
#include <assert.h>

namespace avo {

/**
 * @addtogroup Script
 * @{
 */

/**
 * Wrap an object for ease going in/out of v8. Lovingly stolen with highest
 * respect from NodeJS.
 *
 * @ingroup V8
 */
class ObjectWrap {

public:

	/** Called at GC. */
	virtual ~ObjectWrap();

	/**
	 * Get the T pointer from inside an object.
	 */
	template <class T>
	static inline T* Unwrap(v8::Handle<v8::Object> handle) {

		// Ensure there's an object pointer in here.
		if (handle.IsEmpty() || 0 == handle->InternalFieldCount()) return NULL;

		// Return the object pointer.
		return static_cast<T *>(handle->GetAlignedPointerFromInternalField(0));
	}

protected:

	/**
	 * Wrap this class with the passed in object.
	 */
	inline void Wrap(v8::Handle<v8::Object> handle) {

		// Make sure our handle isn't wrapping anything yet,
		assert(handle_.IsEmpty());
		// and the passed in wrapper is expecting an object pointer.
		assert(handle->InternalFieldCount() > 0);
		// Allocate and embed our object pointer.
		handle_ = v8::Persistent<v8::Object>::New(handle);
		handle_->SetAlignedPointerInInternalField(0, this);
		// Mark it as 'weak' so GC will release it ASAP.
		handle_.MakeWeak(this, WeakCallback);
		handle_.MarkIndependent();
	}

	/** Keep our object around until GC releases it. */
	v8::Persistent<v8::Object> handle_;

private:

	static void WeakCallback(v8::Persistent<v8::Value> value, void *data);
};

}

#endif // AVOCADO_OBJECTWRAP_H
