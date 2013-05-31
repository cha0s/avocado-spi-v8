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
	; make dependencies
	
win32 {
	v8lib.commands += && tools/mingw-generate-makefiles.sh
}

v8lib.commands += ; make -j4 ia32.release \
	&& cp out/ia32.release/obj.target/tools/gyp/libv8_base*.a libv8-avocado.a \
	&& cp out/ia32.release/obj.target/tools/gyp/libv8_snapshot*.a libv8_snapshot-avocado.a; \ 
	cd ../..; \
	echo "Done building v8.";
	
v8lib.target = v

QMAKE_EXTRA_TARGETS += v8lib

PRE_TARGETDEPS += v

win32 {
	debug {
		rm -f debug/libdeps*
	}
	release {
		rm -f release/libdeps*
	}
}
else {
	QMAKE_POST_LINK = rm -f libdeps*
}
