#! /bin/bash
#
# ofxSoundObject libraries install
#
# uses CMake

# define the version


FORMULA_TYPES=( "osx" "android" "linux64")

# download the source code and unpack it into LIB_NAME
function download() {
    pwd
    git clone git://github.com/erikd/libsndfile.git
}

# prepare the build environment, executed inside the lib src dir
function prepare() {
    if [ "$TYPE" == "android" ] ; then
        cp -rf $FORMULA_DIR/jni/  jni
    fi
}

# executed inside the lib src dir
function build() {
    if [ "$TYPE" == "osx" ] || [ "$TYPE" == "linux64" ] ; then
        
        if [ -d CMakeBuild ]; then
           rm -rf CMakeBuild
        fi
        mkdir -p CMakeBuild
        mkdir -p CMakeBuild/install
        cd CMakeBuild
        
        cmake -D CMAKE_OSX_DEPLOYMENT_TARGET=10.9 \
        -D BUILD_STATIC_LIBS=ON \
        -D BUILD_SHARED_LIBS=OFF \
        -D BUILD_PROGRAMS=OFF \
        -D BUILD_EXAMPLES=OFF \
        -D BUILD_TESTING=OFF \
        -D ENABLE_CPACK=OFF \
        -D DISABLE_EXTERNAL_LIBS=ON \
        -D ENABLE_EXPERIMENTAL=OFF \
        -D DISABLE_CPU_CLIP=OFF \
        -D ENABLE_BOW_DOCS=OFF \
        -D CMAKE_INSTALL_PREFIX=$LIBS_DIR/libsndfile/install ..
        cmake --build .
        make & make install

        # else
        #     cmake -D CMAKE_INSTALL_PREFIX=$LIBS_DIR/install ..
        # fi

        # cmake --build . --config Release
        
        cd ../../
    elif [ "$TYPE" == "android" ] ; then
        ${NDK_ROOT}/ndk-build -j4 NDK_DEBUG=0 NDK_PROJECT_PATH=.
    fi
}

# executed inside the lib src dir, first arg $1 is the dest libs dir root
function copy() {
    # headers
    if [ -d $1/include ]; then
        rm -rf $1/include
    fi

    mkdir -p $1/include
    mkdir -p $1/lib/$TYPE

    if [ "$TYPE" == "osx" ] || [ "$TYPE" == "linux64" ] ; then
        mv $1/install/lib/libsndfile.a $1/lib/$TYPE/
        mv $1/install/include $1/include/
        rm -rf $1/install
    elif [ "$TYPE" == "android" ] ; then
        cp -vr . $1/include
        cp -vr obj/local/armeabi-v7a/libsndfile.a $1/lib/android/armeabi-v7a/libsndfile.a
        cp -vr obj/local/x86/libsndfile.a $1/lib/android/x86/libsndfile.a
    fi
}

# executed inside the lib src dir
function clean() {
    if [ "$TYPE" == "osx" ] || [ "$TYPE" == "linux64" ] ; then
        if [ -d CMakeBuild ]; then
            cd CMakeBuild
            cmake clean .
            cd ..
            rm -rf CMakeBuild
            cd ..
        fi
    elif [ "$TYPE" == "android" ] ; then
        rm -rf obj
    fi

}
