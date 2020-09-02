Building UPM                         {#building}
============

UPM uses cmake in order to make compilation relatively painless. Cmake runs
build out of tree so the recommended way is to clone from git and make a build/
directory.

**Dependencies**
 * basic: libmraa, cmake, swig, pkg-config, pthreads, librt
 * bindings: python-dev, nodejs-dev, openjdk
 * documentation: doxygen, graphviz, sphinx, yuidoc
 * sensor specific: bacnet-mstp, modbus, openzwave, jpeg

This project depends on libmraa, so that needs to be installed first. Append
the install location of mraa pkgconfig to the following environment variable:

~~~~~~~~~~~~~
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:.../mraa/build/lib/pkgconfig
~~~~~~~~~~~~~

If you are building the Java or Node.js bindings make sure you set the
`JAVA_HOME` and `NODE_PATH` environment variables respectively.

UPM will attempt to build all directories inside src/ and they must contain
individual CMakeLists.txt files.

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake ..
make
make install
~~~~~~~~~~~~~

The last command will create the include/ and lib/ directories with a copy of
the headers and library objects respectively in your build location. Note that
doing an out-of-source build may cause issues when rebuilding later on. In many
cases you'll need elevated permissions to install:

~~~~~~~~~~~~~{.sh}
sudo make install
~~~~~~~~~~~~~

Our cmake configure has a number of options, *cmake-gui* or *ccmake* can show
you all the options. The interesting ones are detailed below:

Changing install path from /usr/local to /usr
~~~~~~~~~~~~~
-DCMAKE_INSTALL_PREFIX:PATH=/usr
~~~~~~~~~~~~~
Building debug build:
~~~~~~~~~~~~~
-DCMAKE_BUILD_TYPE=DEBUG
~~~~~~~~~~~~~
Using clang instead of gcc:
~~~~~~~~~~~~~
-DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
~~~~~~~~~~~~~
Cross-compiling on a different system:
~~~~~~~~~~~~~
-DCMAKE_CXX_FLAGS:STRING=-m32 -march=i586
-DCMAKE_C_FLAGS:STRING=-m32 -march=i586
~~~~~~~~~~~~~
Enabling Java module building
~~~~~~~~~~~~~
-DBUILDSWIGJAVA=ON
~~~~~~~~~~~~~
Building with an older version of swig (swig 2.0+) requires the disabling of javascript:
~~~~~~~~~~~~~
-DBUILDSWIGNODE=OFF
~~~~~~~~~~~~~
Disabling python module building
~~~~~~~~~~~~~
-DBUILDSWIGPYTHON=OFF
~~~~~~~~~~~~~
Setting the python library to use:
~~~~~~~~~~~~~
-DPYTHON_LIBRARY:FILEPATH=/usr/lib/libpython2.7.so.1.0
~~~~~~~~~~~~~
Building documentation
~~~~~~~~~~~~~
-DBUILDDOC=ON
~~~~~~~~~~~~~
Build C/C++/JAVA examples
~~~~~~~~~~~~~
-DBUILDEXAMPLES=ON
~~~~~~~~~~~~~

If you intend to turn on all the options and build everything at once
(C++, Java, Node, Python and Documentation) you will have to edit the
src/doxy2swig.py file and change the line endings from Windows style to Linux
format. This has to be repeated every time to sync with the master branch since
our Github repository stores files using CR LF line breaks.

You can also generate the include and lib directories containing all the sensor
headers and library files respectively with *make install*. Further, you may
choose to generate these only for a specific sensor you modified, and this can
be achieved by building from the individual makefile of the sensor. Assuming
you're in the build directory, to build/install the lcd module you would:

~~~~~~~~~~~~~
cd src/lcd
make install
~~~~~~~~~~~~~

Note: 'make install' under src/lcd will build all targets (and dependencies)
for the lcd but will NOT install dependencies.

Often developers are only interested in building one module or even just the
python/node module to do some quick testing using scripting. In order to do
this you need to use the target name for the python or node module you want to
rebuild. For example, the lcd module target will have a python2 target prefixed
by _pyupm_ (_pyupm_lcd-python2). Modules not using the UPM cmake macros may
have different naming.  To build the python2 lcd module (and all dependencies),
use the following make target:

~~~~~~~~~~~~~
make _pyupm_lcd-python2
~~~~~~~~~~~~~

Sometimes you want to build a small C++ example against an installed library.
This is fairly easy if installed system-wide. Just link against the correct
library (in this case libupm-lcd) and then add /usr/include/upm to the
loader path:

~~~~~~~~~~~~
g++ test.cxx -lupm-lcd -I/usr/include/upm
~~~~~~~~~~~~

You can also use pkg-config to return the information to you, which is
considered the correct way if including UPM in a build system like cmake or
autotools on linux.

~~~~~~~~~~~
pkg-config --cflags --libs upm-lcd
~~~~~~~~~~~

## Building for Android Things

Requirements:
* [io.mraa.at](https://search.maven.org/#artifactdetails%7Cio.mraa.at%7Cmraa%7C1.8.0%7Caar)
* [io.mraa.at.upm](https://search.maven.org/#artifactdetails%7Cio.mraa.at.upm%7Cupm_zfm20%7C1.3.0%7Caar)
* [Android NDK](https://developer.android.com/ndk/downloads/index.html) >= 14b

### Android NDK r14b

~~~~~~~~~~~~~{.sh}
NDK_HOME="/path/to/android-ndk-r14b"
MRAA_INSTALL_DIR="/path/to/mraa/install"

cmake -DBUILDSWIG=ON \
      -DBUILDSWIGPYTHON=OFF \
      -DBUILDSWIGNODE=OFF \
      -DBUILDSWIGJAVA=ON \
      -DANDROID_COMPILER_FLAGS_CXX='-std=c++11' \
      -DANDROID_PIE=1 \
      -DANDROID_PLATFORM=android-24 \
      -DANDROID_STL_FORCE_FEATURES=ON \
      -DANDROID_STL=c++_shared \
      -DANDROID_TOOLCHAIN_NAME=x86-i686 \
      -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake \
      -DCMAKE_FIND_ROOT_PATH=$MRAA_INSTALL_DIR \
      ..
~~~~~~~~~~~~~

## Building with Docker

You can use `docker` and `docker-compose` to generate a complete build environment
for upm without having to install any other tool.

Requirements:
* [docker](https://www.docker.com/get-docker) >= 1.12.6
* [docker-compose](https://docs.docker.com/compose/install/) >= 1.9.0

**NOTE:** docker-compose is an optional requirement. It actually make running complex
docker build and run command easier. But you can just use docker to build and run.

### Using Docker Images to build Upm

**tl;dr:** Just use this commands to build upm:

```sh
# Build upm documentation
$ docker-compose run doc
# Build upm python2 and python3 packages and run python tests
$ docker-compose run python
# Build upm java package and run java tests
$ docker-compose run java
# Build upm node4 package and run node tests
$ docker-compose run node4
# Build upm node5 package and run node tests
$ docker-compose run node5
# Build upm node6 package and run node tests
$ docker-compose run node6
# Build upm for android things package
$ docker-compose run android
```

**docker-compose** will take a look at the `docker-compose.yaml` file in the repository
root directory, and run an specific command to build upm for the requested target.
Once the build is completed, you will have a `build/` folder in the repository root with all
the compiled code. This `build/` folder is created by using a docker volume. The `build\`
folder contents is reused each time you execute `docker-compose run [TARGET]`.
To know more about volumes in Docker, visit the [Docker Volume Documentation](https://docs.docker.com/engine/tutorials/dockervolumes/).

You can also start an interactive session inside the docker container if you need to run some
custom build commands:

```sh
# Start an interactive bash  shell inside the container
$ docker-compose run python bash
# From now, all the commands are executed inside the container
$ cd build && cmake -DBUILDSWIGPYTHON=ON .. && make clean all
```

If you don't want to use docker-compose, you can also use `docker run` to build upm.
For example, to build upm for python, you can do:

```sh
# From the repository root folder
$ docker run \
      --volume=$(pwd):/usr/src/app \
      --env BUILDSWIGPYTHON=ON \
      --env BUILDSWIGJAVA=OFF \
      --env BUILDSWIGNODE=OFF \
      inteliotdevkit/upm-python \
      bash -c "./scripts/run-cmake.sh && make -Cbuild"
```

### Proxy considerations

If, for some reason, you are behind a proxy, find below a list of common problems related
to proxy settings:

**docker cannot pull images from docker.io**

 Visit [this link](https://docs.docker.com/engine/admin/systemd/#httphttps-proxy)
 to configure docker daemon behind a proxy.

**docker run fails to access the internet**

docker-compose will automatically take `http_proxy`, `https_proxy`, and `no_proxy`
environment variables and use it as build arguments. Be sure to properly configure
this variables before building.

docker, unlinke docker-compose, do not take the proxy settings from the environment
automatically. You need to send them as environment arguments:

```sh
# From the repository root folder
$ docker run \
    --volume=$(pwd):/usr/src/app \
    --env BUILDSWIG=ON \
    --env BUILDSWIGPYTHON=ON \
    --env BUILDSWIGJAVA=OFF \
    --env BUILDSWIGNODE=OFF \
    --env http_proxy=$http_proxy \
    --env https_proxy=$https_proxy \
    --env no_proxy=$no_proxy \
    inteliotdevkit/upm-python \
    bash -c "./scripts/run-cmake.sh && make -Cbuild"
```
