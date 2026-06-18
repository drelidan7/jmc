# recore/ — PCRE 8.38, the project's regular-expression engine.
#
# This is a standard, fully portable Perl-Compatible Regular Expression library
# vendored into the tree. It builds on every platform, which is what lets the
# rest of the portable layer (and the local demo) compile on macOS/Linux.
#
# We compile only the 8-bit library translation units (the pcre_*.c set).
# Deliberately excluded:
#   - main.c          : a Windows DllMain wrapper (this builds a lib, not a DLL)
#   - maketables.c /
#     chartables.c    : standalone table generators, not part of the library
#   - pcre16_*.c /
#     pcre32_*.c      : the 16- and 32-bit variants; the app uses 8-bit PCRE

add_library(pcre STATIC
  pcre_byte_order.c
  pcre_chartables.c
  pcre_compile.c
  pcre_config.c
  pcre_dfa_exec.c
  pcre_exec.c
  pcre_fullinfo.c
  pcre_get.c
  pcre_globals.c
  pcre_jit_compile.c     # compiles to ~nothing unless SUPPORT_JIT is set
  pcre_maketables.c
  pcre_newline.c
  pcre_ord2utf8.c
  pcre_refcount.c
  pcre_string_utils.c
  pcre_study.c
  pcre_tables.c
  pcre_ucd.c
  pcre_valid_utf8.c
  pcre_version.c
  pcre_xclass.c)

# Namespaced alias so consumers link jmc::pcre identically however they reach it.
add_library(jmc::pcre ALIAS pcre)

# HAVE_CONFIG_H makes the sources pull feature flags (SUPPORT_UTF/UCP, LINK_SIZE,
# NEWLINE, limits) from the vendored recore/config.h.
# PCRE_STATIC is PUBLIC: without it, pcre.h decorates the API with
# __declspec(dllimport) on Windows, which is wrong when linking a static lib —
# so every consumer must see it too.
target_compile_definitions(pcre
  PRIVATE HAVE_CONFIG_H
  PUBLIC  PCRE_STATIC)

if(MSVC)
  target_compile_definitions(pcre PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# recore/ holds the public header (pcre.h) and config.h, so expose it to
# consumers for #include <pcre.h>.
target_include_directories(pcre PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
