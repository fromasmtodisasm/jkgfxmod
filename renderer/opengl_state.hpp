#pragma once

#include "base/filesystem.hpp"
#include "common/config.hpp"
#include "glutil/buffer.hpp"
#include "glutil/framebuffer.hpp"
#include "glutil/program.hpp"
#include "glutil/renderbuffer.hpp"
#include "glutil/shader.hpp"
#include "glutil/texture.hpp"
#include "glutil/vertex_array.hpp"
#include <map>
#include <optional>
#include <vector>

namespace jkgm {
    gl::shader compile_shader_from_file(fs::path const &filename, gl::shader_type type);

    void link_program_from_files(std::string const &name,
                                 gl::program *prog,
                                 fs::path const &vx,
                                 fs::path const &fg);

    class post_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        post_model();
    };

    class overlay_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer tcb;
        gl::buffer ib;
        unsigned int num_indices = 6U;

        overlay_model(size<2, int> scr_res, box<2, int> actual_scr_area);
    };

    class hud_model {
    public:
        gl::vertex_array vao;
        gl::buffer vb;
        gl::buffer tcb;
        gl::buffer ib;
        unsigned int num_indices = 0U;

        hud_model(size<2, int> scr_res,
                  size<2, int> int_scr_res,
                  box<2, int> actual_scr_area,
                  float scale);
    };

    class render_depthbuffer {
    public:
        gl::renderbuffer rbo;

        box<2, int> viewport;

        explicit render_depthbuffer(size<2, int> dims);
    };

    class render_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;

        box<2, int> viewport;

        render_buffer(size<2, int> dims, render_depthbuffer *rbo);
    };

    class render_gbuffer {
    public:
        gl::framebuffer fbo;
        gl::texture color_tex;
        gl::texture emissive_tex;
        gl::texture depth_nrm_tex;

        box<2, int> viewport;

        render_gbuffer(size<2, int> dims, render_depthbuffer *rbo);
    };

    class ssao_occlusion_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        box<2, int> viewport;

        explicit ssao_occlusion_buffer(size<2, int> dims);
    };

    class post_buffer {
    public:
        gl::framebuffer fbo;
        gl::texture tex;
        box<2, int> viewport;

        explicit post_buffer(size<2, int> dims);
    };

    class hdr_stack_em {
    public:
        post_buffer a;
        post_buffer b;
        int num_passes;
        float weight;

        hdr_stack_em(size<2, int> dims, int num_passes, float weight);
    };

    class hdr_stack {
    public:
        std::vector<hdr_stack_em> elements;

        hdr_stack();
    };

    struct alignas(32) triangle_buffer_vertex {
        point<4, float> pos;
        point<2, float> texcoords;
        jkgm::color col;
        direction<3, float> normal;
    };

    class triangle_buffer_model {
    private:
        gl::buffer vbo;
        unsigned int vb_capacity = 0U;

    public:
        gl::vertex_array vao;

        span<triangle_buffer_vertex> mmio;
        int num_vertices = 0;

        triangle_buffer_model();

        void maybe_grow_buffers(unsigned int new_capacity);
        void update_buffers();
    };

    struct triangle_buffer_models {
        triangle_buffer_model world_trimdl;
        triangle_buffer_model world_transparent_trimdl;
        triangle_buffer_model gun_trimdl;
        triangle_buffer_model gun_transparent_trimdl;
    };

    class triangle_buffer_sequence {
    private:
        std::vector<triangle_buffer_models> trimdls;
        std::vector<triangle_buffer_models>::iterator it;

    public:
        triangle_buffer_sequence();

        triangle_buffer_models *get_current();
        void swap_next();
    };

    struct srgb_texture {
        size<2, int> dims;
        gl::texture handle;
        int refct = 0;
        std::optional<fs::path> origin_filename;

        explicit srgb_texture(size<2, int> dims);
    };

    struct linear_texture {
        size<2, int> dims;
        gl::texture handle;
        int refct = 0;
        std::optional<fs::path> origin_filename;

        explicit linear_texture(size<2, int> dims);
    };

    struct opengl_state {
        gl::program menu_program;

        gl::program game_opaque_pass_program;
        gl::program game_post_ssao_program;
        gl::program game_post_opaque_composite_program;

        gl::program game_transparency_pass_program;

        gl::program post_box4;
        gl::program post_gauss7;
        gl::program post_low_pass;
        gl::program post_to_srgb;

        post_model postmdl;
        overlay_model menumdl;
        hud_model hudmdl;
        triangle_buffer_sequence tribuf;

        gl::texture menu_texture;
        std::vector<color_rgba8> menu_texture_data;

        gl::texture hud_texture;
        std::vector<color_rgba8> hud_texture_data;

        std::unique_ptr<ssao_occlusion_buffer> ssao_occlusionbuffer;
        std::unique_ptr<ssao_occlusion_buffer> ssao_occlusionbuffer2;
        std::unique_ptr<gl::texture> ssao_noise_texture;

        render_depthbuffer shared_depthbuffer;

        render_buffer screen_renderbuffer;
        post_buffer screen_postbuffer1;
        post_buffer screen_postbuffer2;

        render_gbuffer gbuffer;

        hdr_stack bloom_layers;

        std::vector<srgb_texture> srgb_textures;
        std::map<fs::path, size_t> file_to_srgb_texture_map;

        std::vector<linear_texture> linear_textures;
        std::map<fs::path, size_t> file_to_linear_texture_map;

        opengl_state(size<2, int> screen_res,
                     size<2, int> internal_screen_res,
                     box<2, int> actual_screen_area,
                     config const *the_config);
    };
}