
add_custom_command(
	
	OUTPUT
		libavocado-v8_base.ia32.a
		libavocado-v8_snapshot.a

	COMMAND make dependencies
	COMMAND werror=no make -j4 ia32.release
	
	COMMAND cp out/ia32.release/obj.target/tools/gyp/libv8_base.ia32.a ${CMAKE_CURRENT_BINARY_DIR}/libavocado-v8_base.ia32.a
	COMMAND cp out/ia32.release/obj.target/tools/gyp/libv8_snapshot.a ${CMAKE_CURRENT_BINARY_DIR}/libavocado-v8_snapshot.a
	
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/v8

)

add_custom_target(ScriptService-v8-v8
	DEPENDS
		libavocado-v8_base.ia32.a
		libavocado-v8_snapshot.a
)
