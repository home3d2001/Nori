///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2004 Camilla Berglund <elmindreda@home.se>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
///////////////////////////////////////////////////////////////////////

#include <moira/Config.h>
#include <moira/Core.h>
#include <moira/Point.h>
#include <moira/Log.h>

#include <wendy/Config.h>
#include <wendy/OpenGL.h>
#include <wendy/GLContext.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////
  
Key::Key(unsigned int initValue):
  value(initValue)
{
}

Key::operator unsigned int (void) const
{
  return value;
}

///////////////////////////////////////////////////////////////////////

ContextMode::ContextMode(void)
{
  setDefaults();
}

ContextMode::ContextMode(unsigned int initWidth,
                         unsigned int initHeight,
			 unsigned int initColorBits,
			 unsigned int initDepthBits,
			 unsigned int initStencilBits,
			 unsigned int initFlags):
  width(initWidth),
  height(initHeight),
  colorBits(initColorBits),
  depthBits(initDepthBits),
  stencilBits(initStencilBits),
  flags(initFlags)
{
}

void ContextMode::setDefaults(void)
{
  set(640, 480, 0, 0, 0, 0);
}

void ContextMode::set(unsigned int newWidth,
                      unsigned int newHeight,
		      unsigned int newColorBits,
		      unsigned int newDepthBits,
		      unsigned int newStencilBits,
		      unsigned int newFlags)
{
  width = newWidth;
  height = newHeight;
  colorBits = newColorBits;
  depthBits = newDepthBits;
  stencilBits = newStencilBits;
  flags = newFlags;
}
  
///////////////////////////////////////////////////////////////////////

void ContextObserver::onContextResize(unsigned int width, unsigned int height)
{
}

bool ContextObserver::onContextUpdate(void)
{
  return true;
}

bool ContextObserver::onContextClose(void)
{
  return true;
}

void ContextObserver::onContextKeyEvent(Key key, bool pressed)
{
}

void ContextObserver::onContextMouseMove(const Point2& position)
{
}

void ContextObserver::onContextMouseClick(unsigned int button, bool clicked)
{
}

///////////////////////////////////////////////////////////////////////

Context::~Context(void)
{
}
  
bool Context::create(const ContextMode& mode)
{
  return GLFWContext::create(mode);
}

///////////////////////////////////////////////////////////////////////

GLFWContext::~GLFWContext(void)
{
  glfwCloseWindow();

  instance = NULL;
}

bool GLFWContext::update(void)
{
  bool stopped = false;

  const ObserverList& observers = getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (!(*i)->onContextUpdate())
      stopped = true;
  }

  glfwSwapBuffers();

  if (stopped)
    return false;

  if (glfwGetWindowParam(GLFW_OPENED) != GL_TRUE)
    return false;

  return true;
}

EntryPoint GLFWContext::findEntryPoint(const std::string& name)
{
  return (EntryPoint) glfwGetProcAddress(name.c_str());
}

bool GLFWContext::isWindowed(void) const
{
  return (mode.flags & ContextMode::WINDOWED) != 0;
}

bool GLFWContext::isKeyDown(const Key& key) const
{
  int externalKey = key;

  KeyMap::const_iterator i = externalMap.find(externalKey);
  if (i != externalMap.end())
    externalKey = (*i).second;

  return (glfwGetKey(externalKey) == GLFW_PRESS) ? true : false;
}

bool GLFWContext::isButtonDown(unsigned int button) const
{
  return (glfwGetMouseButton(button + GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) ? true : false;
}

bool GLFWContext::hasExtension(const std::string& name) const
{
  return glfwExtensionSupported(name.c_str()) != 0;
}

unsigned int GLFWContext::getWidth(void) const
{
  return mode.width;
}

unsigned int GLFWContext::getHeight(void) const
{
  return mode.height;
}

unsigned int GLFWContext::getColorBits(void) const
{
  return mode.colorBits;
}

unsigned int GLFWContext::getDepthBits(void) const
{
  return mode.depthBits;
}

unsigned int GLFWContext::getStencilBits(void) const
{
  return mode.stencilBits;
}

const std::string& GLFWContext::getTitle(void) const
{
  return title;
}

void GLFWContext::setTitle(const std::string& newTitle)
{
  glfwSetWindowTitle(newTitle.c_str());
  title = newTitle;
}

const Point2& GLFWContext::getMousePosition(void) const
{
  static Point2 position;
  glfwGetMousePos(&position.x, &position.y);
  return position;
}

void GLFWContext::setMousePosition(const Point2& position)
{
  glfwSetMousePos(position.x, position.y);
}

bool GLFWContext::create(const ContextMode& mode)
{
  Ptr<GLFWContext> context = new GLFWContext();
  if (!context->init(mode))
    return false;
  
  set(context.detachObject());
  return true;
}

GLFWContext::GLFWContext(void)
{
  instance = this;
}

bool GLFWContext::init(const ContextMode& mode)
{
  if (!internalMap.size())
  {
    internalMap[Key::SPACE] = GLFW_KEY_SPACE;
    internalMap[Key::ESCAPE] = GLFW_KEY_ESC;
    internalMap[Key::TAB] = GLFW_KEY_TAB;
    internalMap[Key::ENTER] = GLFW_KEY_ENTER;
    internalMap[Key::BACKSPACE] = GLFW_KEY_BACKSPACE;
    internalMap[Key::INSERT] = GLFW_KEY_INSERT;
    internalMap[Key::DELETE] = GLFW_KEY_DEL;
    internalMap[Key::LSHIFT] = GLFW_KEY_LSHIFT;
    internalMap[Key::RSHIFT] = GLFW_KEY_RSHIFT;
    internalMap[Key::LCTRL] = GLFW_KEY_LCTRL;
    internalMap[Key::RCTRL] = GLFW_KEY_RCTRL;
    internalMap[Key::LALT] = GLFW_KEY_LALT;
    internalMap[Key::RALT] = GLFW_KEY_RALT;
    internalMap[Key::UP] = GLFW_KEY_UP;
    internalMap[Key::DOWN] = GLFW_KEY_DOWN;
    internalMap[Key::LEFT] = GLFW_KEY_LEFT;
    internalMap[Key::RIGHT] = GLFW_KEY_RIGHT;
    internalMap[Key::PAGEUP] = GLFW_KEY_PAGEUP;
    internalMap[Key::PAGEDOWN] = GLFW_KEY_PAGEDOWN;
    internalMap[Key::HOME] = GLFW_KEY_HOME;
    internalMap[Key::END] = GLFW_KEY_END;
    internalMap[Key::F1] = GLFW_KEY_F1;
    internalMap[Key::F2] = GLFW_KEY_F2;
    internalMap[Key::F3] = GLFW_KEY_F3;
    internalMap[Key::F4] = GLFW_KEY_F4;
    internalMap[Key::F5] = GLFW_KEY_F5;
    internalMap[Key::F6] = GLFW_KEY_F6;
    internalMap[Key::F7] = GLFW_KEY_F7;
    internalMap[Key::F8] = GLFW_KEY_F8;
    internalMap[Key::F9] = GLFW_KEY_F9;
    internalMap[Key::F10] = GLFW_KEY_F10;
    internalMap[Key::F11] = GLFW_KEY_F11;
    internalMap[Key::F12] = GLFW_KEY_F12;
  }

  if (!externalMap.size())
  {
    externalMap[GLFW_KEY_SPACE] = Key::SPACE;
    externalMap[GLFW_KEY_ESC] = Key::ESCAPE;
    externalMap[GLFW_KEY_TAB] = Key::TAB;
    externalMap[GLFW_KEY_ENTER] = Key::ENTER;
    externalMap[GLFW_KEY_BACKSPACE] = Key::BACKSPACE;
    externalMap[GLFW_KEY_INSERT] = Key::INSERT;
    externalMap[GLFW_KEY_DEL] = Key::DELETE;
    externalMap[GLFW_KEY_LSHIFT] = Key::LSHIFT;
    externalMap[GLFW_KEY_RSHIFT] = Key::RSHIFT;
    externalMap[GLFW_KEY_LCTRL] = Key::LCTRL;
    externalMap[GLFW_KEY_RCTRL] = Key::RCTRL;
    externalMap[GLFW_KEY_LALT] = Key::LALT;
    externalMap[GLFW_KEY_RALT] = Key::RALT;
    externalMap[GLFW_KEY_UP] = Key::UP;
    externalMap[GLFW_KEY_DOWN] = Key::DOWN;
    externalMap[GLFW_KEY_LEFT] = Key::LEFT;
    externalMap[GLFW_KEY_RIGHT] = Key::RIGHT;
    externalMap[GLFW_KEY_PAGEUP] = Key::PAGEUP;
    externalMap[GLFW_KEY_PAGEDOWN] = Key::PAGEDOWN;
    externalMap[GLFW_KEY_HOME] = Key::HOME;
    externalMap[GLFW_KEY_END] = Key::END;
    externalMap[GLFW_KEY_F1] = Key::F1;
    externalMap[GLFW_KEY_F2] = Key::F2;
    externalMap[GLFW_KEY_F3] = Key::F3;
    externalMap[GLFW_KEY_F4] = Key::F4;
    externalMap[GLFW_KEY_F5] = Key::F5;
    externalMap[GLFW_KEY_F6] = Key::F6;
    externalMap[GLFW_KEY_F7] = Key::F7;
    externalMap[GLFW_KEY_F8] = Key::F8;
    externalMap[GLFW_KEY_F9] = Key::F9;
    externalMap[GLFW_KEY_F10] = Key::F10;
    externalMap[GLFW_KEY_F11] = Key::F11;
    externalMap[GLFW_KEY_F12] = Key::F12;
  }

  unsigned int colorBits = mode.colorBits;
  if (colorBits > 24)
    colorBits = 24;
  
  unsigned int flags;
  
  if (mode.flags & ContextMode::WINDOWED)
    flags = GLFW_WINDOW;
  else
    flags = GLFW_FULLSCREEN;
  
  if (!glfwOpenWindow(mode.width, mode.height, 
                      colorBits / 3, colorBits / 3, colorBits / 3, 0,
                      mode.depthBits, mode.stencilBits, flags))
  {
    Log::writeError("Unable to create GLFW window");
    return false;
  }
  
  if (glewInit() != GLEW_OK)
  {
    Log::writeError("Unable to initialize GLEW");
    return false;
  }

  glfwSetWindowSizeCallback(sizeCallback);
  glfwSetWindowCloseCallback(closeCallback);
  glfwSetMousePosCallback(mousePosCallback);
  glfwSetMouseButtonCallback(mouseButtonCallback);
  glfwSetKeyCallback(keyboardCallback);
  
  glfwSwapInterval(1);

  setTitle("Wendy");
  glfwPollEvents();

  return true;
}

void GLFWCALL GLFWContext::sizeCallback(int width, int height)
{
  instance->mode.width = width;
  instance->mode.height = height;

  const ObserverList& observers = instance->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onContextResize(width, height);
}

int GLFWCALL GLFWContext::closeCallback(void)
{
  const ObserverList& observers = instance->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
  {
    if (!(*i)->onContextClose())
      return 0;
  }

  return 1;
}

void GLFWCALL GLFWContext::keyboardCallback(int key, int action)
{
  if (key > GLFW_KEY_SPECIAL)
  {
    KeyMap::const_iterator i = externalMap.find(key);
    if (i == externalMap.end())
      return;

    key = (*i).second;
  }

  const ObserverList& observers = instance->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onContextKeyEvent(key, (action == GLFW_PRESS) ? true : false);
}

void GLFWCALL GLFWContext::mousePosCallback(int x, int y)
{
  Point2 position(x, y);

  const ObserverList& observers = instance->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onContextMouseMove(position);
}

void GLFWCALL GLFWContext::mouseButtonCallback(int button, int action)
{
  const ObserverList& observers = instance->getObservers();
  for (ObserverList::const_iterator i = observers.begin();  i != observers.end();  i++)
    (*i)->onContextMouseClick(button - GLFW_MOUSE_BUTTON_1, (action == GLFW_PRESS) ? true : false);
}

GLFWContext::KeyMap GLFWContext::internalMap;
GLFWContext::KeyMap GLFWContext::externalMap;

GLFWContext* GLFWContext::instance = NULL;

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
