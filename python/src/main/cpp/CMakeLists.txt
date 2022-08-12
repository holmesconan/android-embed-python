# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("2D Print System Engine")

set(PYBIND11_NOPYTHON ON)
set(PYBIND11_FINDPYTHON OFF)
add_subdirectory(pybind11)

if (DEFINED ANDROID_ABI)
    find_package(native-libs REQUIRED)

    get_target_property(Python_INCLUDE_DIRS native-libs::python3.10 INTERFACE_INCLUDE_DIRECTORIES)
    include_directories(${Python_INCLUDE_DIRS}/python3.10)

    # Creates and names a library, sets it as either STATIC
    # or SHARED, and provides the relative paths to its source code.
    # You can define multiple libraries, and CMake builds them for you.
    # Gradle automatically packages shared libraries with your APK.

    add_library( # Sets the name of the library.
                 engine

                 # Sets the library as a shared library.
                 SHARED

                 # Provides a relative path to your source file(s).
                 jni.cpp pythonthread.cpp
                 Modules/monkeypatch.cpp)

    # Searches for a specified prebuilt library and stores the path as a
    # variable. Because CMake includes system libraries in the search path by
    # default, you only need to specify the name of the public NDK library
    # you want to add. CMake verifies that the library exists before
    # completing its build.

    find_library( # Sets the name of the path variable.
                  log-lib

                  # Specifies the name of the NDK library that
                  # you want CMake to locate.
                  log )

    # Specifies libraries CMake should link to your target library. You
    # can link multiple libraries, such as libraries you define in this
    # build script, prebuilt third-party libraries, or system libraries.

    target_link_libraries( # Specifies the target library.
                           engine

                           native-libs::python3.10
                           pybind11::embed

                           # Links the target library to the log library
                           # included in the NDK.
                           ${log-lib} )
else()
    # Do unit test here
endif()
