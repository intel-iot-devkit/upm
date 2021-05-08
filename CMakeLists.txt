cmake_minimum_required (VERSION 2.8.11)
project (upm)

# Before going any further, define build options
option (BUILDDOC "Build all doc" OFF)
option (BUILDCPP "Build CPP sensor libraries" ON)
option (BUILDFTI "Build Funtion Table Interface (FTI) in C sensor libraries" OFF)
option (BUILDSWIGPYTHON "Build swig python modules" ON)
option (BUILDSWIGNODE "Build swig node modules" ON)
option (BUILDSWIGJAVA "Build swig java modules" OFF)
option (BUILDCORDOVA "Build cordova bindings" OFF)
option (BUILDEXAMPLES "Build C/C++/JAVA examples" OFF)
option (NPM "Generate NPM/GYP tarballs" OFF)
option (BUILDTESTS "Generate check-ups for upm" OFF)
option (WERROR "Make all warnings into errors." ON)

# Warn if building in source root
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message (WARNING "Building into sources dir can be risky, prefer other directory")
endif ()

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# Check if provided compiler supports target flag
# usage:
#       compiler_flag_supported(C/CXX is_supported flag)
#
# The result of output_variable is set to the sanitized flag name if supported
# or cleared if the flag is not supported.
function (compiler_flag_supported compiler output_variable flag)
  # Currently only C and CXX compile flags, clear and return if not supported
  if (NOT ${compiler} MATCHES "C|CXX")
    message (WARNING "Unknown compiler: ${compiler}")
    set ("${output_variable}" "" PARENT_SCOPE)
    return ()
  endif (NOT ${compiler} MATCHES "C|CXX")

  string (REPLACE "-" "_" SANITIZED_FLAG_NAME "${flag}")
  string (REPLACE "/" "_" SANITIZED_FLAG_NAME "${SANITIZED_FLAG_NAME}")
  string (REPLACE "=" "_" SANITIZED_FLAG_NAME "${SANITIZED_FLAG_NAME}")
  string (REPLACE " " "_" SANITIZED_FLAG_NAME "${SANITIZED_FLAG_NAME}")

  # Disable messages from CHECK_C/XX_COMPILER_FLAGS macros
  set (CMAKE_REQUIRED_QUIET_SAVED ${CMAKE_REQUIRED_QUIET})
  set (CMAKE_REQUIRED_QUIET ON)

  # C or CXX?
  if (${compiler} STREQUAL C)
    CHECK_C_COMPILER_FLAG("${flag}" HAS_${SANITIZED_FLAG_NAME})
  elseif (${compiler} STREQUAL CXX)
    CHECK_CXX_COMPILER_FLAG("${flag}" HAS_${SANITIZED_FLAG_NAME})
  endif ()

  # Restore previous CMAKE_REQUIRED_QUIET state
  set (CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVED})

  # Does the compiler support this flag?
  if (HAS_${SANITIZED_FLAG_NAME})
    set ("${output_variable}" "${SANITIZED_FLAG_NAME}" PARENT_SCOPE)
  else ()
    set ("${output_variable}" "" PARENT_SCOPE)
  endif()
endfunction ()

include (CheckCCompilerFlag)
include (CheckCXXCompilerFlag)
# Add scoped compile flag/s using add_compile_options.
# This function checks to see if each flag is supported
# by the compiler before setting the compile option.
function (upm_add_compile_flags compiler)
  set (_TMP_COMPILER_FLAGS "")
  # Iterate the flags, check if supported
  foreach (flag ${ARGN})
    # Check if this compile flag is supported
    compiler_flag_supported(${compiler} is_supported ${flag})

    # Add if supported, warn and skip if not supported
    if (is_supported)
      set (_TMP_COMPILER_FLAGS "${_TMP_COMPILER_FLAGS} ${flag}")
    else ()
      message (WARNING "${compiler} compiler does not support flag \"${flag}\"")
    endif ()
  endforeach (flag ${ARGN})

  # Set the variable in the parent scope
  set (CMAKE_${compiler}_FLAGS "${CMAKE_${compiler}_FLAGS} ${_TMP_COMPILER_FLAGS}" PARENT_SCOPE)
endfunction ()

# Compiler flags common to both C and CXX
# Enable -Wall
set (C_CXX_WARNING_FLAGS -Wall
    -Wno-strict-aliasing
    -Wno-deprecated-declarations # Temp fix for MRAA deprecated methods
    )

# GCC-6 added -Wmisleading-indentation to -Wall, skip these for now
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "6")
  list (APPEND C_CXX_WARNING_FLAGS -Wno-misleading-indentation)
endif ()

# Warnings as errors?
if (WERROR)
  list (APPEND C_CXX_WARNING_FLAGS -Werror)
  message (STATUS "Warnings as errors enabled (-Werror), disable with -DWERROR=off")
endif (WERROR)

# Set C compiler warning flags at top-level scope and emit a warning about
# unsupported flags
upm_add_compile_flags(C ${C_CXX_WARNING_FLAGS}
  -Winit-self
  -Wimplicit
  -Wsign-compare
  -Wmissing-parameter-type)

# Set CXX compiler warning flags at top-level scope and emit a warning about
# unsupported flags
upm_add_compile_flags(CXX ${C_CXX_WARNING_FLAGS}
  -Wnon-virtual-dtor
  -Woverloaded-virtual
  -Wsign-compare
  -Wreorder)

# Allow exception error handling for Android C++
if (ANDROID)
  upm_add_compile_flags(CXX -fexceptions)
endif (ANDROID)

find_package (Threads REQUIRED)
find_package (PkgConfig REQUIRED)

# Force a libmraa search and minimum required version every time a config is generated
unset(MRAA_FOUND CACHE)
set(MRAA_MINIMUM 2.0.0)
pkg_check_modules (MRAA REQUIRED mraa>=${MRAA_MINIMUM})
# Also, get full path to the mraa library
find_library(MRAA_LIBRARY NAMES mraa HINTS ${MRAA_LIBDIR})

# Test MRAA for various compile options
include (CheckLibraryExists)
check_library_exists (${MRAA_LIBRARIES} mraa_iio_init "${MRAA_LIBDIR}" MRAA_IIO_FOUND)
check_library_exists (${MRAA_LIBRARIES} mraa_firmata_init "${MRAA_LIBDIR}" MRAA_FIRMATA_FOUND)
check_library_exists (${MRAA_LIBRARIES} mraa_uart_ow_init "${MRAA_LIBDIR}" MRAA_OW_FOUND)

# Check for BACNET
pkg_check_modules (BACNET libbacnet)

# Check for MODBUS
pkg_check_modules (MODBUS libmodbus>=3.1.2)

# Check for OPENZWAVE
pkg_check_modules (OPENZWAVE libopenzwave)

# Check for TinyB
pkg_check_modules (TINYB tinyb>=0.5.1)
# And get full path to the tinyb library
find_library(TINYB_LIBRARY NAMES tinyb HINTS ${TINYB_LIBDIR})

# Find JPEG
find_package (JPEG)

# Find nodejs
if (BUILDSWIGNODE)
  find_package (Node REQUIRED)
  if (BUILDTESTS)
    find_package (Npm REQUIRED)
  endif (BUILDTESTS)
endif (BUILDSWIGNODE)

# Find JAVA/JNI
if (BUILDSWIGJAVA)
  find_package (Java REQUIRED)
  find_package (JNI REQUIRED)
  pkg_check_modules (MRAAJAVA REQUIRED mraajava>=${MRAA_MINIMUM})
  # Also, get full path to the mraajava library
  find_library(MRAAJAVA_LIBRARY NAMES mraajava HINTS ${MRAA_LIBDIR})
endif (BUILDSWIGJAVA)

# Cordova binding
if (BUILDCORDOVA)
  if (NOT BUILDSWIGJAVA)
    message(FATAL_ERROR "Cordova bindings require JAVA packages, please enable BUILDSWIGJAVA (-DBUILDSWIGJAVA=on).")
  endif()

  find_package (Node REQUIRED)
  find_package (Npm REQUIRED)
  find_package (UpmCordovaGenerator 0.2.1 REQUIRED)
endif (BUILDCORDOVA)

# Find swig if any wrapper is enabled
if (BUILDSWIGPYTHON OR BUILDSWIGNODE OR BUILDSWIGJAVA)
  find_package (SWIG 3.0.5 REQUIRED)
  include (${SWIG_USE_FILE})
endif ()

# Python is required for swig generated python and for UPM tests.
# The UPM build can generated modules for both python2 AND python3
# with the corresponding PYTHONLIBS.  Currently, BUILDTESTS has a
# hard dependency on the PYTHON2INTERP.
# OpenCV python detect will attempt to find python2/3
if (BUILDSWIGPYTHON OR BUILDTESTS)
  include (cmake/modules/OpenCVDetectPython.cmake)

  # Fail if building tests but no python interpreter was found
  if (BUILDTESTS AND NOT PYTHON2INTERP_FOUND)
      message(FATAL_ERROR "BUILDTESTS=ON requires the python2 interpreter")
  endif (BUILDTESTS AND NOT PYTHON2INTERP_FOUND)

  # Fail if no LIBS were found
  if (NOT PYTHON2LIBS_FOUND AND NOT PYTHON3LIBS_FOUND)
      message(FATAL_ERROR "At least one python lib is required")
  endif (NOT PYTHON2LIBS_FOUND AND NOT PYTHON3LIBS_FOUND)
endif (BUILDSWIGPYTHON OR BUILDTESTS)

# Which versions of python were found?
if (PYTHON2LIBS_FOUND AND BUILDSWIGPYTHON)
  message(STATUS "Building python2 modules with python-${PYTHON2LIBS_VERSION_STRING}")
endif (PYTHON2LIBS_FOUND AND BUILDSWIGPYTHON)
if (PYTHON3LIBS_FOUND AND BUILDSWIGPYTHON)
  message(STATUS "Building python3 modules with python-${PYTHON3LIBS_VERSION_STRING}")
endif (PYTHON3LIBS_FOUND AND BUILDSWIGPYTHON)

# Python2 is currently required for python documentation
if (BUILDSWIGPYTHON AND BUILDDOC AND NOT PYTHON2INTERP_FOUND)
  message(FATAL_ERROR "Failed to find python2 interpreter which is required "
      "to build python documentation.")
endif (BUILDSWIGPYTHON AND BUILDDOC AND NOT PYTHON2INTERP_FOUND)

# Set CMAKE_INSTALL_LIBDIR if not defined
include(GNUInstallDirs)
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Installation path for libraries")

# Make a version file containing the current version from git.
include (GetGitRevisionDescription)
git_describe (VERSION "--tags")
# If git_describe fails, use a dirty version
if (${VERSION} MATCHES -NOTFOUND)
  set (VERSION "v1.7.1")
  message (WARNING "Failed to retrieve UPM version with 'git describe' (using "
      "${VERSION}). Check that git is installed and this is a valid git repo.")
endif ()

message (STATUS "UPM Version ${VERSION}")

# Parse the version information into pieces.
string (REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+).*" "\\1" VERSION_COMMIT "${VERSION}")
string (REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+-[0-9]+\\-(.*)" "\\1" VERSION_SHA1 "${VERSION}")

if ("${VERSION_COMMIT}" MATCHES "^v.*")
  set (VERSION_COMMIT "")
endif()

set (upm_VERSION_MAJOR ${VERSION_MAJOR})
set (upm_VERSION_MINOR ${VERSION_MINOR})
set (upm_VERSION_PATCH ${VERSION_PATCH})
set (upm_VERSION_STRING ${upm_VERSION_MAJOR}.${upm_VERSION_MINOR}.${upm_VERSION_PATCH})

# enable c++11 standards support unconditionally
include(CheckCXXCompilerFlag)
if (CMAKE_VERSION VERSION_LESS "3.1")
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
  if (COMPILER_SUPPORTS_CXX11)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  elseif (COMPILER_SUPPORTS_CXX0X)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  else()
    message(FATAL_ERROR "A C++11 compliant compiler is required to build UPM.")
  endif()
else()
  # 3.1+ uses this generic method to enable c++11
  if(NOT "${CMAKE_CXX_STANDARD}")
    set (CMAKE_CXX_STANDARD 11)
  endif ()
  set (CXX_STANDARD_REQUIRED ON)
  set (CXX_EXTENSIONS OFF)
endif()

include(CheckCCompilerFlag)
if (CMAKE_VERSION VERSION_LESS "3.1")
  CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
  if (COMPILER_SUPPORTS_C11)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
  else()
    message(FATAL_ERROR "A C11 compliant C compiler is required to build UPM.")
  endif()
else()
  # 3.1+ uses this generic method to enable c11
  set (CMAKE_C_STANDARD 11)
  set (C_STANDARD_REQUIRED ON)
  set (C_EXTENSIONS OFF)
endif()

# The doc target depends on the C/C++ source and all libraries
#
# doc
#   ├──> src
#   ├──> include
#   ├──> libupm_sensor0
#   ├──> libupm_sensor1
#   ├──> ...
#   └──> libupm_sensor_n
#
# The pydoc target builds documentation with sphinx via inspection by loading
# each python2 module.  Those modules must include the CXX documentation via
# a monolithic swig file generated by doxy2swig
#
# pydoc
#  └──> _pyupm_sensor0_python2
#        ├──> libupm_sensor0
#        └──> doxy2swig
#
# The doxy2swig target is dependent upon the doc target IF BUILDDOC=ON,
#
# doxy2swig
#  └──> BUILDDOC=ON───> doc
#
# The jsdoc target builds js documentation via yuidoc and only requires
# the doc target
#
# jsdoc ─> doc
#
function (CreateDocumentationTargets)
  # Add a target to generate API documentation with Doxygen
  find_package (Doxygen 1.8 REQUIRED)
  if (DOXYGEN_FOUND AND DOXYGEN_VERSION VERSION_GREATER "1.8")
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/doxy/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    if (BUILDSWIGJAVA)
        configure_file (${CMAKE_CURRENT_SOURCE_DIR}/doxy/Doxyfile.java.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-java @ONLY)
    endif()
    file(GLOB PNG_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/docs docs/icons/*.png)
    foreach(PNG_FILE ${PNG_FILES})
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/${PNG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/html/docs/${PNG_FILE} COPYONLY)
    endforeach()

    # Custom command to run doxygen (note depends on ALL UPM C++ targets)
    add_custom_command (
      OUTPUT ${CMAKE_BINARY_DIR}/xml/index.xml
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${UPM_TARGETS_CXX}
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      COMMAND tar -czf html/xml.tar.gz -C xml .
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating C/C++ API documentation with Doxygen"
      VERBATIM)
    add_custom_target(doc DEPENDS ${CMAKE_BINARY_DIR}/xml/index.xml)
  else ()
    message (SEND_ERROR "ERROR - Failed to find a compatible version of Doxygen. API doc will not be generated")
  endif (DOXYGEN_FOUND AND DOXYGEN_VERSION VERSION_GREATER "1.8")

  # Check if Sphinx is installed and add target to generate API documentation
  # Currently, the per-module documentation for python is generated from the
  # python2 modules.
  # Since python2 is required for documentation, only copy from python2 paths, this
  # ensures that sphinx doesn't run across python2 and python3 binaries.  When running
  # the sphinx tools, explicitly run from the python2 interpreter (tested with the sphinx
  # 1.3.6 python2 and python3 modules).
  if(BUILDSWIGPYTHON)
    # Generate python module documentation from doxygen collateral
    #
    # doxygen index.xml -> doxy2swig.py -> pyupm_doxy2swig.i
    add_custom_command (
      OUTPUT ${CMAKE_BINARY_DIR}/src/pyupm_doxy2swig.i
      COMMAND ${PYTHON2_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/doxy2swig.py
              ${CMAKE_BINARY_DIR}/xml/index.xml --quiet
              ${CMAKE_BINARY_DIR}/src/pyupm_doxy2swig.i
      COMMENT "Generating pyupm_doxy2swig.i from Doxygen output for use by SWIG"
      DEPENDS doc
      VERBATIM)
    add_custom_target(pyupm_doxy2swig DEPENDS ${CMAKE_BINARY_DIR}/src/pyupm_doxy2swig.i)
    foreach (_python2_target ${UPM_TARGETS_PYTHON2})
      add_dependencies(${_python2_target} pyupm_doxy2swig)
    endforeach()

    find_package (Sphinx 1.3 REQUIRED)
    if (SPHINX_FOUND AND SPHINX_VERSION VERSION_GREATER "1.3")
      configure_file (${CMAKE_CURRENT_SOURCE_DIR}/doxy/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/pydoc/conf.py @ONLY)
      configure_file (${CMAKE_CURRENT_SOURCE_DIR}/doxy/index.rst ${CMAKE_CURRENT_BINARY_DIR}/pydoc/index.rst COPYONLY)
      add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pydoc.stamp
        COMMAND rm -r -f ${CMAKE_BINARY_DIR}/pyupm && mkdir -p ${CMAKE_BINARY_DIR}/pyupm
        COMMAND find ${CMAKE_BINARY_DIR}/src -path "*python${MIN_VER_PYTHON2}/_pyupm_*.so" -exec cp {} ${CMAKE_BINARY_DIR}/pyupm \;
        COMMAND find ${CMAKE_BINARY_DIR}/src -path "*python${MIN_VER_PYTHON2}/pyupm_*.py" -exec cp {} ${CMAKE_BINARY_DIR}/pyupm \;
        COMMAND ${PYTHON2_EXECUTABLE} ${SPHINX_API_EXECUTABLE} -f -o pydoc  ${CMAKE_BINARY_DIR}/pyupm
        # TODO: use a separate cmake FILE module for string replacement instead
        COMMAND ${PYTHON2_EXECUTABLE} ${SPHINX_EXECUTABLE} -b html pydoc html/python
        COMMAND sed -i.bak s|\">pyupm_|\">|g html/python/index.html html/python/modules.html
        COMMAND sed -i.bak s|[[:space:]][mM]odule</a>|</a>|g html/python/index.html html/python/modules.html
        COMMAND cmake -E touch ${CMAKE_CURRENT_BINARY_DIR}/pydoc.stamp
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Python API documentation with Sphinx"
        DEPENDS doc ${UPM_TARGETS_PYTHON2}
        VERBATIM)
      add_custom_target(pydoc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pydoc.stamp)
    else ()
      message (SEND_ERROR "ERROR - Failed to find a compatible version of Sphinx. Python API doc will not be generated")
    endif ()
  endif(BUILDSWIGPYTHON)

  # Check if Yuidoc is installed and add target for API documentation
  if(BUILDSWIGNODE)
    find_package (Yuidoc 0.10 REQUIRED)
    if (YUIDOC_FOUND AND YUIDOC_VERSION VERSION_GREATER "0.10")
      add_custom_command (
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jsdoc/jsdoc.stamp
        COMMAND ${CMAKE_SOURCE_DIR}/doxy/doxygen2jsdoc/docgen.js -m upm -i xml -o jsdoc -t ${CMAKE_CURRENT_SOURCE_DIR}/src -g ../../
        COMMAND ${YUIDOC_EXECUTABLE} -C --no-sort --helpers ${CMAKE_SOURCE_DIR}/doxy/node/generators/yuidoc/helper.js --themedir ${CMAKE_SOURCE_DIR}/doxy/node/generators/yuidoc/tmpl -o html/node jsdoc/yuidoc/upm
        COMMAND ${CMAKE_SOURCE_DIR}/doxy/doxygen2jsdoc/tolower.js -i html/node
        COMMAND cmake -E touch ${CMAKE_CURRENT_BINARY_DIR}/jsdoc/jsdoc.stamp
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Javascript API documentation with Yuidoc"
        DEPENDS doc
        VERBATIM)
      add_custom_target(jsdoc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/jsdoc/jsdoc.stamp)
    else ()
      message (SEND_ERROR "ERROR - Failed to find a compatible version of Yuidoc. Node.js API doc will not be generated")
    endif ()
  endif(BUILDSWIGNODE)
endfunction()

# UPM common headers
set (UPM_COMMON_HEADER_DIRS ${CMAKE_HOME_DIRECTORY}/include)

# Generate a build-only C++ header to add functionality to SWIG'ed modules
configure_file (${PROJECT_SOURCE_DIR}/cmake/modules/version.hpp.in ${PROJECT_BINARY_DIR}/src/version.hpp @ONLY)

# UPM source
add_subdirectory (src)

# UPM examples
add_subdirectory (examples)

# Python interp is previously found if BUILDTESTS=ON
if (BUILDTESTS)
  enable_testing ()
  add_subdirectory (tests)
endif()

# Setup documentation AFTER all source targets have been added
if (BUILDDOC)
  CreateDocumentationTargets()
endif()

# Install C headers
install(DIRECTORY include/ DESTINATION include/upm
    COMPONENT ${CMAKE_PROJECT_NAME}-dev
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
