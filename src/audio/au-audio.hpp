#ifndef GEAROENIX_AUDIO_AUDIO_HPP
#define GEAROENIX_AUDIO_AUDIO_HPP
#include "../core/asset/cr-asset.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace audio {
    class Audio : public core::asset::Asset {
    private:
    protected:
        Audio(const core::Id my_id, const std::shared_ptr<system::stream::Stream>& f);

    public:
        virtual ~Audio();
        static Audio* read(const core::Id my_id, const std::shared_ptr<system::stream::Stream>& f);
    };
} // namespace audio
} // namespace gearoenix
#endif // GEAROENIX_AUDIO_AUDIO_HPP
