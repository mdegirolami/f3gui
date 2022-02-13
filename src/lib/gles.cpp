#include "gles_impl.h"

// Add to gles.h when recompiling from sing
/*
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
    
*/

#define glDepthRangef glDepthRange

namespace mmapp {


///////////////////
//
// Helper functions
//
///////////////////

enum class ShaderType { vertex, fragment };

static GLuint createAShader(const char *source, ShaderType stype, std::string *errordesc)
{
    GLint   bShaderCompiled;

    // create the shader
    GLuint shader;
    if (stype == ShaderType::vertex) {
        shader = glCreateShader(GL_VERTEX_SHADER);
    } else {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    }
    if (shader == 0) return(0);

    // Load the source code into the shader.
    GLint len = (GLint)strlen(source);
    glShaderSource(shader, 1, (const GLchar**)&source, &len);

    // Compile the source code.
    glCompileShader(shader);

    // Test if compilation succeeded.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &bShaderCompiled);
    if (!bShaderCompiled) {
        int     i32InfoLogLength, i32CharsWritten;
        char    *pszInfoLog;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
        pszInfoLog = new char[i32InfoLogLength + 1];
        glGetShaderInfoLog(shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
        pszInfoLog[i32InfoLogLength] = 0;
        *errordesc = pszInfoLog;
        delete[] pszInfoLog;

        // return failure
        glDeleteShader(shader);
        return(0);
    }
    return(shader);
}

static GLuint createAProgram(GLuint vs, GLuint fs)
{
    // creation
    GLuint program = glCreateProgram();
    if (program == 0) return(0);

    // attach the shaders and link
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    if (glGetError() != GL_NO_ERROR) {
        glDeleteProgram(program);
        return(0);
    }
    return(program);
}

static GLenum compareFunc2Gl(CompareFunc func)
{
    switch (func) {
    case CompareFunc::never:    return(GL_NEVER);
    case CompareFunc::always:   return(GL_ALWAYS);
    case CompareFunc::less:     return(GL_LESS);
    case CompareFunc::lequal:   return(GL_LEQUAL);
    case CompareFunc::equal:    return(GL_EQUAL);
    case CompareFunc::greater:  return(GL_GREATER);
    case CompareFunc::gequal:   return(GL_GEQUAL);
    case CompareFunc::notequal: return(GL_NOTEQUAL);
    }
    return(GL_NEVER);
}

static GLenum faceOrientation2Gl(FaceOrientation facing)
{
    switch (facing) {
    case FaceOrientation::front:    return(GL_FRONT);
    case FaceOrientation::back:     return(GL_BACK);
    }
    return(GL_FRONT_AND_BACK);   // both
}

static GLenum stencilOp2Gl(StencilOp op)
{
    switch (op) {
    case StencilOp::keep:    return(GL_KEEP);
    case StencilOp::zero:    return(GL_ZERO);
    case StencilOp::replace:    return(GL_REPLACE);
    case StencilOp::inc:    return(GL_INCR);
    case StencilOp::dec:    return(GL_DECR);
    case StencilOp::invert:    return(GL_INVERT);
    case StencilOp::inc_wrap:     return(GL_INCR_WRAP);
    }
    return(GL_DECR_WRAP);   //dec_wrap
}

static GLenum blendOp2Gl(BlendOp op)
{
    switch (op) {
    case BlendOp::add: return(GL_FUNC_ADD);
    case BlendOp::src_minus_dst: return(GL_FUNC_SUBTRACT);
    }
    return(GL_FUNC_REVERSE_SUBTRACT);   //dst_minus_src
}

static GLenum blendWeight2Gl(BlendWeight weight)
{
    switch (weight) {
    case BlendWeight::zero: return(GL_ZERO);
    case BlendWeight::one: return(GL_ONE);
    case BlendWeight::src_color: return(GL_SRC_COLOR);
    case BlendWeight::one_minus_src_color: return(GL_ONE_MINUS_SRC_COLOR);
    case BlendWeight::dst_color: return(GL_DST_COLOR);
    case BlendWeight::one_minus_dst_color: return(GL_ONE_MINUS_DST_COLOR);
    case BlendWeight::src_alpha: return(GL_SRC_ALPHA);
    case BlendWeight::one_minus_src_alpha: return(GL_ONE_MINUS_SRC_ALPHA);
    case BlendWeight::dst_alpha: return(GL_DST_ALPHA);
    case BlendWeight::one_minus_dst_alpha: return(GL_ONE_MINUS_DST_ALPHA);
    case BlendWeight::constant_color: return(GL_CONSTANT_COLOR);
    case BlendWeight::one_minus_constant_color: return(GL_ONE_MINUS_CONSTANT_COLOR);
    case BlendWeight::constant_alpha: return(GL_CONSTANT_ALPHA);
    case BlendWeight::one_minus_constant_alpha: return(GL_ONE_MINUS_CONSTANT_ALPHA);
    }
    return(GL_SRC_ALPHA_SATURATE);   //src_alpha_saturate
}

static GLenum featureId2Gl(FeatureId id)
{
    switch (id) {
    case FeatureId::cull: return(GL_CULL_FACE);
    case FeatureId::poly_offset: return(GL_POLYGON_OFFSET_FILL);
    case FeatureId::scissor: return(GL_SCISSOR_TEST);
    case FeatureId::depth_test: return(GL_DEPTH_TEST);
    case FeatureId::stencil_test: return(GL_STENCIL_TEST);
    case FeatureId::blend: return(GL_BLEND);
    }
    return(GL_BLEND);
}

static GLenum drawPrimitive2Gl(DrawPrimitive dp)
{
    switch (dp) {
    case DrawPrimitive::points: return(GL_POINTS);
    case DrawPrimitive::line_strip: return(GL_LINE_STRIP);
    case DrawPrimitive::line_loop: return(GL_LINE_LOOP);
    case DrawPrimitive::lines: return(GL_LINES);
    case DrawPrimitive::triangle_strip: return(GL_TRIANGLE_STRIP);
    case DrawPrimitive::triangle_fan: return(GL_TRIANGLE_FAN);
    }
    return(GL_TRIANGLES);
}

///////////////////
//
// mmapp::CGlEs object implementation
//
///////////////////

void CGlEs::init(int32_t width, int32_t height)
{
    framebuffer_ = 0;
    glGenFramebuffers(1, &framebuffer_);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&original_fb_);
#ifdef _IPAD
    GetCurrentColorbufferSize(&_original_width, &_original_height); // doesn't work on MAC
    _resolve_src = 0;
    glGenFramebuffers(1, &_resolve_src);
    _resolve_dst = 0;
    glGenFramebuffers(1, &_resolve_dst);
#endif

    // SELECT DEPTH BUFFER FORMAT
#if defined _IPAD

    // ipad always supports the interleaved stencil option
    _depth_format = GL_DEPTH24_STENCIL8_OES;
    _interleaved_stencil = true;
#elif defined _ANDROID

    // check for the interleaved stencil option
    char *extensions = (char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "OES_packed_depth_stencil") != NULL) {
        _depth_format = GL_DEPTH24_STENCIL8_OES;
        _interleaved_stencil = true;
    } else {
        _depth_format = GL_DEPTH_COMPONENT16;
        _interleaved_stencil = false;
    }
#else

    // other targets have full opengl - interleaved stencil is part of standard specifications
    depth_format_ = GL_DEPTH24_STENCIL8;
    interleaved_stencil_ = true;
#endif
    using_offline_target_ = false;
    bkbuf_width_ = width;
    bkbuf_height_ = height;
    buffers_changed_ = true;
    enabled_attributes_ = 0;
    error_ = GlError::no_error;
}

void CGlEs::update_bkbuf_size(int32_t width, int32_t height)
{
    bkbuf_width_ = width;
    bkbuf_height_ = height;
}

CGlEs::~CGlEs()
{
    curr_color_ = nullptr;
    curr_depth_ = nullptr;
    curr_bufs_.clear();
    curr_cpu_vertices_.clear();
    curr_indices_ = nullptr;
    curr_program_ = nullptr;
    if (framebuffer_ != 0) glDeleteFramebuffers(1, &framebuffer_);
    for (auto &entry : curr_textures_) entry = nullptr;
    for (auto &entry : curr_target_textures_) entry = nullptr;
}

// nfo
GlError CGlEs::getError()
{
    GlError local = error_;
    error_ = GlError::no_error;
    switch (glGetError()) {
    case GL_NO_ERROR: return(local);
    case GL_INVALID_ENUM: return(GlError::invalid_enum);
    case GL_INVALID_VALUE: return(GlError::invalid_value);
    case GL_INVALID_OPERATION: return(GlError::invalid_operation);
    case GL_OUT_OF_MEMORY: return(GlError::out_of_memory);
    }
    return(GlError::other);
}

std::string CGlEs::getString(StringInfo info_type) const
{
    GLenum name;

    switch (info_type) {
    case StringInfo::vendor: name = GL_VENDOR; break;
    case StringInfo::renderer: name = GL_RENDERER; break;
    case StringInfo::version: name = GL_VERSION; break;
    case StringInfo::sl_version: name = GL_SHADING_LANGUAGE_VERSION; break;
    case StringInfo::extensions: name = GL_EXTENSIONS; break;
    default: return("");
    }
    return((char*)glGetString(name));
}

// resources
std::shared_ptr<IVertexBuffer> CGlEs::createVertexBuffer(const Buffer &content)
{
    GLuint buf;

    glGenBuffers(1, &buf);
    if (buf == 0) return(nullptr);
    std::shared_ptr<CVertexBuffer> obj = std::make_shared<CVertexBuffer>(buf);
    if (!obj->fill(content)) {
        setError(GlError::invalid_value);
    }
    return(obj);
}

std::shared_ptr<IVertexBuffer> CGlEs::createVertexBufferF32(const std::vector<float> &content, const std::vector<std::string> &names,
    const std::vector<int32_t> &dimensions)
{
    GLuint buf;

    glGenBuffers(1, &buf);
    if (buf == 0) return(nullptr);
    std::shared_ptr<CVertexBuffer> obj = std::make_shared<CVertexBuffer>(buf);
    if (!obj->fillF32(content, names, dimensions)) {
        setError(GlError::invalid_value);
    }
    return(obj);
}

std::shared_ptr<IIndexBuffer> CGlEs::createIndexBuffer(const std::vector<uint16_t> &content)
{
    GLuint buf;

    glGenBuffers(1, &buf);
    if (buf == 0) return(nullptr);
    std::shared_ptr<CIndexBuffer> obj = std::make_shared<CIndexBuffer>(buf);
    obj->fill(content);
    return(obj);
}

std::shared_ptr<ITexture> CGlEs::createTexture(int32_t width, int32_t height, TexFilter filtering, TexWrap wrap_mode, TexUsage purpose)
{
    GLuint tex;

    if (width < 1 || height < 1 || purpose == TexUsage::cube && (!isPowerOfTwo(width) || !isPowerOfTwo(height))) {
        setError(GlError::invalid_value);
        return(nullptr);
    }
    glGenTextures(1, &tex);
    if (tex == 0) return(nullptr);
    return(std::make_shared<CTexture>(tex, width, height, filtering, wrap_mode, purpose));
}

std::shared_ptr<IglEsProgram> CGlEs::createProgram(const char *vs_source, const char *fs_source, std::string *errordesc)
{
    // compile
    *errordesc = "";
    GLuint vs = createAShader(vs_source, ShaderType::vertex, errordesc);
    if (vs == 0) return(nullptr);
    GLuint fs = createAShader(fs_source, ShaderType::fragment, errordesc);
    if (fs == 0) {
        glDeleteShader(vs);
        return(nullptr);
    }

    // link
    GlError old = getError();           // preserve and reset glError
    GLuint pp = createAProgram(vs, fs);
    if (pp == 0) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        *errordesc = "link error !!";
        setError(GlError::other);
        return(nullptr);
    }
    setError(old);                      // restore the error

    return(std::make_shared<CglEsProgram>(vs, fs, pp));
}

std::shared_ptr<IColorTarget> CGlEs::createColorTarget(int32_t width, int32_t height)
{
    // create
    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (texture == 0) return(nullptr);

    // load
    GlError old = getError();               // preserve and reset glError
    glActiveTexture(GL_TEXTURE0);           // dont modify the bindings of stages != 0 
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (glGetError() != GL_NO_ERROR) {
        glDeleteTextures(1, &texture);
        setError(GlError::other);        
        return(nullptr);
    }
    setError(old);                      // restore the error

    return(std::make_shared<CColorTarget>(texture, width, height));
}

std::shared_ptr<IDepthTarget> CGlEs::createDepthTarget(int32_t width, int32_t height)
{
    // create
    GLuint depth_buffer = 0;
    GLuint stencil_buffer = 0;
    glGenRenderbuffers(1, &depth_buffer);
    if (depth_buffer == 0) return(nullptr);

    // preserve and reset glError
    GlError old = getError();               

    // load
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, depth_format_, width, height);
    if (glGetError() != GL_NO_ERROR) {
        goto err_exit;
    }

    if (!interleaved_stencil_) {

        // must create separate stencil
        glGenRenderbuffers(1, &stencil_buffer);
        if (stencil_buffer == 0) goto err_exit;

        // load
        glBindRenderbuffer(GL_RENDERBUFFER, stencil_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);

        if (glGetError() != GL_NO_ERROR) {
            goto err_exit;
        }
    }

    // restore the error
    setError(old);                      

    return(std::make_shared<CDepthTarget>(depth_buffer, stencil_buffer));

err_exit:
    if (depth_buffer != 0)glDeleteRenderbuffers(1, &depth_buffer);
    if (stencil_buffer != 0)glDeleteRenderbuffers(1, &stencil_buffer);
    setError(GlError::other);        
    return(nullptr);
}

// inputs
void CGlEs::uniform1f(int32_t index, float value)
{
    glUniform1f(index, value);
}

void CGlEs::uniform2f(int32_t index, float v0, float v1)
{
    glUniform2f(index, v0, v1);
}

void CGlEs::uniform3f(int32_t index, float v0, float v1, float v2)
{
    glUniform3f(index, v0, v1, v2);
}

void CGlEs::uniform4f(int32_t index, float v0, float v1, float v2, float v3)
{
    glUniform4f(index, v0, v1, v2, v3);
}

void CGlEs::uniform1i(int32_t index, int32_t value)
{
    glUniform1i(index, value);
}

void CGlEs::uniform2i(int32_t index, int32_t v0, int32_t v1)
{
    glUniform2i(index, v0, v1);
}

void CGlEs::uniform3i(int32_t index, int32_t v0, int32_t v1, int32_t v2)
{
    glUniform3i(index, v0, v1, v2);
}

void CGlEs::uniform4i(int32_t index, int32_t v0, int32_t v1, int32_t v2, int32_t v3)
{
    glUniform4i(index, v0, v1, v2, v3);
}

void CGlEs::uniformMatrix2f(int32_t index, const sing::array<sing::array<float, 2>, 2> &values, bool transpose)
{
    glUniformMatrix2fv(index, 1, transpose, &values[0][0]);
}

void CGlEs::uniformMatrix3f(int32_t index, const sing::array<sing::array<float, 3>, 3> &values, bool transpose)
{
    glUniformMatrix3fv(index, 1, transpose, &values[0][0]);
}

void CGlEs::uniformMatrix4f(int32_t index, const sing::array<sing::array<float, 4>, 4> &values, bool transpose)
{
    glUniformMatrix4fv(index, 1, transpose, &values[0][0]);
}

void CGlEs::setTextureIndex(const char *uniform_name, int32_t stage)
{
    if (curr_program_ != nullptr) {
        int32_t index = curr_program_->getUniformLocation(uniform_name);
        if (index != -1) {
            glUniform1i(index, stage);
        }
    }
}

void CGlEs::useVbuffer(std::shared_ptr<const IVertexBuffer> vb)
{
    curr_bufs_.clear();
    useAdditionalVbuffer(vb);
}

void CGlEs::useAdditionalVbuffer(std::shared_ptr<const IVertexBuffer> vb)
{
    curr_cpu_vertices_.clear();
    std::shared_ptr<const CVertexBuffer> buf = std::dynamic_pointer_cast<const CVertexBuffer>(vb);
    if (buf == nullptr) return;
    curr_bufs_.push_back(buf);
    buffers_changed_ = true;
}

void CGlEs::useCpuVertices(std::shared_ptr<const Buffer> vertices)
{
    curr_cpu_vertices_.clear();
    useAdditionalCpuVertices(vertices);
}

void CGlEs::useAdditionalCpuVertices(std::shared_ptr<const Buffer> vertices)
{
    curr_bufs_.clear();
    if (vertices == nullptr) return;
    curr_cpu_vertices_.push_back(vertices);
    buffers_changed_ = true;
}

void CGlEs::useIbuffer(std::shared_ptr<const IIndexBuffer> ib)
{
    curr_indices_ = std::dynamic_pointer_cast<const CIndexBuffer>(ib);
}

void CGlEs::useProgram(std::shared_ptr<const IglEsProgram> prog)
{
    const IglEsProgram *oldp = &*curr_program_;
    curr_program_ = std::dynamic_pointer_cast<const CglEsProgram>(prog);
    if (oldp != &*curr_program_) {
        buffers_changed_ = true;
    }
    if (curr_program_ != nullptr) curr_program_->prepare_for_use(&enabled_attributes_);
}

void CGlEs::useTexture(std::shared_ptr<const ITexture> texture, int32_t stage, TexUsage purpose)
{
    if (stage < 0 || stage >= max_textures) return;
    curr_textures_[stage] = std::dynamic_pointer_cast<const CTexture>(texture);
    curr_target_textures_[stage] = nullptr;
    if (stage > 0) {
        glActiveTexture(stage);
        if (curr_textures_[stage] != nullptr) {
            (*curr_textures_[stage]).prepare_for_use();
        } else {
            glBindTexture(purpose == TexUsage::cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
        }
    }
}

void CGlEs::useColorTargetAsTexture(std::shared_ptr<const IColorTarget> texture, int32_t stage)
{
    if (stage < 0 || stage >= max_textures) return;
    curr_textures_[stage] = nullptr;
    curr_target_textures_[stage] = std::dynamic_pointer_cast<const CColorTarget>(texture);
    if (stage > 0) {
        glActiveTexture(stage);
        if (curr_target_textures_[stage] != nullptr) {
            (*curr_target_textures_[stage]).prepare_for_use();
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void CGlEs::useBackBuffer()
{
    if (using_offline_target_) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);

        // restore the backbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, original_fb_);
#ifdef _IPAD
        GetCurrentColorbufferSize(&_original_width, &_original_height);
#endif      
        using_offline_target_ = false;
        glViewport(0, 0, bkbuf_width_, bkbuf_height_);
    }
}

void CGlEs::useOfflineTarget()
{
    // get desc and set texture as target buffer
    if (!using_offline_target_) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
        using_offline_target_ = true;
        if (curr_color_ != nullptr) curr_color_->attachToFb();
        if (curr_depth_ != nullptr) curr_depth_->attachToFb();
        glDepthRangef(0, 1);
    }
}

std::shared_ptr<const IColorTarget> CGlEs::selectOfflineColor(std::shared_ptr<const IColorTarget> target)
{
    if (curr_color_ == target) return(target);
    std::shared_ptr<const IColorTarget>   the_old = curr_color_;
    curr_color_ = std::dynamic_pointer_cast<const CColorTarget>(target);
    if (using_offline_target_) {
        if (curr_color_ != nullptr) curr_color_->attachToFb();
        glDepthRangef(0, 1);
    }
    return(the_old);
}

std::shared_ptr<const IDepthTarget> CGlEs::selectOfflineDepth(std::shared_ptr<const IDepthTarget> target)
{
    if (curr_depth_ == target) return(target);
    std::shared_ptr<const IDepthTarget>   the_old = curr_depth_;
    curr_depth_ = std::dynamic_pointer_cast<const CDepthTarget>(target);
    if (using_offline_target_) {
        if (curr_depth_ != nullptr) curr_depth_->attachToFb();
        glDepthRangef(0, 1);
    }
    return(the_old);
}

// options
void CGlEs::viewport(int32_t left_x, int32_t lower_y, int32_t width, int32_t height)
{
    glViewport(left_x, lower_y, width, height);
}

void CGlEs::scissor(int32_t left_x, int32_t lower_y, int32_t width, int32_t height)
{
    glScissor(left_x, lower_y, width, height);
}

void CGlEs::depthRangef(float min_z, float max_z)
{
    glDepthRangef(min_z, max_z);
}

void CGlEs::lineWidth(float width)
{
    glLineWidth(width);
}

void CGlEs::frontFace(FaceMode mode)
{
    glFrontFace(mode == FaceMode::ccw ? GL_CCW : GL_CW);
}

void CGlEs::cullFace(FaceOrientation mode)
{
    glCullFace(faceOrientation2Gl(mode));
}

void CGlEs::polygonOffset(float factor, float units)
{
    glPolygonOffset(factor, units);
}

void CGlEs::stencilFunc(CompareFunc func, int32_t ref, uint32_t mask)
{
    glStencilFunc(compareFunc2Gl(func), ref, mask);
}

void CGlEs::stencilFuncSeparate(FaceOrientation face, CompareFunc func, int32_t ref, uint32_t mask)
{
    glStencilFuncSeparate(faceOrientation2Gl(face), compareFunc2Gl(func), ref, mask);
}

void CGlEs::stencilOp(StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass)
{
    glStencilOp(stencilOp2Gl(on_stencil_fail), stencilOp2Gl(on_depth_fail), stencilOp2Gl(on_pass));
}

void CGlEs::stencilOpSeparate(FaceOrientation face, StencilOp on_stencil_fail, StencilOp on_depth_fail, StencilOp on_pass)
{
    glStencilOpSeparate(faceOrientation2Gl(face), stencilOp2Gl(on_stencil_fail), stencilOp2Gl(on_depth_fail), stencilOp2Gl(on_pass));
}

void CGlEs::depthFunc(CompareFunc func)
{
    glDepthFunc(compareFunc2Gl(func));
}

void CGlEs::blendEquation(BlendOp op)
{
    glBlendEquation(blendOp2Gl(op));
}

void CGlEs::blendEquationSeparate(BlendOp rgb, BlendOp alpha)
{
    glBlendEquationSeparate(blendOp2Gl(rgb), blendOp2Gl(alpha));
}

void CGlEs::blendFunc(BlendWeight src, BlendWeight dst)
{
    glBlendFunc(blendWeight2Gl(src), blendWeight2Gl(dst));
}

void CGlEs::blendFuncSeparate(BlendWeight rgb_src, BlendWeight rgb_dst, BlendWeight alpha_src, BlendWeight alpha_dst)
{
    glBlendFuncSeparate(blendWeight2Gl(rgb_src), blendWeight2Gl(rgb_dst), blendWeight2Gl(alpha_src), blendWeight2Gl(alpha_dst));
}

void CGlEs::blendColor(float red, float green, float blue, float alpha)
{
    glBlendColor(red, green, blue, alpha);
}

void CGlEs::colorMask(bool red, bool green, bool blue, bool alpha)
{
    glColorMask(red ? GL_TRUE : GL_FALSE, green ? GL_TRUE : GL_FALSE, blue ? GL_TRUE : GL_FALSE, alpha ? GL_TRUE : GL_FALSE);
}

void CGlEs::depthMask(bool write_enable)
{
    glDepthMask(write_enable ? GL_TRUE : GL_FALSE);
}

void CGlEs::stencilMask(uint32_t mask)
{
    glStencilMask(mask);
}

void CGlEs::stencilMaskSeparate(FaceOrientation face, uint32_t mask)
{
    glStencilMaskSeparate(faceOrientation2Gl(face), mask);
}

void CGlEs::enable(FeatureId feature)
{
    glEnable(featureId2Gl(feature));
}

void CGlEs::disable(FeatureId feature)
{
    glDisable(featureId2Gl(feature));
}


// options for clear
void CGlEs::clearColor(float red, float green, float blue, float alpha)
{
    glClearColor(red, green, blue, alpha);
}

void CGlEs::clearDepth(float depth)
{
    glClearDepth(depth);
}

void CGlEs::clearStencil(int32_t value)
{
    glClearStencil(value);
}


// commands
void CGlEs::readPixels(std::vector<uint8_t> *data, int32_t left_x, int32_t lower_y, int32_t width, int32_t height) const
{
    data->clear();
    data->resize(width * height * 4);
    glReadPixels(left_x, lower_y, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_UNSIGNED_BYTE, data->data());
}

void CGlEs::clear(bool color, bool depth, bool stencil)
{
    GLbitfield mask = 0;
    if (color) mask |= GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

void CGlEs::flush()
{
    glFlush();
}

void CGlEs::finish()
{
    glFinish();
}

void CGlEs::draw(DrawPrimitive mode, int32_t first, int32_t count)
{
    if (!setupForDrawing()) {
        setError(GlError::invalid_operation);
        return;
    }
    glDrawArrays(drawPrimitive2Gl(mode), first, (GLsizei)count);
}

void CGlEs::drawElements(DrawPrimitive mode, int32_t first, int32_t count)
{
    if (!setupForDrawing() || curr_indices_ == nullptr) {
        setError(GlError::invalid_operation);
        return;
    }
    (*curr_indices_).prepare_for_use();
    glDrawElements(drawPrimitive2Gl(mode), (GLsizei)count, GL_UNSIGNED_SHORT, (void*)(uint64_t)first);
}

void CGlEs::drawCpuElements(DrawPrimitive mode, const std::vector<uint16_t> &indices)
{
    if (indices.size() < 1) return;
    if (!setupForDrawing()) {
        setError(GlError::invalid_operation);
        return;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(drawPrimitive2Gl(mode), (GLsizei)indices.size(), GL_UNSIGNED_SHORT, indices.data());
}

bool CGlEs::setupForDrawing(void)
{
    if (curr_program_ == nullptr || curr_bufs_.size() + curr_cpu_vertices_.size() < 1) {
        return(false);
    }
    if (buffers_changed_) {
        buffers_changed_ = false;
        if (curr_bufs_.size() > 0) {
            for (auto &buf : curr_bufs_) {
                (*buf).prepare_for_use(&*curr_program_);
            }
        } else {
            for (auto &buf : curr_cpu_vertices_) {

                // info from the Buffer
                const std::vector<ComponentDesc> *comp_vector = (*buf).getComponents();
                const std::vector<uint8_t> *data = (*buf).getContent();
                GLsizei stride = (GLsizei)(*buf).getVertexSize();

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                for (int ii = 0; ii < comp_vector->size(); ++ii) {
                    const ComponentDesc *comp = &(*comp_vector)[ii];
                    int32_t idx = (*curr_program_).idx_from_name(comp->name.c_str());
                    if (idx >= 0) {
                        glVertexAttribPointer(
                            idx,
                            comp->dimensions,
                            componentType2Gl(comp->ctype),
                            comp->normalized ? GL_TRUE : GL_FALSE,
                            stride,
                            (void*)(data->data() + comp->offset)
                        );
                    }
                }
            }
        }
    }

    // bindings on stage 0 can be dirty due to textures being bound to stage 0 for the purpose to manipulate them.
    glActiveTexture(0);
    if (curr_textures_[0] != nullptr) {
        (*curr_textures_[0]).prepare_for_use();
    } else if (curr_target_textures_[0] != nullptr) {
        (*curr_target_textures_[0]).prepare_for_use();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    return(true);
}

void CGlEs::setError(GlError error)
{
    if (error != GlError::no_error) {
        glGetError();          // reset internal gl error.
        error_ = error;
    }
}

} // namespace