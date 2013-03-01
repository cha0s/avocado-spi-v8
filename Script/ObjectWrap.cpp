#include "main/avocado-global.h"

#include "ObjectWrap.h"

namespace avo {

ObjectWrap::~ObjectWrap() {

	// Only kill it once.
	if (!handle_.IsEmpty()) {

		// GC has run?
		assert(handle_.IsNearDeath());

		// NULL out the handle.
		handle_.ClearWeak();
		handle_->SetInternalField(0, v8::Undefined());
		handle_.Dispose();
		handle_.Clear();
	}
}

void ObjectWrap::WeakCallback(v8::Persistent<v8::Value> value, void *data) {

	// Get coherence back.
	ObjectWrap *obj = static_cast<ObjectWrap *>(data);

	// This is us, and GC ran?
	assert(value == obj->handle_);
	assert(value.IsNearDeath());

	// Kill this.
	delete obj;
}

}
