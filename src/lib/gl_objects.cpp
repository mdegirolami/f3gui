#include "gl_objects.h"

namespace mmapp {

//////////////////
//
// UTILITY FUNCTIONS
//
//////////////////
template<class T>
static bool setComponent(int32_t vertex_size, const std::vector<ComponentDesc> &components, std::vector<uint8_t> &content,
    const std::vector<T> &values, ComponentType ctype, int32_t component_idx, int32_t vertex_idx)
{
    if (vertex_size < 1 || component_idx < 0 || component_idx >= components.size()) return(false);
    const ComponentDesc *comp = &components[component_idx];
    if (comp->ctype != ctype) return(false);
    size_t num_vertices = content.size() / vertex_size - vertex_idx;
    size_t received_vertices = values.size() / comp->dimensions;
    if (num_vertices > received_vertices) num_vertices = received_vertices;
    if (num_vertices < 1) return(false);
    uint8_t *dst = &content[comp->offset + vertex_idx * vertex_size];
    int32_t dims = comp->dimensions;

    const T *src = values.data();
    for (int ii = 0; ii < num_vertices; ++ii) {
        T *typeddst = (T*)dst;
        for (int jj = 0; jj < dims; ++jj) {
            *typeddst++ = *src++;
        }
        dst += vertex_size;
    }
    return(true);
}


template<class T>
static bool getComponent(int32_t vertex_size, const std::vector<ComponentDesc> &components, const std::vector<uint8_t> &content,
    std::vector<T> &values, ComponentType ctype, int32_t component_idx, int32_t vertex_idx, int32_t count)
{
    if (vertex_size < 1 || component_idx < 0 || component_idx >= components.size()) return(false);
    const ComponentDesc *comp = &components[component_idx];
    if (comp->ctype != ctype) return(false);
    size_t num_vertices = content.size() / vertex_size - vertex_idx;
    if (count > 0 && num_vertices > count) num_vertices = count;
    if (num_vertices < 1) return(false);
    const uint8_t *src = &content[comp->offset + vertex_idx * vertex_size];
    int32_t dims = comp->dimensions;

    for (int ii = 0; ii < num_vertices; ++ii) {
        T *typedsrc = (T*)src;
        for (int jj = 0; jj < dims; ++jj) {
            values.push_back(*typedsrc++);
        }
        src += vertex_size;
    }
    return(true);
}

bool isPowerOfTwo(int32_t value)
{
    while ((value & 1) == 0) value >>= 1;
    return(value == 1);
}

static int32_t pixelSize(TexFormat format)
{
    switch (format) {
    case TexFormat::alpha:
    case TexFormat::luminance:
        return(1);
    case TexFormat::lumnance_alpha:
        return(2);
    case TexFormat::rgb:
        return(3);
    }
    return(4);
}

static bool texLevelSize(int32_t *ww, int32_t *hh, int32_t level, int32_t level0_w, int32_t level0_h)
{
    int32_t width = level0_w >> level;
    int32_t height = level0_h >> level;
    if (width == 0 && height == 0) return(false);
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    *ww = width;
    *hh = height;
    return(true);
}

static GLenum targetFromCubeface(int32_t cubeface)
{
    if (cubeface < 0) cubeface = 0;
    if (cubeface > 5) cubeface = 5;
    return(GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeface);
}

GLenum glpixelFromFormat(TexFormat format)
{
    switch (format) {
    case TexFormat::alpha: return(GL_ALPHA);
    case TexFormat::luminance: return(GL_LUMINANCE);
    case TexFormat::lumnance_alpha: return(GL_LUMINANCE_ALPHA);
    case TexFormat::rgb: return(GL_RGB);
    }
    return(GL_RGBA);
}

static GLenum glfilterFromEnum(TexFilter filter)
{
    switch (filter) {
    case TexFilter::nearest: return(GL_NEAREST);
    case TexFilter::bilinear: return(GL_LINEAR);
    }
    return(GL_LINEAR_MIPMAP_LINEAR);
}

static int32_t componentType2Size(ComponentType ctype)
{
    switch (ctype) {
    case ComponentType::float32:
        return(4);
    case ComponentType::signed16:
    case ComponentType::unsigned16:
        return(2);
    case ComponentType::signed8:
    case ComponentType::unsigned8:
        return(1);
    }
    return(4);
}

GLenum componentType2Gl(ComponentType ctype)
{
    switch (ctype) {
    case ComponentType::float32: return(GL_FLOAT);
    case ComponentType::signed16: return(GL_SHORT);
    case ComponentType::unsigned16: return(GL_UNSIGNED_SHORT);
    case ComponentType::signed8: return(GL_BYTE);
    case ComponentType::unsigned8: return(GL_UNSIGNED_BYTE);
    }
    return(4);
}

//////////////////
//
// Buffer
//
//////////////////
Buffer::Buffer()
{
    vertex_size_ = 0;
}

void Buffer::clear()
{
    components_.clear();
    content_.clear();
    vertex_size_ = 0;
}

bool Buffer::addComponent(const char *name, int32_t dimensions, ComponentType ctype, bool normalized)
{
    ComponentDesc comp;

    if (vertex_size_ != 0 || dimensions < 1 || dimensions > 4) return(false);
    comp.name = name;
    comp.dimensions = dimensions;
    comp.ctype = ctype;
    comp.normalized = normalized;
    components_.push_back(comp);
    return(true);
}

bool Buffer::getComponentInfo(int32_t idx, std::string *name, int32_t *dimensions, ComponentType *ctype, bool *normalized) const
{
    if (idx >= components_.size()) return(false);
    const ComponentDesc *comp = &components_[idx];
    *name = comp->name;
    *dimensions = comp->dimensions;
    *ctype = comp->ctype;
    *normalized = comp->normalized;
    return(true);
}

bool Buffer::resize(int32_t num_vertices)
{
    if (components_.size() == 0) return(false);

    // compute the vertex size
    int32_t offset = 0;
    int32_t max = 0;
    for (auto &comp : components_) {
        int32_t size = componentType2Size(comp.ctype);

        // align to size before using
        offset = (offset + size - 1) & ~(size - 1);
        comp.offset = offset;

        // increment by the field size
        offset += size * comp.dimensions;

        // for final alignment
        max = max > size ? max : size;
    }
    vertex_size_ = (offset + max - 1) & ~(max - 1);
    content_.clear();
    content_.resize(num_vertices * vertex_size_);
    return(true);
}

bool Buffer::setI8Component(const std::vector<int8_t> &values, int32_t component_idx, int32_t vertex_idx)
{
    return(setComponent(vertex_size_, components_, content_, values, ComponentType::signed8, component_idx, vertex_idx));
}

bool Buffer::setI16Component(const std::vector<int16_t> &values, int32_t component_idx, int32_t vertex_idx)
{
    return(setComponent(vertex_size_, components_, content_, values, ComponentType::signed16, component_idx, vertex_idx));
}

bool Buffer::setU8Component(const std::vector<uint8_t> &values, int32_t component_idx, int32_t vertex_idx)
{
    return(setComponent(vertex_size_, components_, content_, values, ComponentType::unsigned8, component_idx, vertex_idx));
}

bool Buffer::setU16Component(const std::vector<uint16_t> &values, int32_t component_idx, int32_t vertex_idx)
{
    return(setComponent(vertex_size_, components_, content_, values, ComponentType::unsigned16, component_idx, vertex_idx));
}

bool Buffer::setF32Component(const std::vector<float> &values, int32_t component_idx, int32_t vertex_idx)
{
    return(setComponent(vertex_size_, components_, content_, values, ComponentType::float32, component_idx, vertex_idx));
}

void Buffer::getI8Component(std::vector<int8_t> *values, int32_t component_idx, int32_t vertex_idx, int32_t count) const
{
    getComponent(vertex_size_, components_, content_, *values, ComponentType::signed8, component_idx, vertex_idx, count);
}

void Buffer::getI16Component(std::vector<int16_t> *values, int32_t component_idx, int32_t vertex_idx, int32_t count) const
{
    getComponent(vertex_size_, components_, content_, *values, ComponentType::signed16, component_idx, vertex_idx, count);
}

void Buffer::getU8Component(std::vector<uint8_t> *values, int32_t component_idx, int32_t vertex_idx, int32_t count) const
{
    getComponent(vertex_size_, components_, content_, *values, ComponentType::unsigned8, component_idx, vertex_idx, count);
}

void Buffer::getU16Component(std::vector<uint16_t> *values, int32_t component_idx, int32_t vertex_idx, int32_t count) const
{
    getComponent(vertex_size_, components_, content_, *values, ComponentType::unsigned16, component_idx, vertex_idx, count);
}

void Buffer::getF32Component(std::vector<float> *values, int32_t component_idx, int32_t vertex_idx, int32_t count) const
{
    getComponent(vertex_size_, components_, content_, *values, ComponentType::float32, component_idx, vertex_idx, count);
}

//////////////////
//
// RAII gl objects wrappers
//
//////////////////
bool CVertexBuffer::fill(const Buffer &content)
{
    const std::vector<uint8_t> *data = content.getContent();
    if (buffer_ == 0 || data->size() == 0) return(false);
    components_ = *content.getComponents();
    vertex_size_ = content.getVertexSize();
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);
    glBufferData(buffer_, data->size(), data->data(), GL_STATIC_DRAW);
    return(true);
}

bool CVertexBuffer::fillF32(const std::vector<float> &content, const std::vector<std::string> &names, const std::vector<int32_t> &dimensions)
{
    if (buffer_ == 0 || content.size() == 0 || dimensions.size() == 0 || dimensions.size() != names.size()) return(false);
    components_.clear();
    int32_t offset = 0;
    for (int32_t ii = 0; ii < names.size(); ++ii) {
        ComponentDesc comp;
        comp.name = names[ii];
        comp.ctype = ComponentType::float32;
        comp.dimensions = dimensions[ii];
        comp.normalized = false;
        comp.offset = offset;
        components_.push_back(comp);
        offset += dimensions[ii] << 2;
    }
    vertex_size_ = offset;
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);
    glBufferData(GL_ARRAY_BUFFER, content.size() << 2, content.data(), GL_STATIC_DRAW);
    return(true);
}

void CVertexBuffer::prepare_for_use(const CglEsProgram *program) const
{
    if (program == nullptr) return;
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);
    for (int ii = 0; ii < components_.size(); ++ii) {
        int32_t idx = program->idx_from_name(components_[ii].name.c_str());
        if (idx >= 0) {
            const ComponentDesc *comp = &components_[ii];
            glVertexAttribPointer(
                idx, 
                comp->dimensions, 
                componentType2Gl(comp->ctype), 
                comp->normalized ? GL_TRUE : GL_FALSE, 
                vertex_size_, 
                (void*)(uint64_t)comp->offset
            );
        }
    }
}

bool CIndexBuffer::fill(const std::vector<uint16_t> &content)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, content.size() << 1, content.data(), GL_STATIC_DRAW);
    return(true);
}

void CIndexBuffer::prepare_for_use(void) const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
}

CglEsProgram::~CglEsProgram()
{
    if (program_ != 0) glDeleteProgram(program_);
    if (fragment_ != 0) glDeleteShader(fragment_);
    if (vertex_ != 0) glDeleteShader(vertex_);
}

int32_t CglEsProgram::getUniformLocation(const char *name) const 
{ 
    GLint location = glGetUniformLocation(program_, name);
    return((int32_t)location);
}

CglEsProgram::CglEsProgram(GLuint vertex, GLuint fragment, GLuint program)
{
    vertex_ = vertex;
    fragment_ = fragment;
    program_ = program;
}

int32_t CglEsProgram::idx_from_name(const char *name) const
{
    GLint location = glGetAttribLocation(program_, name);
    return((int32_t)location);
}

void CglEsProgram::prepare_for_use(GLint *enabled_attributes) const
{
    glUseProgram(program_);
    GLint comp_num;
    glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &comp_num);
    if (comp_num < 0) comp_num = 0;
    if (comp_num > *enabled_attributes) {
        for (GLint ii = *enabled_attributes; ii < comp_num; ++ii) {
            glEnableVertexAttribArray((GLuint)ii);
        }
    } else if (comp_num < *enabled_attributes) {
        for (GLint ii = comp_num; ii < *enabled_attributes; ++ii) {
            glDisableVertexAttribArray((GLuint)ii);
        }
    }
    *enabled_attributes = comp_num;
}

CColorTarget::CColorTarget(GLuint texture, GLuint ww, GLuint hh) 
{
    texture_ = texture; 
    width_ = ww; 
    height_ = hh;
}

void CColorTarget::attachToFb(void) const
{ 
#ifdef _IPAD
    if (desc->msaa_buffer != 0 && !desc->resolved) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, desc->msaa_buffer);
    } else {
#endif            
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
#ifdef _IPAD
    }
#endif  
    glViewport(0, 0, width_, height_);
}

void CColorTarget::prepare_for_use() const
{
    glBindTexture(GL_TEXTURE_2D, texture_);
}
 
CDepthTarget::CDepthTarget(GLuint depth_buffer, GLuint stencil_buffer)
{
    depth_buffer_ = depth_buffer;
    stencil_buffer_ = stencil_buffer;
}

CDepthTarget::~CDepthTarget()
{
    if (depth_buffer_ != 0)glDeleteRenderbuffers(1, &depth_buffer_);
    if (stencil_buffer_ != 0)glDeleteRenderbuffers(1, &stencil_buffer_);
}

void CDepthTarget::attachToFb(void) const
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_);
    if (stencil_buffer_ == 0) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_);
    } else {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_buffer_);
    }
}

CTexture::CTexture(GLuint tex, int32_t width, int32_t height, TexFilter filtering, TexWrap wrap, TexUsage purpose)
{
    texture_ = tex;
    width_ = width;
    height_ = height;
    usage_ = purpose;
    isP2_ = isPowerOfTwo(width) && isPowerOfTwo(height);
    filter_ = filtering;
    wrap_ = wrap;
    has_mips_ = false;
}

bool CTexture::fill(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level, TexFormat format,
    int32_t cubeface, bool autoMipMaps)
{
    int32_t ww, hh;

    if (!texLevelSize(&ww, &hh, level, width_, height_)) return(false);
    if (width != ww || height != hh) return(false);
    if (level != 0 && !isP2_ || content.size() != pixelSize(format) * width * height) {
        return(false);
    }
    GLenum target = GL_TEXTURE_2D;
    GLenum load_target = GL_TEXTURE_2D;
    if (usage_ == TexUsage::cube) {
        target = GL_TEXTURE_CUBE_MAP;
        load_target = targetFromCubeface(cubeface);
    }
    GLenum glformat = glpixelFromFormat(format);
    glActiveTexture(GL_TEXTURE0);                   // dont modify the bindings of stages != 0 
    glBindTexture(target, texture_);
    glTexImage2D(load_target, level, glformat, width, height, 0, glformat, GL_UNSIGNED_BYTE, content.data());
    if (autoMipMaps && isP2_ && (usage_ == TexUsage::plain || cubeface == 5)) {
        glGenerateMipmap(target);
    }
    setFilterAndWrap(level, target);
    return(true);
}

bool CTexture::fillCompressed(const std::vector<uint8_t> &content, int32_t width, int32_t height, int32_t level, int32_t cubeface)
{
    int32_t ww, hh;

    if (!texLevelSize(&ww, &hh, level, width_, height_)) return(false);
    if (width != ww || height != hh) return(false);
    if (level != 0 && !isP2_) {
        return(false);
    }
    GLenum target = GL_TEXTURE_2D;
    GLenum load_target = GL_TEXTURE_2D;
    if (usage_ == TexUsage::cube) {
        target = GL_TEXTURE_CUBE_MAP;
        load_target = targetFromCubeface(cubeface);
    }
    glActiveTexture(GL_TEXTURE0);                   // dont modify the bindings of stages != 0 
    glBindTexture(target, texture_);
    glCompressedTexImage2D(load_target, level, GL_RGBA, width, height, 0, (GLsizei)content.size(), content.data());
    setFilterAndWrap(level, target);
    return(true);
}

void CTexture::prepare_for_use() const
{
    glBindTexture(usage_ == TexUsage::cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texture_);
}

void CTexture::setFilterAndWrap(int32_t level, GLenum target)
{
    if (level == 0) {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_ == TexWrap::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_ == TexWrap::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, glfilterFromEnum(filter_));
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter_ == TexFilter::nearest ? GL_NEAREST : GL_LINEAR);
    } else {
        has_mips_ = true;
    }
}

} // namespace