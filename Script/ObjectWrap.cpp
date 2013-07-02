#include "core/avocado-global.h"

#include "ObjectWrap.h"

namespace avo {

ObjectWrap::~ObjectWrap() {

	// Only kill it once.
	if (!handle_.IsEmpty()) {

		// GC has run?
		assert(handle_.IsNearDeath(
#ifndef AVOCADO_NODE
			V8::avocadoIsolate
#endif
		));

		// NULL out the handle.
		handle_.ClearWeak(
#ifndef AVOCADO_NODE
			V8::avocadoIsolate
#endif
		);
		handle_->SetInternalField(0, v8::Undefined());
		handle_.Dispose(
#ifndef AVOCADO_NODE
			V8::avocadoIsolate
#endif
		);
		handle_.Clear();
	}
}

void ObjectWrap::WeakCallback(
#ifndef AVOCADO_NODE
	v8::Isolate* env,
#endif
	v8::Persistent<v8::Value> value,
	void *data
) {
	v8::HandleScope scope(
#ifndef AVOCADO_NODE
		V8::avocadoIsolate
#endif
	);

	// Get coherence back.
	ObjectWrap *obj = static_cast<ObjectWrap *>(data);

	// This is us, and GC ran?
	assert(value == obj->handle_);
	assert(value.IsNearDeath(
#ifndef AVOCADO_NODE
		env
#endif
	));

	// Kill this.
	delete obj;
}

}
