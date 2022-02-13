#pragma once
#include "direct/gles.h"
#include "GlHeaders.h"
#include "gl_objects.h"

namespace mmapp {

static const int max_textures = 16;

class CGlEs : public IGlEs {
    GLenum      depth_format_;
    bool        interleaved_stencil_;
    GLuint      framebuffer_;
    GLuint      original_fb_;
    bool        using_offline_target_;
    GLuint      bkbuf_width_;
    GLuint      bkbuf_height_;
    bool        buffers_changed_;
    GLint       enabled_attributes_;
    GlError     error_;

    std::shared_ptr<const CColorTarget>   curr_color_;
    std::shared_ptr<const CDepthTarget>   curr_depth_;

    std::vector<std::shared_ptr<const CVertexBuffer>> curr_bufs_;
    std::vector<std::shared_ptr<const Buffer>>        curr_cpu_vertices_;
    std::shared_ptr<const CIndexBuffer>               curr_indices_;
    std::shared_ptr<const CglEsProgram>         curr_program_;
    std::shared_ptr<const CTexture>             curr_textures_[max_textures];
    std::shared_ptr<const CColorTarget>         curr_target_textures_[max_textures];

    bool setupForDrawing(void);
    void setError(GlError error);
public:
    virtual ~CGlEs();
    virtual void *get__id() const { return(nullptr); }

    void init(int32_t width, int32_t height);
    void update_bkbuf_size(int32_t width, int32_t height);

    // nfo
    virtual GlError getError();
    virtual std::string getString(StringInfo info_type) const;

    // resources
    virtual std::shared_ptr<IVertexBuffer> createVertexBuffer(const Buffer &content);
    virtual std::shared_ptr<IVertexBuffer> createVertexBufferF32(const std::vector<float> &content, const std::vector<std::string> &names,
        const std::vector<int32_t> &dimensions);
    virtual std::shared_ptr<IIndexBuffer> createIndexBuffer(const std::vector<uint16_t> &content);
    virtual std::shared_ptr<ITexture> createTexture(int32_t width, int32_t height, TexFilter filtering, TexWrap wrap_mode, TexUsage purpose);
    virtual std::shared_ptr<IglEsProgram> createProgram(const char *vs_source, const char *fs_source, std::string *errordesc);
    virtual std::shared_ptr<IColorTarget> createColorTarget(int32_t width, int32_t height);
    virtual std::shared_ptr<IDepthTarget> createDepthTarget(int32_t width, int32_t height);

    // inputs
    virtual void uniform1f(int32_t index, float value);
    virtual void uniform2f(int32_t index, float v0, float v1);
    virtual void uniform3f(int32_t index, float v0, float v1, float v2);
    virtual void uniform4f(int32_t index, float v0, float v1, float v2, float v3);
    virtual void uniform1i(int32_t index, int32_t value);
    virtual void uniform2i(int32_t index, int32_t v0, int32_t v1);
    virtual void uniform3i(int32_t index, int32_t v0, int32_t v1, int32_t v2);
    virtual void uniform4i(int32_t index, int32_t v0, int32_t v1, int32_t v2, int32_t v3);
    virtual void uniformMatrix2f(int32_t index, const sing::array<sing::array<float, 2>, 2> &values, bool transpose);
    virtual void uniformMatrix3f(int32_t index, const sing::array<sing::array<float, 3>, 3> &values, bool transpose);
    virtual void uniformMatrix4f(int32_t index, const sing::array<sing::array<float, 4>, 4> &values, bool transpose);
    virtual void setTextureIndex(const char *uniform_name, int32_t stage);

    virtual void useVbuffer(std::shared_ptr<const IVertexBuffer> vb);
    virtual void useAdditionalVbuffer(std::shared_ptr<const IVertexBuffer> vb);
    virtual void useCpuVertices(std::shared_ptr<const Buffer> vertices);
    virtual void useAdditionalCpuVertices(std::shared_ptr<const Buffer> vertices);
    virtual void useIbuffer(std::shared_ptr<const IIndexBuffer> ib);
    virtual void useProgram(std::shared_ptr<const IglEsProgram> prog);
    virtual void useTexture(std::shared_ptr<const ITexture> texture, int32_t stage, TexUsage purpose = TexUsage::plain);
    virtual void useColorTargetAsTexture(std::shared_ptr<const IColorTarget> texture, int32_t stage);

    virtual void useBackBuffer();
    virtual void useOfflineTarget();
    virtual std::shared_ptr<const IColorTarget> selectOfflineColor(std::shared_ptr<const IColorTarget> target);
    virtual std::shared_ptr<const IDepthTarget> selectOfflineDepth(std::shared_ptr<const IDepthTarget> target);

    // options
    virtual void viewport(int32_t left_x, int32_t lower_y, int32_t width, int32_t height);
    virtual void scissor(int32_t left_x, int32_t lower_y, int32_t width, int32_t height);
    virtual void depthRangef(float min_z, float max_z);
    virtual void lineWidth(float width);
    virtual void frontFace(FaceMode mode);
    virtual void cullFace(FaceOrientation mode);
    virtual void polygonOffset(float factor, float units);
    virtual void stencilFunc(CompareFunc func, int32_t ref, uint32_t mask);
    virtual void stencilFuncSeparate(FaceOrientation face, CompareFunc func, int32_t ref, uint32_t mask);
    virtual void stencilOp(StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass);
    virtual void stencilOpSeparate(FaceOrientation face, StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass);
    virtual void depthFunc(CompareFunc func);
    virtual void blendEquation(BlendOp op);
    virtual void blendEquationSeparate(BlendOp rgb, BlendOp alpha);
    virtual void blendFunc(BlendWeight src, BlendWeight dst);
    virtual void blendFuncSeparate(BlendWeight rgb_src, BlendWeight rgb_dst, BlendWeight alpha_src, BlendWeight alpha_dst);
    virtual void blendColor(float red, float green, float blue, float alpha);
    virtual void colorMask(bool red, bool green, bool blue, bool alpha);
    virtual void depthMask(bool write_enable);
    virtual void stencilMask(uint32_t mask);
    virtual void stencilMaskSeparate(FaceOrientation face, uint32_t mask);
    virtual void enable(FeatureId feature);
    virtual void disable(FeatureId feature);

    // options for clear
    virtual void clearColor(float red, float green, float blue, float alpha);
    virtual void clearDepth(float depth);
    virtual void clearStencil(int32_t value);

    // commands
    virtual void readPixels(std::vector<uint8_t> *data, int32_t left_x, int32_t lower_y, int32_t width, int32_t height) const;
    virtual void clear(bool color, bool depth, bool stencil);
    virtual void flush();
    virtual void finish();
    virtual void draw(DrawPrimitive mode, int32_t first, int32_t count);
    virtual void drawElements(DrawPrimitive mode, int32_t first, int32_t count);
    virtual void drawCpuElements(DrawPrimitive mode, const std::vector<uint16_t> &indices);
};

} // namespace