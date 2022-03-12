#ifndef __GLHEADERS_H
#define __GLHEADERS_H

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdint.h>

typedef uint64_t GLuint64;
typedef int64_t GLint64;

//#include <gl\GL.h>
// #define GL_VERSION_1_0 1    // exclude: already in windows
// #define GL_VERSION_1_1 1    // exclude: already in windows
#define GL_VERSION_3_2 1    // exclude above 3.1 (not supported by the context)
#define GL_VERSION_3_3 1

#define GL_VERSION_4_0 1
#define GL_VERSION_4_1 1
#define GL_VERSION_4_2 1
#define GL_VERSION_4_3 1
#define GL_VERSION_4_4 1
#define GL_VERSION_4_5 1

//#define GL_ARB_bindless_texture 1
#define GL_ARB_cl_event 1
//#define GL_ARB_gpu_shader_int64 1
//#define GL_NV_bindless_texture 1
//#define GL_INTEL_performance_query 1

#include "glcorearb.h"

const int GL_LUMINANCE = 0x1909;
const int GL_LUMINANCE_ALPHA = 0x190A;

#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE    // from glext.h

// GL 1.0
extern PFNGLGETERRORPROC glGetError;
extern PFNGLGETINTEGERVPROC glGetIntegerv;
extern PFNGLGETSTRINGPROC glGetString;
extern PFNGLTEXIMAGE2DPROC glTexImage2D;
extern PFNGLTEXPARAMETERIPROC glTexParameteri;
extern PFNGLVIEWPORTPROC glViewport;
extern PFNGLDEPTHRANGEPROC glDepthRange;
extern PFNGLSCISSORPROC glScissor;
extern PFNGLLINEWIDTHPROC glLineWidth;
extern PFNGLFRONTFACEPROC glFrontFace;
extern PFNGLCULLFACEPROC glCullFace;
extern PFNGLSTENCILFUNCPROC glStencilFunc;
extern PFNGLSTENCILOPPROC glStencilOp;
extern PFNGLCOLORMASKPROC glColorMask;
extern PFNGLDEPTHMASKPROC glDepthMask;
extern PFNGLSTENCILMASKPROC glStencilMask;
extern PFNGLENABLEPROC glEnable;
extern PFNGLDISABLEPROC glDisable;
extern PFNGLCLEARCOLORPROC glClearColor;
extern PFNGLCLEARDEPTHPROC glClearDepth;
extern PFNGLCLEARSTENCILPROC glClearStencil;
extern PFNGLREADPIXELSPROC glReadPixels;
extern PFNGLCLEARPROC glClear;
extern PFNGLFLUSHPROC glFlush;
extern PFNGLDEPTHFUNCPROC glDepthFunc;
extern PFNGLBLENDFUNCPROC glBlendFunc;
extern PFNGLFINISHPROC glFinish;

// GL 1.1
extern PFNGLDELETETEXTURESPROC glDeleteTextures;
extern PFNGLGENTEXTURESPROC glGenTextures;
extern PFNGLBINDTEXTUREPROC glBindTexture;
extern PFNGLPOLYGONOFFSETPROC glPolygonOffset;
extern PFNGLDRAWARRAYSPROC glDrawArrays;
extern PFNGLDRAWELEMENTSPROC glDrawElements;

// GL 1.2
extern PFNGLBLENDCOLORPROC glBlendColor;
extern PFNGLBLENDEQUATIONPROC glBlendEquation;
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
extern PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

// GL 1.3
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;

// GL 1.4
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
extern PFNGLPOINTPARAMETERIPROC glPointParameteri;
extern PFNGLPOINTPARAMETERIVPROC glPointParameteriv;

// GL 1.5
extern PFNGLGENQUERIESPROC glGenQueries;
extern PFNGLDELETEQUERIESPROC glDeleteQueries;
extern PFNGLISQUERYPROC glIsQuery;
extern PFNGLBEGINQUERYPROC glBeginQuery;
extern PFNGLENDQUERYPROC glEndQuery;
extern PFNGLGETQUERYIVPROC glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLISBUFFERPROC glIsBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
extern PFNGLMAPBUFFERPROC glMapBuffer;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;

// GL 2.0
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC glIsProgram;
extern PFNGLISSHADERPROC glIsShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2IPROC glUniform2i;
extern PFNGLUNIFORM3IPROC glUniform3i;
extern PFNGLUNIFORM4IPROC glUniform4i;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORM2IVPROC glUniform2iv;
extern PFNGLUNIFORM3IVPROC glUniform3iv;
extern PFNGLUNIFORM4IVPROC glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

// GL 2.1
extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

// GL 3.0
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;

// GL 3.1
extern PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

#endif