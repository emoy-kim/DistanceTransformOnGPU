#pragma once

#include "_Common.h"

class CanvasGL final
{
public:
   CanvasGL();
   ~CanvasGL();

   [[nodiscard]] GLuint getCanvasID() const { return CanvasID; }
   [[nodiscard]] GLuint getColor0TextureID() const { return COLOR0TextureID; }
   [[nodiscard]] GLuint getColor1TextureID() const { return COLOR1TextureID; }
   void setCanvas(int width, int height, GLenum format, bool use_stencil = false);
   void setCanvasWithDoubleDrawBuffers(int width, int height, GLenum format, bool use_stencil = false);
   void setMultiSampledCanvas(int width, int height, int sample_num, GLenum format, bool use_stencil = false);
   void clearColor(int buffer_index = 0) const;
   void clearColor(const std::array<GLfloat, 4>& color, int buffer_index = 0) const;
   void clearStencil() const;
   void turnOnColorBuffers() const
   {
      constexpr GLenum draw_buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
      glNamedFramebufferDrawBuffers( CanvasID, 2, draw_buffers );
   }
   void turnOnMainColorBufferOnly() const
   {
      constexpr GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
      glNamedFramebufferDrawBuffers( CanvasID, 1, draw_buffers );
   }

private:
   GLuint CanvasID;
   GLuint COLOR0TextureID;
   GLuint COLOR1TextureID;
   GLuint StencilTextureID;

   void deleteAllTextures();
};