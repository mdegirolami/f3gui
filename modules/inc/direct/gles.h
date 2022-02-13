#pragma once

#include <sing.h>

namespace mmapp {

enum class ComponentType {unsigned8, signed8, unsigned16, signed16, float32};

struct ComponentDesc {
    std::string     name;
    int32_t         dimensions;
    ComponentType   ctype;
    int32_t         offset;
    bool            normalized;
};

class Buffer final {
    std::vector<ComponentDesc> components_;
    std::vector<uint8_t> content_;
    int32_t vertex_size_;

public:
    Buffer();
    const std::vector<ComponentDesc> *getComponents() const { return(&components_); }
    const std::vector<uint8_t> *getContent() const { return(&content_); }
    int32_t getVertexSize() const { return(vertex_size_); }

    void clear();
    bool addComponent(const char *name, int32_t dimensions, ComponentType ctype, bool normalized = false);
    bool getComponentInfo(int32_t idx, std::string *name, int32_t *dimensions, ComponentType *ctype, bool *normalized) const;
    bool resize(int32_t num_vertices);

    bool setI8Component(const std::vector<int8_t> &values, int32_t component_idx, int32_t vertex_idx = 0);
    bool setI16Component(const std::vector<int16_t> &values, int32_t component_idx, int32_t vertex_idx = 0);
    bool setU8Component(const std::vector<uint8_t> &values, int32_t component_idx, int32_t vertex_idx = 0);
    bool setU16Component(const std::vector<uint16_t> &values, int32_t component_idx, int32_t vertex_idx = 0);
    bool setF32Component(const std::vector<float> &values, int32_t component_idx, int32_t vertex_idx = 0);

    void getI8Component(std::vector<int8_t> *values, int32_t component_idx, int32_t vertex_idx = 0, int32_t count = 0) const;
    void getI16Component(std::vector<int16_t> *values, int32_t component_idx, int32_t vertex_idx = 0, int32_t count = 0) const;
    void getU8Component(std::vector<uint8_t> *values, int32_t component_idx, int32_t vertex_idx = 0, int32_t count = 0) const;
    void getU16Component(std::vector<uint16_t> *values, int32_t component_idx, int32_t vertex_idx = 0, int32_t count = 0) const;
    void getF32Component(std::vector<float> *values, int32_t component_idx, int32_t vertex_idx = 0, int32_t count = 0) const;
};

enum class GlError {invalid_enum, invalid_value, invalid_operation, out_of_memory, no_error, other};
enum class StringInfo {vendor, renderer, version, sl_version, extensions};
enum class TexFormat {rgb, rgba, luminance, lumnance_alpha, alpha};
enum class TexUsage {plain, cube};
enum class TexWrap {wrap, clamp};
enum class TexFilter {nearest, bilinear, trilinear};
enum class FaceMode {ccw, cw};
enum class FaceOrientation {front, back, both};
enum class FeatureId {cull, poly_offset, scissor, depth_test, stencil_test, blend};
enum class CompareFunc {never, always, less, lequal, equal, greater, gequal, notequal};
enum class StencilOp {keep, zero, replace, inc, dec, invert, inc_wrap, dec_wrap};
enum class BlendOp {add, src_minus_dst, dst_minus_src};
enum class BlendWeight {zero, one, src_color, one_minus_src_color, dst_color, one_minus_dst_color, src_alpha, one_minus_src_alpha, dst_alpha,
    one_minus_dst_alpha, constant_color, one_minus_constant_color, constant_alpha, one_minus_constant_alpha, src_alpha_saturate};
enum class DrawPrimitive {points, line_strip, line_loop, lines, triangle_strip, triangle_fan, triangles};

class IVertexBuffer {
public:
    virtual ~IVertexBuffer() {}
    virtual void *get__id() const = 0;
    virtual bool fill(const Buffer &content) = 0;
    virtual bool fillF32(const std::vector<float> &content, const std::vector<std::string> &names, const std::vector<int32_t> &dimensions) = 0;
};

class IIndexBuffer {
public:
    virtual ~IIndexBuffer() {}
    virtual void *get__id() const = 0;
    virtual bool fill(const std::vector<uint16_t> &content) = 0;
};

class IglEsProgram {
public:
    virtual ~IglEsProgram() {}
    virtual void *get__id() const = 0;
    virtual int32_t getUniformLocation(const char *name) const = 0;
};

class ITexture {
public:
    virtual ~ITexture() {}
    virtual void *get__id() const = 0;
    virtual bool fill(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level, TexFormat format = TexFormat::rgba,
        int32_t cubeface = -1, bool autoMipMaps = true) = 0;

    virtual bool fillCompressed(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level, int32_t cubeface = -1) = 0;
};

class IColorTarget {
public:
    virtual ~IColorTarget() {}
    virtual void *get__id() const = 0;
    virtual int32_t getWidth() const = 0;
    virtual int32_t getHeight() const = 0;
};

class IDepthTarget {
public:
    virtual ~IDepthTarget() {}
    virtual void *get__id() const = 0;
};

class IGlEs {
public:
    virtual ~IGlEs() {}
    virtual void *get__id() const = 0;

    // nfo
    virtual GlError getError() = 0;
    virtual std::string getString(StringInfo info_type) const = 0;

    // resources
    virtual std::shared_ptr<IVertexBuffer> createVertexBuffer(const Buffer &content) = 0;
    virtual std::shared_ptr<IVertexBuffer> createVertexBufferF32(const std::vector<float> &content, const std::vector<std::string> &names,
        const std::vector<int32_t> &dimensions) = 0;
    virtual std::shared_ptr<IIndexBuffer> createIndexBuffer(const std::vector<uint16_t> &content) = 0;
    virtual std::shared_ptr<ITexture> createTexture(int32_t width, int32_t height, TexFilter filtering = TexFilter::trilinear,
        TexWrap wrap_mode = TexWrap::wrap, TexUsage purpose = TexUsage::plain) = 0;
    virtual std::shared_ptr<IglEsProgram> createProgram(const char *vs_source, const char *fs_source, std::string *errordesc) = 0;
    virtual std::shared_ptr<IColorTarget> createColorTarget(int32_t width, int32_t height) = 0;
    virtual std::shared_ptr<IDepthTarget> createDepthTarget(int32_t width, int32_t height) = 0;

    // inputs
    virtual void uniform1f(int32_t index, float value) = 0;
    virtual void uniform2f(int32_t index, float v0, float v1) = 0;
    virtual void uniform3f(int32_t index, float v0, float v1, float v2) = 0;
    virtual void uniform4f(int32_t index, float v0, float v1, float v2, float v3) = 0;
    virtual void uniform1i(int32_t index, int32_t value) = 0;
    virtual void uniform2i(int32_t index, int32_t v0, int32_t v1) = 0;
    virtual void uniform3i(int32_t index, int32_t v0, int32_t v1, int32_t v2) = 0;
    virtual void uniform4i(int32_t index, int32_t v0, int32_t v1, int32_t v2, int32_t v3) = 0;
    virtual void uniformMatrix2f(int32_t index, const sing::array<sing::array<float, 2>, 2> &values, bool transpose = false) = 0;
    virtual void uniformMatrix3f(int32_t index, const sing::array<sing::array<float, 3>, 3> &values, bool transpose = false) = 0;
    virtual void uniformMatrix4f(int32_t index, const sing::array<sing::array<float, 4>, 4> &values, bool transpose = false) = 0;
    virtual void setTextureIndex(const char *uniform_name, int32_t stage) = 0;

    virtual void useVbuffer(std::shared_ptr<const IVertexBuffer> vb) = 0;
    virtual void useAdditionalVbuffer(std::shared_ptr<const IVertexBuffer> vb) = 0;
    virtual void useCpuVertices(std::shared_ptr<const Buffer> vertices) = 0;
    virtual void useAdditionalCpuVertices(std::shared_ptr<const Buffer> vertices) = 0;
    virtual void useIbuffer(std::shared_ptr<const IIndexBuffer> ib) = 0;
    virtual void useProgram(std::shared_ptr<const IglEsProgram> prog) = 0;
    virtual void useTexture(std::shared_ptr<const ITexture> texture, int32_t stage, TexUsage purpose = TexUsage::plain) = 0;
    virtual void useColorTargetAsTexture(std::shared_ptr<const IColorTarget> texture, int32_t stage) = 0;

    virtual void useBackBuffer() = 0;
    virtual void useOfflineTarget() = 0;
    virtual std::shared_ptr<const IColorTarget> selectOfflineColor(std::shared_ptr<const IColorTarget> target) = 0;
    virtual std::shared_ptr<const IDepthTarget> selectOfflineDepth(std::shared_ptr<const IDepthTarget> target) = 0;

    // options
    virtual void viewport(int32_t left_x, int32_t lower_y, int32_t width, int32_t height) = 0;
    virtual void scissor(int32_t left_x, int32_t lower_y, int32_t width, int32_t height) = 0;
    virtual void depthRangef(float min_z, float max_z) = 0;
    virtual void lineWidth(float width) = 0;
    virtual void frontFace(FaceMode mode) = 0;
    virtual void cullFace(FaceOrientation mode) = 0;
    virtual void polygonOffset(float factor, float units) = 0;
    virtual void stencilFunc(CompareFunc func, int32_t ref, uint32_t mask) = 0;
    virtual void stencilFuncSeparate(FaceOrientation face, CompareFunc func, int32_t ref, uint32_t mask) = 0;
    virtual void stencilOp(StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass) = 0;
    virtual void stencilOpSeparate(FaceOrientation face, StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass) = 0;
    virtual void depthFunc(CompareFunc func) = 0;
    virtual void blendEquation(BlendOp op) = 0;
    virtual void blendEquationSeparate(BlendOp rgb, BlendOp alpha) = 0;
    virtual void blendFunc(BlendWeight src, BlendWeight dst) = 0;
    virtual void blendFuncSeparate(BlendWeight rgb_src, BlendWeight rgb_dst, BlendWeight alpha_src, BlendWeight alpha_dst) = 0;
    virtual void blendColor(float red, float green, float blue, float alpha) = 0;
    virtual void colorMask(bool red, bool green, bool blue, bool alpha) = 0;
    virtual void depthMask(bool write_enable) = 0;
    virtual void stencilMask(uint32_t mask) = 0;
    virtual void stencilMaskSeparate(FaceOrientation face, uint32_t mask) = 0;
    virtual void enable(FeatureId feature) = 0;
    virtual void disable(FeatureId feature) = 0;

    // options for clear
    virtual void clearColor(float red, float green, float blue, float alpha) = 0;
    virtual void clearDepth(float depth) = 0;
    virtual void clearStencil(int32_t value) = 0;

    // commands
    virtual void readPixels(std::vector<uint8_t> *data, int32_t left_x, int32_t lower_y, int32_t width, int32_t height) const = 0;
    virtual void clear(bool color, bool depth, bool stencil) = 0;
    virtual void flush() = 0;
    virtual void finish() = 0;
    virtual void draw(DrawPrimitive mode, int32_t first, int32_t count) = 0;
    virtual void drawElements(DrawPrimitive mode, int32_t first, int32_t count) = 0;
    virtual void drawCpuElements(DrawPrimitive mode, const std::vector<uint16_t> &indices) = 0;
};

}   // namespace
