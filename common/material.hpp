#pragma once

#include "base/filesystem.hpp"
#include "math/color.hpp"
#include <optional>

namespace jkgm {
    enum class material_alpha_mode { blend, mask };

    class material {
    public:
        std::optional<fs::path> albedo_map;
        color albedo_factor = color::fill(1.0f);

        std::optional<fs::path> emissive_map;
        color_rgb emissive_factor = color_rgb::zero();

        std::optional<fs::path> displacement_map;
        float displacement_factor = 0.0f;

        material_alpha_mode alpha_mode = material_alpha_mode::blend;
        float alpha_cutoff = 0.5f;
    };
}