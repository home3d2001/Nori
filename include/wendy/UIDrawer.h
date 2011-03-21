///////////////////////////////////////////////////////////////////////
// Wendy user interface library
// Copyright (c) 2006 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_UIDRAWER_H
#define WENDY_UIDRAWER_H
///////////////////////////////////////////////////////////////////////

#include <wendy/Core.h>
#include <wendy/Bezier.h>
#include <wendy/Segment.h>
#include <wendy/Triangle.h>

#include <wendy/OpenGL.h>
#include <wendy/GLTexture.h>
#include <wendy/GLBuffer.h>
#include <wendy/GLProgram.h>
#include <wendy/GLContext.h>
#include <wendy/GLState.h>

#include <wendy/RenderPool.h>
#include <wendy/RenderFont.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace UI
  {

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
enum WidgetState
{
  STATE_DISABLED,
  STATE_NORMAL,
  STATE_ACTIVE,
  STATE_SELECTED,
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
enum HorzAlignment
{
  LEFT_ALIGNED,
  RIGHT_ALIGNED,
  CENTERED_ON_X,
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
enum VertAlignment
{
  TOP_ALIGNED,
  BOTTOM_ALIGNED,
  CENTERED_ON_Y,
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
enum Orientation
{
  HORIZONTAL,
  VERTICAL,
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
class Alignment
{
public:
  Alignment(HorzAlignment horizontal = CENTERED_ON_X,
            VertAlignment vertical = CENTERED_ON_Y);
  void set(HorzAlignment newHorizontal, VertAlignment newVertical);
  HorzAlignment horizontal;
  VertAlignment vertical;
};

///////////////////////////////////////////////////////////////////////

/*! @brief Widget drawing singleton.
 *  @ingroup ui
 *
 *  This class implements default drawing behavior for widgets.
 *
 *  @remarks This should probable be made overridable at some point.
 */
class Drawer
{
public:
  void begin(void);
  void end(void);
  /*! Pushes a clipping area onto the clip stack. The current
   *  clipping area then becomes the specified area as clipped by the
   *  previously current clipping area.
   *  @param area The desired clipping area.
   *  @return @c true if successful, or @c false if the specified area
   *  would result in a null total clipping area.
   *  @remarks If the resulting clipping area is empty, it is not pushed
   *  onto the stack, so you do not need to (and should not) pop it. The
   *  recommended pattern is:
   *  @code
   *  if (drawer.pushClipArea(childArea))
   *  {
   *	drawStuff();
   *	drawer.popClipArea();
   *  }
   *  @endcode
   */
  bool pushClipArea(const Rect& area);
  /*! Pops the top clipping area from the clip stack, restoring the
   *  previously current clipping area.
   */
  void popClipArea(void);
  void drawPoint(const vec2& point, const vec4& color);
  void drawLine(const Segment2& segment, const vec4& color);
  void drawTriangle(const Triangle2& triangle, const vec4& color);
  void drawBezier(const BezierCurve2& spline, const vec4& color);
  void drawRectangle(const Rect& rectangle, const vec4& color);
  void fillRectangle(const Rect& rectangle, const vec4& color);
  void fillTriangle(const Triangle2& triangle, const vec4& color);
  void blitTexture(const Rect& area, GL::Texture& texture);
  void drawText(const Rect& area,
                const String& text,
		const Alignment& alignment,
		const vec3& color);
  void drawText(const Rect& area,
                const String& text,
		const Alignment& alignment = Alignment(),
		WidgetState state = STATE_NORMAL);
  void drawWell(const Rect& area, WidgetState state);
  void drawFrame(const Rect& area, WidgetState state);
  void drawHandle(const Rect& area, WidgetState state);
  void drawButton(const Rect& area, WidgetState state, const String& text = "");
  const vec3& getWidgetColor(void);
  void setWidgetColor(const vec3& newColor);
  const vec3& getTextColor(void);
  void setTextColor(const vec3& newColor);
  const vec3& getWellColor(void);
  void setWellColor(const vec3& newColor);
  const vec3& getSelectionColor(void);
  void setSelectionColor(const vec3& newColor);
  const vec3& getSelectedTextColor(void);
  void setSelectedTextColor(const vec3& newColor);
  render::Font& getDefaultFont(void);
  render::Font& getCurrentFont(void);
  void setCurrentFont(render::Font* newFont);
  float getDefaultEM(void) const;
  float getCurrentEM(void) const;
  render::GeometryPool& getGeometryPool(void) const;
  static Drawer* create(render::GeometryPool& pool);
private:
  Drawer(render::GeometryPool& pool);
  bool init(void);
  void setDrawingState(const vec4& color, bool wireframe);
  render::GeometryPool& pool;
  RectClipStackf clipAreaStack;
  vec3 widgetColor;
  vec3 textColor;
  vec3 wellColor;
  vec3 selectionColor;
  vec3 selectedTextColor;
  Ref<render::Font> defaultFont;
  Ref<render::Font> currentFont;
  GL::RenderState drawPass;
  GL::RenderState blitPass;
  Ref<GL::SharedProgramState> state;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace UI*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_UIDRAWER_H*/
///////////////////////////////////////////////////////////////////////