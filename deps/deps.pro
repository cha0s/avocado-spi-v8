TEMPLATE = lib

v8lib.commands += \
	echo "Building dependencies..."; \
	#
	# Test for binaries.
	#
	`type git >/dev/null 2>&1` || exit 1; \
	#
	# V8
	#
	echo "Building v8..."; \
	#
	# Checkout the V8 git repository if it hasn't been yet.
	#
	cd v8 \
	&& git apply --ignore-space-change --ignore-whitespace ../v8.patch \
	|| make dependencies \
	&& make -j4 native \
	&& cp out/native/obj.target/tools/gyp/libv8_base.a libv8-avocado.a \
	&& cp out/native/obj.target/tools/gyp/libv8_snapshot.a libv8_snapshot-avocado.a; \ 
	cd ../..; \
	echo "Done building v8.";
	
v8lib.target = v

QMAKE_EXTRA_TARGETS += v8lib

PRE_TARGETDEPS += v

QMAKE_POST_LINK = rm libdeps*
