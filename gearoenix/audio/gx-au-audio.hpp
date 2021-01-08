#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include "../core/asset/gx-cr-asset.hpp"
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
}
namespace gearoenix::audio {
struct Audio : public core::asset::Asset {
private:
protected:
    Audio(core::Id my_id, std::string name, const std::shared_ptr<platform::stream::Stream>& f) noexcept;

public:
    ~Audio() noexcept override;
    [[nodiscard]] static Audio* read(core::Id my_id, std::string name, const std::shared_ptr<platform::stream::Stream>& f) noexcept;
};
}
#endif
