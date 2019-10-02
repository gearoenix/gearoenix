#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include "../core/asset/cr-asset.hpp"
#include <memory>
namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace audio {
    class Audio : public core::asset::Asset {
    private:
    protected:
        Audio(core::Id my_id, const std::shared_ptr<system::stream::Stream>& f) noexcept;

    public:
        virtual ~Audio() noexcept;
        static Audio* read(core::Id my_id, const std::shared_ptr<system::stream::Stream>& f) noexcept;
    };
}
}
#endif
