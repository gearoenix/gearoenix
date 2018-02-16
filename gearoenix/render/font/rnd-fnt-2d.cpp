#include "rnd-fnt-2d.hpp"
#include "../../system/file/sys-fl-local.hpp"
#include "../../system/file/sys-fl-memory.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../../external/stb/stb_truetype.h"
#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../../external/stb/stb_image_write.h"
#include "../rnd-engine.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

#define BAKED_ASPECT 1024
#define FIRST_CHAR 33
#define NUM_CHARS 94

struct PngWriterContext {
    gearoenix::system::file::Memory* m;
    gearoenix::system::file::Local* l;
};

static void png_write_function(void *context, void *data, int size) {
    PngWriterContext* png_ctx = reinterpret_cast<PngWriterContext*>(context);
    png_ctx->m->write(data, size);
    png_ctx->l->write(data, size);
}

gearoenix::render::font::Font2D::Font2D(core::Id my_id, system::file::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : Font(my_id, Shape::D2)
{
    const std::string baked_file_name = std::string("baked_font") + std::to_string(my_id) + ".bin";
    letters_properties.resize(NUM_CHARS);
    const bool baked_exist = system::file::Local::exist(baked_file_name);
    if(!baked_exist){
        system::file::Local baked_file(baked_file_name, true);
        const int root_nchar = (int)std::ceil(std::sqrt((float)NUM_CHARS));
        std::vector<stbtt_bakedchar> bkchar(NUM_CHARS);
        std::vector<unsigned char> ttbuffer;
        f->read(ttbuffer);
        std::vector<unsigned char> atlas_data(BAKED_ASPECT * BAKED_ASPECT);
        stbtt_BakeFontBitmap(
                    ttbuffer.data(), 0,
                    BAKED_ASPECT / root_nchar,
                    &(atlas_data[0]), BAKED_ASPECT, BAKED_ASPECT,
                    FIRST_CHAR, NUM_CHARS,
                    &(bkchar[0]));
        for (int i = 0; i < NUM_CHARS; ++i) {
             stbtt_aligned_quad q;
             float xx = 0.0f, yy = 0.0f;
             stbtt_GetBakedQuad(bkchar.data(), BAKED_ASPECT, BAKED_ASPECT, i, &xx, &yy, &q, 1);
             LetterProperties lp;
             lp.pos_max = math::Vec2(q.x1, q.y1);
             lp.pos_min = math::Vec2(q.x0, q.y0);
             lp.uv_max = math::Vec2(q.s1, q.t1);
             lp.uv_min = math::Vec2(q.s0, q.t0);
             letters_properties[i] = lp;
         }
        baked_file.write(letters_properties.data(), letters_properties.size());
        system::file::Memory m;
        core::Count pngsize = 0;
        m.write(&pngsize, sizeof(pngsize));
        baked_file.write(&pngsize, sizeof(pngsize));
        const unsigned int pngsizepos = baked_file.tell();
        PngWriterContext pngctx;
        pngctx.m = &m;
        pngctx.l = &baked_file;
        stbi_write_png_to_func(png_write_function, &pngctx, BAKED_ASPECT, BAKED_ASPECT, 1, atlas_data.data(), 0);
        pngsize = m.tell() - sizeof(pngsize);
        m.seek(0);
        baked_file.seek(pngsizepos);
        m.write(&pngsize, sizeof(pngsize));
        baked_file.write(&pngsize, sizeof(pngsize));
        m.seek(0);
        baked_texture = std::shared_ptr<texture::Texture2D>(e->create_texture_2d(&m, core::EndCaller<core::EndCallerIgnore>([c](std::shared_ptr<core::EndCallerIgnore>) -> void {})));
    } else {
        system::file::Local baked_file(baked_file_name);
        baked_file.read(&(letters_properties[0]), letters_properties.size());
        baked_texture = std::shared_ptr<texture::Texture2D>(e->create_texture_2d(&baked_file, core::EndCaller<core::EndCallerIgnore>([c](std::shared_ptr<core::EndCallerIgnore>) -> void {})));
    }
}

gearoenix::render::font::Font2D::~Font2D()
{
}
