#include "avocado-global.h"

#include "ObjectWrap.h"

namespace avo {

ObjectWrap::~ObjectWrap() {

	// Only kill it once.
	if (!handle_.IsEmpty()) {

		// GC has run?
		assert(handle_.IsNearDeath(V8::avocadoIsolate));

		// NULL out the handle.
		handle_.ClearWeak(V8::avocadoIsolate);
		handle_->SetInternalField(0, v8::Undefined());
		handle_.Dispose(V8::avocadoIsolate);
		handle_.Clear();
	}
}

void ObjectWrap::WeakCallback(v8::Isolate* env, v8::Persistent<v8::Value> value, void *data) {
	v8::HandleScope scope(V8::avocadoIsolate);

	// Get coherence back.
	ObjectWrap *obj = static_cast<ObjectWrap *>(data);

	// This is us, and GC ran?
	assert(value == obj->handle_);
	assert(value.IsNearDeath(env));

	// Kill this.
	delete obj;
}

}
