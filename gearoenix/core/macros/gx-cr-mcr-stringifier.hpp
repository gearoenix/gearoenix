#ifndef GEAROENIX_CORE_MACROS_STRINGIFIER_HPP
#define GEAROENIX_CORE_MACROS_STRINGIFIER_HPP
#define GX_STR_0(a) #a
#define GX_STR_1(a) GX_STR_0(a)
#define GX_STR_2(a) GX_STR_1(a)
#define GX_STR_3(a) GX_STR_2(a)
#define GX_STR_4(a) GX_STR_3(a)
#define GX_STR_5(a) GX_STR_4(a)
#define GX_STR_6(a) GX_STR_5(a)
#define GX_STR_7(a) GX_STR_6(a)
#define GX_STR_8(a) GX_STR_7(a)
#define GX_STR_9(a) GX_STR_8(a)
#define GX_STRINGIFY(a) GX_STR_9(a)
#endif