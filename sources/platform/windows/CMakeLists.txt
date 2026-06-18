# ===========================================================================
# Windows-only application: the MFC GUI (gui/) and the ATL/COM core (ttcoreex/).
#
# This subtree is pervasively coupled to MFC (CString, CWnd, ...), ATL/COM
# (CComModule, IDispatch), and the Active Scripting host headers under
# include/. It requires MSVC with the MFC and ATL components installed, and the
# parent only descends here when WIN32 is true.
#
# IMPORTANT — UNVERIFIED ON THIS HOST:
# The development host for this CMake work was macOS, so these two targets have
# NOT been compiled. They are structured to mirror the original Visual C++
# project (smc.dsp / ttcoreex.dsp) and are a starting point to finish and
# verify on a Windows + Visual Studio machine. The portable layer
# (jmc::pcre + regex_demo) is the part that has been built and run here.
#
# Likely points to confirm on Windows:
#   - MFC/ATL components installed; the StdAfx.h precompiled headers compile.
#   - The COM server's MIDL outputs: ttcoreex_i.c / ttcoreex_p.c / dlldata.c
#     are checked in, so no midl step is wired here. Regenerate from
#     ttcoreex.idl if they drift.
#   - Exact link libraries and DLL exports (ttcoreex.def), and COM registration.
# ===========================================================================

if(NOT MSVC)
  message(WARNING
    "platform/windows requires MSVC (MFC/ATL). Skipping the jmc app targets.")
  return()
endif()

set(CMAKE_MFC_FLAG 2)   # use MFC in a shared DLL

# --- Core engine: ttcoreex (ATL/COM in-process server) ---------------------
# NOTE: file(GLOB) is normally discouraged, but this is a large, frozen MFC/ATL
# source set that is edited on Windows; CONFIGURE_DEPENDS makes the build
# re-glob when files change. Swap to an explicit list if you prefer.
file(GLOB TTCOREEX_SOURCES CONFIGURE_DEPENDS "ttcoreex/*.cpp")

add_library(ttcoreex SHARED
  ${TTCOREEX_SOURCES}
  ttcoreex/ttcoreex_i.c      # MIDL-generated IID/CLSID definitions (checked in)
  ttcoreex/dlldata.c         # COM proxy/stub data
  ttcoreex/ttcoreex.rc)

target_include_directories(ttcoreex PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"            # so #include "ttcoreex/x.h" resolves
  "${CMAKE_CURRENT_SOURCE_DIR}/ttcoreex"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"    # Active Scripting headers
  "${CMAKE_SOURCE_DIR}/sources/wolfssl")   # #include <wolfssl/...>

target_compile_definitions(ttcoreex PRIVATE
  _USRDLL _WINDLL _ATL_APARTMENT_THREADED _CRT_SECURE_NO_WARNINGS)

# Portable regex engine + Windows system libraries (sockets, multimedia timers).
target_link_libraries(ttcoreex PRIVATE jmc::pcre ws2_32 winmm)

# Compression + crypto. Prefer the vendored prebuilt import libs that shipped
# with the original Windows build; fall back to anything found by the parent.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ttcoreex/zlib.lib")
  target_link_libraries(ttcoreex PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ttcoreex/zlib.lib")
elseif(ZLIB_FOUND)
  target_link_libraries(ttcoreex PRIVATE ZLIB::ZLIB)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ttcoreex/wolfssl.lib")
  target_link_libraries(ttcoreex PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ttcoreex/wolfssl.lib")
elseif(JMC_HAVE_WOLFSSL)
  target_link_libraries(ttcoreex PRIVATE "${JMC_WOLFSSL_TARGET}")
endif()

# --- GUI application: jmc (MFC) --------------------------------------------
file(GLOB JMC_GUI_SOURCES CONFIGURE_DEPENDS "gui/*.cpp")

add_executable(jmc WIN32
  ${JMC_GUI_SOURCES}
  gui/smc.rc)

target_include_directories(jmc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/gui"
  "${CMAKE_CURRENT_SOURCE_DIR}"            # so #include "ttcoreex/x.h" resolves
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_definitions(jmc PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(jmc PRIVATE jmc::pcre)

# The GUI loads ttcoreex as a COM server at runtime rather than linking it, so
# we only force build order, not a link dependency.
add_dependencies(jmc ttcoreex)
