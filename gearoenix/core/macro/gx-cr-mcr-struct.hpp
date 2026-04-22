#pragma once

#define GX_STRUCT_DEF(name, body) \
    struct name final body;       \
    constexpr char name##_def_str[] = "struct " #name " " #body ";"