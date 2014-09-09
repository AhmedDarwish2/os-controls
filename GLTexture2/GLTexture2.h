#pragma once

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes
#include "GLTexture2Params.h"
#include "GLTexture2PixelData.h"

// This is an invaluable resource: http://www.opengl.org/wiki/Common_Mistakes

// This class wraps creation and use of 2-dimensional GL textures.  While the GLTexture2Params value
// used to construct GLTexture2 is persistent (it is stored almost unmodified), the pixel_data is
// not, and is only used to pass in data for texel-loading operations.
class GLTexture2 {
public:
  GLTexture2(const GLTexture2& rhs) = delete;

  // TODO: make GLTexture2-specific std::exception subclass?

  // Construct a GLTexture2 with the specified parameters and pixel data. The pixel data is only
  // passed into glTexImage2D, and is not stored.  The default value for pixel_data is "empty",
  // which indicates that while the texture memory will be allocated for it, it will not be
  // initialized.  An exception will be thrown upon error.
  GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data = GLTexture2PixelDataEmpty());
  // Automatically frees the allocated resources.
  ~GLTexture2 ();

  // Returns the assigned GLuint generated by this texture
  GLuint Id () { return m_texture_name; }
  // Returns the GLTexture2Params used to construct this texture.
  const GLTexture2Params &Params() const { return m_params; }

  // This method should be called to bind this shader.
  void Bind () { glBindTexture(m_params.Target(), m_texture_name); }
  // This method should be called when no shader program should be used.
  void Unbind () { glBindTexture(m_params.Target(), 0); }

  // DEPRECATED old version of UpdateTexture -- DO NOT USE IN NEW CODE
  void UpdateTexture(const void *data);
  // Updates the contents of this texture from the specified pixel data.
  void UpdateTexture(const GLTexture2PixelData &pixel_data);
  
private:

  void VerifyPixelDataOrThrow (const GLTexture2PixelData &pixel_data) const;

  GLenum m_format; // TODO: DELETE once deprecated version of UpdateTexture is deleted
  GLenum m_type;   // TODO: DELETE once deprecated version of UpdateTexture is deleted
  GLTexture2Params m_params;
  GLuint m_texture_name;
};
