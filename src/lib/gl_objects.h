#pragma once

#include "direct/gles.h"
#include "GlHeaders.h"

namespace mmapp {

static const int max_v_components = 16;

bool isPowerOfTwo(int32_t value);
GLenum glpixelFromFormat(TexFormat format);
GLenum componentType2Gl(ComponentType ctype);

class CglEsProgram;

class CVertexBuffer : public IVertexBuffer {
    GLuint  buffer_;
    std::vector<ComponentDesc> components_;
    int32_t vertex_size_;
public:
    CVertexBuffer(GLuint bufid) { buffer_ = bufid; vertex_size_ = 0; }
    virtual ~CVertexBuffer() { if (buffer_ != 0) glDeleteBuffers(1, &buffer_); }
    virtual void *get__id() const { return(nullptr); }
    virtual bool fill(const Buffer &content);
    virtual bool fillF32(const std::vector<float> &content, const std::vector<std::string> &names, const std::vector<int32_t> &dimensions);
    int32_t getComponentsCount() const { return((int32_t)components_.size()); }
    void prepare_for_use(const CglEsProgram *program) const;
};

class CIndexBuffer : public IIndexBuffer {
    GLuint buffer_;
public:
    CIndexBuffer(GLuint buf) { buffer_ = buf; }
    virtual ~CIndexBuffer() { if (buffer_ != 0) glDeleteBuffers(1, &buffer_); }
    virtual void *get__id() const { return(nullptr); }
    virtual bool fill(const std::vector<uint16_t> &content);
    void prepare_for_use(void) const;
};

class CglEsProgram : public IglEsProgram {
    GLuint vertex_;
    GLuint fragment_;
    GLuint program_;
public:
    CglEsProgram(GLuint vertex, GLuint fragment, GLuint program);
    virtual ~CglEsProgram();
    virtual void *get__id() const { return(nullptr); }
    virtual int32_t getUniformLocation(const char *name) const;
    int32_t idx_from_name(const char *name) const;
    void prepare_for_use(GLint *enabled_attributes)const;
};

class CColorTarget : public IColorTarget {
    GLuint texture_;
    int32_t width_;
    int32_t height_;
public:
    CColorTarget(GLuint texture, GLuint ww, GLuint hh);
    virtual ~CColorTarget() { if (texture_ != 0) glDeleteTextures(1, &texture_); }
    virtual void *get__id() const { return(nullptr); }
    virtual int32_t getWidth() const { return(width_); }
    virtual int32_t getHeight() const { return(height_); }
    void attachToFb(void) const;
    void prepare_for_use(void) const;
};

class CDepthTarget : public IDepthTarget {
    GLuint depth_buffer_;
    GLuint stencil_buffer_;
public:
    CDepthTarget(GLuint depth_buffer, GLuint stencil_buffer);
    virtual ~CDepthTarget();
    virtual void *get__id() const { return(nullptr); }
    void attachToFb(void) const;
};

class CTexture : public ITexture {
    GLuint      texture_;
    int32_t     width_;
    int32_t     height_;
    TexUsage    usage_;
    bool        isP2_;
    bool        has_mips_;
    TexFilter   filter_;
    TexWrap     wrap_;

    void setFilterAndWrap(int32_t level, GLenum target);
public:
    CTexture(GLuint tex, int32_t width, int32_t height, TexFilter filtering, TexWrap wrap, TexUsage purpose);
    virtual ~CTexture() { if (texture_ != 0) glDeleteTextures(1, &texture_); }
    virtual void *get__id() const { return(nullptr); }
    virtual bool fill(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level, TexFormat format,
        int32_t cubeface, bool autoMipMaps);
    virtual bool fillCompressed(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level,
        int32_t cubeface);
    void prepare_for_use()const;
};

} // namespace
