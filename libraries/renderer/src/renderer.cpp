#include "renderer.hpp"
#include "opengl.hpp"

// NV12
namespace
{
    const char* vs =
        " precision highp float; \n "
        " layout (location = 0) in vec3 aPos; \n"
        " layout (location = 1) in vec2 aTexCoord; \n"
        " out vec2 TexCoord; \n"
        " void main() \n"
        " { \n"
        " gl_Position = vec4(aPos, 1.0); \n"
        " TexCoord = aTexCoord; \n"
        " } \n";

    const char* fs =
        "precision highp float;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main()\n"
        "{\n"
        "vec4 texel = texture(uTexture, TexCoord);\n"
        "FragColor = texel;\n"
        "}\n";

    const char* fs_gray =
        "precision highp float;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main()\n"
        "{\n"
        "vec4 texel = texture(uTexture, TexCoord);\n"
        "FragColor = vec4(texel.x, texel.x, texel.x, 1.0);\n"
        "}\n";
    }

    namespace bnb::render
    {
        renderer::renderer(int width, int height)
            : m_program("RendererCamera", vs, fs)
            , m_program_gray("RendererCameraGray", vs, fs_gray)
            , m_frame_surface(BNB_DEG_0_ALIAS, false)
        {
            surface_change(width, height);
        }

        void renderer::surface_change(int32_t width, int32_t height)
        {
            m_width = width;
            m_height = height;
            m_surface_changed = true;
        }

        void renderer::update_data(int texture_id, bool draw_gray)
        {
            if (m_texture_updated && m_rendering) {
                return;
            }

            m_texture_updated = false;
            m_texture_id = texture_id;
            m_draw_gray = draw_gray;
            m_texture_updated = true;
        }

        bool renderer::draw()
        {
            if (!m_texture_updated) {
                return false;
            }

            if (m_surface_changed) {
                GL_CALL(glViewport(0, 0, m_width, m_height));
                m_surface_changed = false;
            }

            m_rendering = true;

            m_texture_updated = false;

            if (m_draw_gray) {
                m_program_gray.use();
            } else {
                m_program.use();
            }

            GL_CALL(glActiveTexture(GLenum(GL_TEXTURE0)));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_texture_id));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            m_frame_surface.draw();

            m_program.unuse();
            m_rendering = false;

            return true;
        }

    } // namespace bnb::render
