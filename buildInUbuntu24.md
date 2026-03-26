cocos2d-x-ubuntu24开发环境搭建
1.git checkout
git@github.com:KIMI-Z/cocos2d-x.git
2.会失败，需要手动checkout子模块
git submodule update --init


[submodule "tools/cocos2d-console"]
        path = tools/cocos2d-console
        url = https://github.com/cocos2d/cocos2d-console.git
[submodule "tools/bindings-generator"]
        path = tools/bindings-generator
        url = https://github.com/cocos2d/bindings-generator.git
[submodule "tests/cpp-tests/Resources/ccs-res"]
        path = tests/cpp-tests/Resources/ccs-res
        url = https://github.com/dumganhar/ccs-res.git

3.需要安装python2.7支持该命令
$ cd cocos2d-x
$ ./setup.py
$ source FILE_TO_SAVE_SYSTEM_VARIABLE

过程需要下载python2.7 tar 包，编译安装

设置环境变量，需要配置cmake 路径
$ cd cocos2d-x
$ ./install-linux-deps.sh


不使用原本的编译命令，使用cmake 命令编译
mkdir build 
cd build

cmake ..     -DCMAKE_CXX_STANDARD=17     -DCMAKE_CXX_FLAGS="-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION"     -DCMAKE_EXE_LINKER_FLAGS="-L/media/kimi/Data/Document/Cmake/cocos2d-x/external -lglibc_compat -lm -Wl,--undefined-version"     -DUSE_FMOD=OFF

make -j$(nproc)



__powf_finite和__expf_finite未定解决方案
cd /media/kimi/Data/Document/Cmake/cocos2d-x

# 创建兼容性C文件
cat > glibc_compat.c << 'EOF'
#include <math.h>

// 提供旧版glibc的数学函数符号
float __powf_finite(float x, float y) {
    return powf(x, y);
}

double __pow_finite(double x, double y) {
    return pow(x, y);
}

float __expf_finite(float x) {
    return expf(x);
}

double __exp_finite(double x) {
    return exp(x);
}

float __logf_finite(float x) {
    return logf(x);
}

double __log_finite(double x) {
    return log(x);
}

float __sinf_finite(float x) {
    return sinf(x);
}

double __sin_finite(double x) {
    return sin(x);
}

float __cosf_finite(float x) {
    return cosf(x);
}

double __cos_finite(double x) {
    return cos(x);
}
EOF

# 编译成静态库
gcc -c -O2 -fPIC glibc_compat.c -o glibc_compat.o
ar rcs libglibc_compat.a glibc_compat.o

# 复制到external目录
cp libglibc_compat.a external/

coco make 文件添加以下配置
/media/kimi/Data/Document/Cmake/cocos2d-x/cocos/CMakeLists.txt
# use external libs
add_subdirectory(${COCOS2DX_ROOT_PATH}/external ${ENGINE_BINARY_PATH}/external)
target_link_libraries(cocos2d external m)

#add for rebuild file libglibc_compat.a
target_link_directories(cocos2d PRIVATE ${COCOS2DX_ROOT_PATH}/external)
target_link_libraries(cocos2d glibc_compat)

