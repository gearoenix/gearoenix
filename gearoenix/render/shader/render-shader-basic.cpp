#include "render-shader-basic.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "stage/render-shader-stage-fragment.hpp"
#include "stage/render-shader-stage-vertex.hpp"
gearoenix::render::shader::Basic::Basic(std::shared_ptr<system::File>& file, Engine* engine)
{
    std::vector<std::uint8_t> data;
    file->read(data);
    vertex = new stage::Vertex(data, engine);
    file->read(data);
    fragment = new stage::Fragment(data, engine);
}
