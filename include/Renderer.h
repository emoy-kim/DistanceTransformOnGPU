#pragma once

#include "Canvas.h"
#include "Object.h"

class RendererGL
{
public:
   RendererGL(const RendererGL&) = delete;
   RendererGL(const RendererGL&&) = delete;
   RendererGL& operator=(const RendererGL&) = delete;
   RendererGL& operator=(const RendererGL&&) = delete;


   RendererGL();
   ~RendererGL();

   void play();

private:
   inline static RendererGL* Renderer = nullptr;
   GLFWwindow* Window;
   int FrameWidth;
   int FrameHeight;
   std::unique_ptr<CameraGL> MainCamera;
   std::unique_ptr<ShaderGL> ObjectShader;
   std::unique_ptr<ShaderGL> FieldShader;
   std::unique_ptr<DistanceTransformShaderGL> DistanceTransformShader;
   std::unique_ptr<ObjectGL> ImageObject;
   std::unique_ptr<ObjectGL> DistanceObject;
   std::unique_ptr<CanvasGL> Canvas;
 
   void registerCallbacks() const;
   void initialize();

   static void printOpenGLInformation();
   void error(int error, const char* description) const;
   void cleanup(GLFWwindow* window);
   void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
   void reshape(GLFWwindow* window, int width, int height) const;
   static void errorWrapper(int error, const char* description);
   static void cleanupWrapper(GLFWwindow* window);
   static void keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void reshapeWrapper(GLFWwindow* window, int width, int height);

   void setImageObject() const;
   void setDistanceObject() const;
   void setShaders() const;
   void drawImage();
   void drawDistanceField();
   void render();

   [[nodiscard]] static int getGroupSize(int size)
   {
      constexpr int ThreadGroupSize = 32;
      return (size + ThreadGroupSize - 1) / ThreadGroupSize;
   }
};