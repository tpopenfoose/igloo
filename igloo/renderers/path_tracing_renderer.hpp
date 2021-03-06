#pragma once

#include <igloo/renderers/renderer.hpp>
#include <igloo/primitives/scene.hpp>
#include <igloo/records/image.hpp>

namespace igloo
{


class path_tracing_renderer : public renderer
{
  public:
    path_tracing_renderer(const scene &s, image &im, std::size_t max_path_length = 10);

    void render(const float4x4 &modelview, render_progress &progress);

  private:
    const scene &scene_;
    image &image_;
    size_t max_path_length_;
};


}

