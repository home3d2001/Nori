///////////////////////////////////////////////////////////////////////
// Wendy user interface library
// Copyright (c) 2007 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_UISCROLLER_HPP
#define WENDY_UISCROLLER_HPP
///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace UI
  {

///////////////////////////////////////////////////////////////////////

/*! @ingroup ui
 */
class Scroller : public Widget
{
public:
  Scroller(Layer& layer, Orientation orientation);
  Orientation getOrientation() const;
  float getMinValue() const;
  float getMaxValue() const;
  void setValueRange(float newMinValue, float newMaxValue);
  float getValue() const;
  void setValue(float newValue);
  float getPercentage() const;
  void setPercentage(float newPercentage);
  SignalProxy1<void, Scroller&> getValueChangedSignal();
protected:
  void draw() const;
private:
  void onMouseButton(Widget& widget,
                     vec2 position,
                     MouseButton button,
                     Action action,
                     uint mods);
  void onKey(Widget& widget, Key key, Action action, uint mods);
  void onScroll(Widget& widget, vec2 offset);
  void onDragBegun(Widget& widget, vec2 position);
  void onDragMoved(Widget& widget, vec2 position);
  void setValue(float newValue, bool notify);
  float getHandleSize() const;
  float getHandleOffset() const;
  float getValueStep() const;
  Signal1<void, Scroller&> valueChangedSignal;
  float minValue;
  float maxValue;
  float value;
  float percentage;
  float reference;
  Orientation orientation;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace UI*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_UISCROLLER_HPP*/
///////////////////////////////////////////////////////////////////////