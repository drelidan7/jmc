# sources/ — orchestrates the JMC build.
#
# The project is a Windows-native MUD client (MFC GUI + ATL/COM core). Its
# portable layer, however, builds on any platform. This file builds the
# portable pieces everywhere and only descends into the Windows application
# subtree on Windows.

# --- Portable regex engine (builds on every platform) ----------------------
add_subdirectory(recore)

# --- Optional portable system dependencies ---------------------------------
# These are consumed from the system (the vendored wolfssl/ source ships no
# build system, and zlib is present only as a prebuilt Windows .lib). On
# macOS/Linux install them with your package manager; see BUILDING.md.

# zlib (compression — used by the MCCP path in the Windows core).
find_package(ZLIB)
if(ZLIB_FOUND)
  message(STATUS "jmc: zlib found (${ZLIB_VERSION_STRING})  -> ZLIB::ZLIB")
else()
  message(STATUS "jmc: zlib NOT found  (macOS: 'brew install zlib')")
endif()

# wolfSSL (TLS/crypto — used by the secure-connection path). Probe in order:
# CMake package config, then pkg-config, then a bare library search.
set(JMC_HAVE_WOLFSSL FALSE)
set(JMC_WOLFSSL_TARGET "")
find_package(wolfssl CONFIG QUIET)
if(wolfssl_FOUND)
  set(JMC_HAVE_WOLFSSL TRUE)
  set(JMC_WOLFSSL_TARGET wolfssl::wolfssl)
else()
  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(WOLFSSL IMPORTED_TARGET QUIET wolfssl)
    if(WOLFSSL_FOUND)
      set(JMC_HAVE_WOLFSSL TRUE)
      set(JMC_WOLFSSL_TARGET PkgConfig::WOLFSSL)
    endif()
  endif()
endif()
if(JMC_HAVE_WOLFSSL)
  message(STATUS "jmc: wolfSSL found       -> ${JMC_WOLFSSL_TARGET}")
else()
  message(STATUS "jmc: wolfSSL NOT found  (macOS: 'brew install wolfssl')")
endif()

# --- Portable smoke-test executable ----------------------------------------
# (add_subdirectory child scopes inherit JMC_HAVE_WOLFSSL / ZLIB_FOUND above.)
add_subdirectory(tools/regex_demo)

# --- Windows-only application (MFC GUI + ATL/COM core) ---------------------
if(WIN32)
  add_subdirectory(platform/windows)
else()
  message(STATUS
    "jmc: non-Windows host -> building the portable layer only "
    "(jmc::pcre + regex_demo).")
  message(STATUS
    "jmc: the MFC GUI and ATL/COM core under platform/windows/ are skipped "
    "(they require MSVC + MFC + ATL).")
endif()
