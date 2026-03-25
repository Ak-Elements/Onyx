#pragma once

namespace onyx {
enum class VectorComponentMask : uint8_t {
    None = 0,
    X = 1 << 0,
    Y = 1 << 2,
    Z = 1 << 3,
    W = 1 << 4,
    XY = X | Y,
    XZ = X | Z,
    YZ = Y | Z,
    XYZ = X | Y | Z,
    All = X | Y | Z | W
};

enum class SwizzleMask : uint8_t {
    //// 2D Out

    // 2D In
    XY,
    YX,
    XX,
    YY,

    // 3D In
    XZ,
    YZ,
    ZX,
    ZY,
    ZZ,

    // 4D In
    XW,
    YW,
    ZW,
    WX,
    WY,
    WZ,
    WW,

    //// 3D Out

    // 2D In
    XXX,
    XXY,
    XYX,
    XYY,

    YXX,
    YYX,
    YXY,
    YYY,

    // 3D In
    XYZ,

    YXZ,
    XZY,
    XXZ,
    XZX,
    XZZ,

    YZX,
    YZY,
    YYZ,
    YZZ,

    ZYX,
    ZXY,
    ZZX,
    ZZY,
    ZXZ,
    ZYZ,
    ZZZ,

    // 4D In
    XYW,
    XWZ,
    XWW,
    XXW,

    YXW,
    YZW,
    YWX,
    YWZ,
    YYW,

    ZXW,
    ZYW,
    ZZW,
    ZWX,
    ZWY,

    WXY,
    WXZ,
    WYX,
    WYZ,
    WZY,
    WWX,
    WWY,
    WWZ,
    WWW,

    //// 4D Out

    // 2D In
    XXXX,
    XXXY,
    XXYY,
    XYYY,
    XYXY,

    YYYX,
    YYXX,
    YXXX,
    YXYX,
    YYYY,

    // 3D In
    XXYZ,
    XZXZ,
    XXZZ,
    XYZZ,
    XZZZ,

    YYXZ,
    YZYZ,
    YXZZ,
    YYZX,
    YYZY,

    ZXYZ,
    ZYYZ,
    ZYXZ,
    ZZXX,
    ZZYY,
    ZZZZ,
    // add more 3d swizzles if needed

    // 4D In
    XYZW,
    WZYX,
    XXWW,
    YYWW,
    ZZWW,
    WWWW

    // add more 4d swizzles if needed
};
} // namespace onyx