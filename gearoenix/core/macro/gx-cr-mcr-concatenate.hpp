#ifndef GEAROENIX_CORE_MACRO_CONCATENATE_HPP
#define GEAROENIX_CORE_MACRO_CONCATENATE_HPP

#define GX_CONCAT_0(x, y) x##y
#define GX_CONCAT_1(x, y) GX_CONCAT_0(x, y)
#define GX_CONCAT_2(x, y) GX_CONCAT_1(x, y)
#define GX_CONCAT_3(x, y) GX_CONCAT_2(x, y)
#define GX_CONCAT_4(x, y) GX_CONCAT_3(x, y)
#define GX_CONCAT_5(x, y) GX_CONCAT_4(x, y)

#endif