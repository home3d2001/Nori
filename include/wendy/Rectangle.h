///////////////////////////////////////////////////////////////////////
// Wendy core library
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
#ifndef WENDY_RECTANGLE_H
#define WENDY_RECTANGLE_H
///////////////////////////////////////////////////////////////////////

#include <stack>

///////////////////////////////////////////////////////////////////////

namespace wendy
{

///////////////////////////////////////////////////////////////////////

class Rect
{
public:
  Rect(void);
  Rect(const Vec2& position, const Vec2& size);
  Rect(float x, float y, float width, float height);
  bool contains(const Vec2& point) const;
  bool contains(const Rect& other) const;
  bool intersects(const Rect& other) const;
  bool clipBy(const Rect& other);
  void envelop(const Rect& other);
  void normalize(void);
  bool operator == (const Rect& other) const;
  bool operator != (const Rect& other) const;
  Rect operator + (const Vec2& offset) const;
  Rect operator - (const Vec2& offset) const;
  Rect operator * (const Vec2& scale) const;
  Rect& operator += (const Vec2& offset);
  Rect& operator -= (const Vec2& offset);
  Rect& operator *= (const Vec2& scale);
  Vec2 getCenter(void) const;
  void setCenter(const Vec2& newCenter);
  void getBounds(float& minX, float& minY, float& maxX, float& maxY) const;
  void setBounds(float minX, float minY, float maxX, float maxY);
  void set(const Vec2& newPosition, const Vec2& newSize);
  void set(float x, float y, float width, float height);
  Vec2 position;
  Vec2 size;
};

///////////////////////////////////////////////////////////////////////

class Recti
{
public:
  Recti(void);
  Recti(const Vec2i& position, const Vec2i& size);
  Recti(int x, int y, int width, int height);
  bool contains(const Vec2i& point) const;
  bool contains(const Recti& other) const;
  bool intersects(const Recti& other) const;
  bool clipBy(const Recti& other);
  void envelop(const Recti& other);
  void normalize(void);
  bool operator == (const Recti& other) const;
  bool operator != (const Recti& other) const;
  Recti operator + (const Vec2i& offset) const;
  Recti operator - (const Vec2i& offset) const;
  Recti operator * (const Vec2i& scale) const;
  Recti& operator += (const Vec2i& offset);
  Recti& operator -= (const Vec2i& offset);
  Recti& operator *= (const Vec2i& scale);
  Vec2i getCenter(void) const;
  void setCenter(const Vec2i& newCenter);
  void getBounds(int& minX, int& minY, int& maxX, int& maxY) const;
  void setBounds(int minX, int minY, int maxX, int maxY);
  void set(const Vec2i& newPosition, const Vec2i& newSize);
  void set(int x, int y, int width, int height);
  Vec2i position;
  Vec2i size;
};

///////////////////////////////////////////////////////////////////////

template<typename T>
class RectClipStack
{
public:
  inline RectClipStack(void);
  inline bool push(const T& rectangle);
  inline void pop(void);
  inline bool isEmpty(void) const;
  inline unsigned int getCount(void) const;
  inline const T& getTop(void) const;
  inline const T& getTotal(void) const;
private:
  struct Entry
  {
    T local;
    T total;
  };
  std::stack<Entry> entries;
};

///////////////////////////////////////////////////////////////////////

typedef RectClipStack<Rect> RectClipStackf;
typedef RectClipStack<Recti> RectClipStacki;

///////////////////////////////////////////////////////////////////////

template <typename T>
inline RectClipStack<T>::RectClipStack(void)
{
}

template <typename T>
inline bool RectClipStack<T>::push(const T& rectangle)
{
  Entry entry;
  entry.total = rectangle;
  entry.local = rectangle;

  if (!entries.empty())
  {
    if (!entry.total.clipBy(entries.top().total))
      return false;
  }

  entries.push(entry);
  return true;
}

template <typename T>
inline void RectClipStack<T>::pop(void)
{
  if (entries.empty())
    throw Exception("Cannot pop empty scissor area clip stack");

  entries.pop();
}

template <typename T>
inline bool RectClipStack<T>::isEmpty(void) const
{
  return entries.empty();
}

template <typename T>
inline unsigned int RectClipStack<T>::getCount(void) const
{
  return (unsigned int) entries.size();
}

template <typename T>
inline const T& RectClipStack<T>::getTop(void) const
{
  if (entries.empty())
    throw Exception("Rect clip stack is empty");

  return entries.top().local;
}

template <typename T>
inline const T& RectClipStack<T>::getTotal(void) const
{
  if (entries.empty())
    throw Exception("Rect clip stack is empty");

  return entries.top().total;
}

///////////////////////////////////////////////////////////////////////

} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_RECTANGLE_H*/
///////////////////////////////////////////////////////////////////////