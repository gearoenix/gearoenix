#include "gx-rnd-cmr-colour-tuning.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

void gearoenix::render::camera::ColourTuning::GammaCorrection::write(platform::stream::Stream& s) const
{
    gamma_exponent.write(s);
}

void gearoenix::render::camera::ColourTuning::GammaCorrection::read(platform::stream::Stream& s)
{
    gamma_exponent.read(s);
}

void gearoenix::render::camera::ColourTuning::Multiply::write(platform::stream::Stream& s) const
{
    scale.write(s);
}

void gearoenix::render::camera::ColourTuning::Multiply::read(platform::stream::Stream& s)
{
    scale.read(s);
}

const gearoenix::render::camera::ColourTuning::GammaCorrection& gearoenix::render::camera::ColourTuning::get_gamma_correction() const
{
    GX_ASSERT(index == gamma_correction_index);
    return data.gamma_correction;
}

const gearoenix::render::camera::ColourTuning::Multiply& gearoenix::render::camera::ColourTuning::get_multiply() const
{
    GX_ASSERT(index == multiply_index);
    return data.multiply;
}

const gearoenix::render::camera::ColourTuning::Unchanged& gearoenix::render::camera::ColourTuning::get_unchanged() const
{
    GX_ASSERT(index == unchanged_index);
    return data.unchanged;
}

bool gearoenix::render::camera::ColourTuning::show_debug_gui()
{
    GX_UNIMPLEMENTED;
}

void gearoenix::render::camera::ColourTuning::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(index);
    switch (index) {
    case gamma_correction_index:
        data.gamma_correction.write(s);
        break;
    case multiply_index:
        data.multiply.write(s);
        break;
    case unchanged_index:
        break;
    default: {
        GX_UNEXPECTED;
    }
    }
}

void gearoenix::render::camera::ColourTuning::read(platform::stream::Stream& s)
{
    s.read(index);
    switch (index) {
    case gamma_correction_index:
        data.gamma_correction.read(s);
        break;
    case multiply_index:
        data.multiply.read(s);
        break;
    case unchanged_index:
        break;
    default: {
        GX_UNEXPECTED;
    }
    }
}
