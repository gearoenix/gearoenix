#pragma once

#define GX_FLAG_CHECK(x, flag) ((static_cast<std::uint64_t>(x) & static_cast<std::uint64_t>(flag)) == static_cast<std::uint64_t>(flag))