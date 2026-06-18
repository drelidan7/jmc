# regex_demo — portable smoke-test executable.
# Always builds; links the portable PCRE engine and any optional deps that the
# parent scope located (ZLIB_FOUND / JMC_HAVE_WOLFSSL are set in
# sources/CMakeLists.txt and visible here).

add_executable(regex_demo main.c)
target_link_libraries(regex_demo PRIVATE jmc::pcre)

if(ZLIB_FOUND)
  target_link_libraries(regex_demo PRIVATE ZLIB::ZLIB)
  target_compile_definitions(regex_demo PRIVATE HAVE_ZLIB)
endif()

if(JMC_HAVE_WOLFSSL)
  target_link_libraries(regex_demo PRIVATE ${JMC_WOLFSSL_TARGET})
  target_compile_definitions(regex_demo PRIVATE HAVE_WOLFSSL)
endif()
