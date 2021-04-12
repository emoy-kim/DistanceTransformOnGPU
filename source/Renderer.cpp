#include "Renderer.h"

RendererGL::RendererGL() : 
   Window( nullptr ), FrameWidth( 1920 ), FrameHeight( 1080 ), DistanceType( DISTANCE_TYPE::EUCLIDEAN ),
   MainCamera( std::make_unique<CameraGL>() ), ObjectShader( std::make_unique<ShaderGL>() ),
   FieldShader( std::make_unique<ShaderGL>() ), DistanceTransformShader( std::make_unique<DistanceTransformShaderGL>() ),
   ImageObject( std::make_unique<ObjectGL>() ), DistanceObject( std::make_unique<ObjectGL>() ),
   Canvas( std::make_unique<CanvasGL>() )
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   //glfwTerminate();
}

void RendererGL::printOpenGLInformation()
{
   std::cout << "****************************************************************\n";
   std::cout << " - GLFW version supported: " << glfwGetVersionString() << "\n";
   std::cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << "\n";
   std::cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << "\n";
   std::cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n";
   std::cout << "****************************************************************\n\n";
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      std::cout << "Cannot Initialize OpenGL...\n";
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   Window = glfwCreateWindow( FrameWidth, FrameHeight, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
   }
   
   registerCallbacks();

   glDisable( GL_DEPTH_TEST );

   MainCamera->updateWindowSize( FrameWidth, FrameHeight );
}

void RendererGL::error(int error, const char* description) const
{
   puts( description );
}

void RendererGL::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
}

void RendererGL::cleanup(GLFWwindow* window)
{
   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_1:
         DistanceType = DISTANCE_TYPE::EUCLIDEAN;
         break;
      case GLFW_KEY_2:
         DistanceType = DISTANCE_TYPE::MANHATTAN;
         break;
      case GLFW_KEY_3:
         DistanceType = DISTANCE_TYPE::CHESSBOARD;
         break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void RendererGL::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height) const
{
   MainCamera->updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void RendererGL::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void RendererGL::registerCallbacks() const
{
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setImageObject() const
{
   const std::vector<glm::vec3> vertices = {
      { 1.0f, 0.0f, 0.0f },
      { 1.0f, 1.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },

      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f }
   };
   const std::vector<glm::vec3> normals = {
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },

      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f }
   };
   const std::vector<glm::vec2> textures = {
      { 1.0f, 0.0f },
      { 1.0f, 1.0f },
      { 0.0f, 1.0f },

      { 1.0f, 0.0f },
      { 0.0f, 1.0f },
      { 0.0f, 0.0f }
   };
   ImageObject->setObject( GL_TRIANGLES, vertices, normals, textures, "../horse.png" );
}

void RendererGL::setDistanceObject() const
{
   const int data_size = FrameWidth * FrameHeight;
   DistanceObject->addShaderStorageBufferObject<int>( "inside_column_scanner", data_size );
   DistanceObject->addShaderStorageBufferObject<int>( "outside_column_scanner", data_size );
   DistanceObject->addShaderStorageBufferObject<int>( "inside_distance_field", data_size );
   DistanceObject->addShaderStorageBufferObject<int>( "outside_distance_field", data_size );

   Canvas->setCanvas( FrameWidth, FrameHeight, GL_RGBA8 );
}

void RendererGL::setShaders() const
{
   const std::string shader_directory_path = std::string(CMAKE_SOURCE_DIR) + "/shaders";
   ObjectShader->setShader(
      std::string(shader_directory_path + "/BasicPipeline.vert").c_str(),
      std::string(shader_directory_path + "/BasicPipeline.frag").c_str()
   );
   FieldShader->setShader(
      std::string(shader_directory_path + "/Field.vert").c_str(),
      std::string(shader_directory_path + "/Field.frag").c_str()
   );
   DistanceTransformShader->setComputeShaders(
      { std::string(shader_directory_path + "/DistanceTransform.comp").c_str() }
   );

   ObjectShader->setUniformLocations();
   FieldShader->setUniformLocations();
   DistanceTransformShader->setUniformLocations();
}

void RendererGL::drawImage()
{
   Canvas->clearColor();

   glUseProgram( ObjectShader->getShaderProgram() );
   const GLuint texture_id = ImageObject->getTextureID( 0 );
   const glm::ivec2 texture_size = ImageObject->getTextureSize( texture_id );
   const glm::mat4 to_world =
      glm::translate( glm::mat4(1.0f), glm::vec3((FrameWidth - texture_size.x) / 2, (FrameHeight - texture_size.y) / 2, 0.0f) ) *
      glm::scale( glm::mat4(1.0f), glm::vec3(texture_size, 1.0f) );
   ObjectShader->transferBasicTransformationUniforms( to_world, MainCamera.get(), true );

   glBindFramebuffer( GL_FRAMEBUFFER, Canvas->getCanvasID() );
   glBindTextureUnit( 0, ImageObject->getTextureID( 0 ) );
   glBindVertexArray( ImageObject->getVAO() );
   glDrawArrays( ImageObject->getDrawMode(), 0, ImageObject->getVertexNum() );
}

void RendererGL::drawDistanceField()
{
   glUseProgram( DistanceTransformShader->getComputeShaderProgram( 0 ) );
   glBindImageTexture( 0, Canvas->getColor0TextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8 );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, DistanceObject->getCustomBufferID( "inside_column_scanner" ) );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, DistanceObject->getCustomBufferID( "outside_column_scanner" ) );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 2, DistanceObject->getCustomBufferID( "inside_distance_field" ) );
   glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 3, DistanceObject->getCustomBufferID( "outside_distance_field" ) );
   DistanceTransformShader->uniform1i( "Phase", 1, 0 );
   glDispatchCompute( getGroupSize( FrameHeight ), 1, 1 );
   glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
   DistanceTransformShader->uniform1i( "Phase", 2, 0 );
   DistanceTransformShader->uniform1i( "DistanceType", static_cast<int>(DistanceType), 0 );
   glDispatchCompute( getGroupSize( FrameWidth ), 1, 1 );
   glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

   glUseProgram( FieldShader->getShaderProgram() );
   const glm::mat4 to_world = glm::scale( glm::mat4(1.0f), glm::vec3(FrameWidth, FrameHeight, 1.0f) );
   FieldShader->transferBasicTransformationUniforms( to_world, MainCamera.get(), true );
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   glBindTextureUnit( 0, Canvas->getColor0TextureID() );
   glBindVertexArray( ImageObject->getVAO() );
   glDrawArrays( ImageObject->getDrawMode(), 0, ImageObject->getVertexNum() );
}

void RendererGL::render()
{
   glViewport( 0, 0, FrameWidth, FrameHeight );
   glClearColor( 0.97f, 0.93f, 0.89f, 1.0f );
   glClear( OPENGL_COLOR_BUFFER_BIT );

   drawImage();
   drawDistanceField();

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setImageObject();
   setDistanceObject();
   setShaders();

   while (!glfwWindowShouldClose( Window )) {
      render();
      
      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}