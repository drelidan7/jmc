# Building JMC

JMC is a Windows-native MUD client. Its GUI is built on **MFC**, and its core
engine (`ttcoreex`) is an **ATL/COM** in-process server that also uses the
Windows **Active Scripting** host. Those parts only build on Windows with Visual
Studio.

To make the project workable on a non-Windows machine, the build is now
**platform-aware**: the portable pieces build everywhere, and the Windows-only
application is compiled only on Windows.

## Layout

```
CMakeLists.txt              top-level project
CMakePresets.json           build presets (debug / release / windows)
sources/
  CMakeLists.txt            orchestrator: portable libs always, Windows app on Windows
  recore/                   PCRE 8.38 — portable regex engine  -> target jmc::pcre
  wolfssl/                  vendored wolfSSL source (consumed as a system package)
  tools/regex_demo/         small portable program that proves the local build works
  platform/windows/         ====== Windows-only (MSVC + MFC/ATL) ======
    gui/                     the MFC GUI (former loose sources/*.cpp + smc.rc + res/)
    ttcoreex/                the ATL/COM core engine
    include/                 Active Scripting / COM host headers
```

Everything under `sources/platform/windows/` is Windows-only. Everything else
builds on any platform.

## What builds where

| Component | macOS / Linux | Windows |
| --- | :---: | :---: |
| `jmc::pcre` (regex engine) | ✅ | ✅ |
| `regex_demo` (smoke test) | ✅ | ✅ |
| zlib / wolfSSL (system deps) | ✅ (when installed) | ✅ |
| `ttcoreex` (ATL/COM core) | — skipped | ✅ |
| `jmc` (MFC GUI app) | — skipped | ✅ |

The full application is Windows-only because its GUI and core are pervasively
coupled to MFC, ATL/COM, and Active Scripting. Porting those to macOS/Linux
would mean rewriting the string/COM layer — a large effort, out of scope here.

## Building on macOS / Linux

Prerequisites:
- A C/C++ compiler (Apple Clang via Xcode Command Line Tools, or GCC/Clang).
- CMake ≥ 3.21.
- Optional but recommended: **zlib** (usually already present) and **wolfSSL**.

```sh
# configure + build the portable layer
cmake --preset debug
cmake --build --preset debug

# run the smoke test
./build/debug/sources/tools/regex_demo/regex_demo
```

Expected output:

```
JMC portable layer smoke test
  PCRE version : 8.38 2015-11-23
  zlib version : 1.2.12
  wolfSSL      : <version, if installed>
  regex match  : 'alice@example.com'
OK
```

`release` is the same with optimizations: `cmake --preset release && cmake --build --preset release`.

### Optional dependencies

The CMake configure step prints whether each was found and links it
automatically when present.

- **zlib** — macOS ships it in the SDK; on Linux install `zlib1g-dev`
  (Debian/Ubuntu) or `zlib-devel` (Fedora). Homebrew: `brew install zlib`.
- **wolfSSL** — `brew install wolfssl`, or `apt install libwolfssl-dev`, or
  build from source. Found via CMake package config or `pkg-config`. The
  `wolfssl/` directory in this repo is a source snapshot **without** a build
  system, so a system install is used rather than building it in-tree.

If a dependency is missing, the build still succeeds — that dependency is just
reported as "not found" and skipped.

> Note: if `cmake` isn't installed and you have no package manager, a quick way
> to get it is `python3 -m pip install --user cmake` (adds `cmake` under
> `~/Library/Python/<ver>/bin`, which you may need to add to `PATH`).

## Building the full app on Windows

This requires Visual Studio with the **MFC** and **ATL** components installed.

```sh
cmake --preset windows
cmake --build --preset windows --config Release
```

> **Status:** the Windows target graph in `sources/platform/windows/CMakeLists.txt`
> is structured to mirror the original Visual C++ project, but it has **not been
> compiled** as part of this work (the development host was macOS). Treat it as a
> verified-portable base plus a Windows scaffold to finish on a Windows machine.
> Points to confirm there: MFC/ATL precompiled headers (`StdAfx.h`), the COM
> server's MIDL outputs (`ttcoreex_i.c` / `ttcoreex_p.c` / `dlldata.c` are
> checked in), the exact link libraries / DLL exports (`ttcoreex.def`), and COM
> registration. See the comments in that file.
