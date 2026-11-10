// Precompiled header (set up in CMakeLists.txt)
//   Compiled once per build directory, then automatically included first in every .cpp file,
//     no '#include "pch.h"' needed.
//   Put only rarely changing, often used headers here: standard library, third-party libraries.
//     Changing this file (or anything it includes) recompiles every source file.
//   Don't put the project's own headers here.
//   Still include what a file uses in the file itself, so it also compiles without the PCH.
#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// platform exclusive headers
#ifdef _WIN32
//    #include <windows.h>
#endif
