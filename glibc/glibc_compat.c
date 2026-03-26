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
