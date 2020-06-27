#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include "../core/asset/cr-asset.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}
namespace gearoenix::audio {
class Audio : public core::asset::Asset {
private:
protected:
    Audio(core::Id my_id, std::string name, const std::shared_ptr<system::stream::Stream>& f) noexcept;

public:
    ~Audio() noexcept override;
    [[nodiscard]] static Audio* read(core::Id my_id, std::string name, const std::shared_ptr<system::stream::Stream>& f) noexcept;
};
}
#endif
