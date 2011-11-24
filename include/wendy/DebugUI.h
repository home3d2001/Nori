///////////////////////////////////////////////////////////////////////
// Wendy debug interface
// Copyright (c) 2011 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_DEBUGUI_H
#define WENDY_DEBUGUI_H
///////////////////////////////////////////////////////////////////////

#include <wendy/UIDrawer.h>
#include <wendy/UILayer.h>
#include <wendy/UIWidget.h>
#include <wendy/UILabel.h>
#include <wendy/UILayout.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace debug
  {

///////////////////////////////////////////////////////////////////////

class Interface : public UI::Layer
{
public:
  Interface(input::Context& context, UI::Drawer& drawer);
  void update();
  void draw();
private:
  enum
  {
    LABEL_FRAMERATE,
    LABEL_PASSES,
    LABEL_VERTICES,
    LABEL_POINTS,
    LABEL_LINES,
    LABEL_TRIANGLES,
    LABEL_COUNT,
  };
  GL::Stats stats;
  UI::Widget* root;
  UI::Label* labels[LABEL_COUNT];
};

///////////////////////////////////////////////////////////////////////

  } /*namespace debug*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_DEBUGUI_H*/
///////////////////////////////////////////////////////////////////////
