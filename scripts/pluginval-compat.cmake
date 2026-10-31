# pluginval 1.0.4 uses std::chrono duration literals in BasicTests.cpp but relies
# on <thread> including <chrono> transitively. Current MSVC correctly requires
# the explicit header. Supply it without patching upstream or changing tests.
if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/FIchrono>")
endif()
