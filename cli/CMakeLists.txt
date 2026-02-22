set(SOURCE
		include/chiaki-cli.h
		src/discover.c
		src/wakeup.c)
add_library(chiaki-cli-lib STATIC ${SOURCE})
target_include_directories(chiaki-cli-lib PUBLIC "include")
target_link_libraries(chiaki-cli-lib chiaki-lib)
if(CHIAKI_CLI_ARGP_STANDALONE)
	find_library(ARGP_LIBRARY NAMES argp libargp argp-standalone)
	if(ARGP_LIBRARY)
		target_link_libraries(chiaki-cli-lib "${ARGP_LIBRARY}")
	else()
		message(FATAL_ERROR "argp library not found! Install mingw-w64-ucrt-x86_64-argp-standalone or similar.")
	endif()
endif()
add_executable(chiaki-cli src/main.c)
target_link_libraries(chiaki-cli chiaki-cli-lib)
install(TARGETS chiaki-cli)
