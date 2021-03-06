///////////////////////////////////////////////////////////////////////
// Nori - a simple game engine
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

#pragma once

namespace nori
{

class Recti;

class Rect
{
public:
  Rect() { }
  Rect(vec2 position, vec2 size);
  Rect(float x, float y, float width, float height);
  explicit Rect(const Recti& source);
  bool contains(vec2 point) const;
  bool contains(const Rect& other) const;
  bool intersects(const Rect& other) const;
  bool clipBy(const Rect& other);
  void envelop(vec2 other);
  void envelop(const Rect& other);
  void normalize();
  bool operator == (const Rect& other) const;
  bool operator != (const Rect& other) const;
  vec2 center() const { return position + size / 2.f; }
  void setCenter(vec2 newCenter);
  void bounds(float& minX, float& minY, float& maxX, float& maxY) const;
  void setBounds(float minX, float minY, float maxX, float maxY);
  void set(vec2 newPosition, vec2 newSize);
  void set(float x, float y, float width, float height);
  vec2 position;
  vec2 size;
};

class Recti
{
public:
  Recti() { }
  Recti(ivec2 position, ivec2 size);
  Recti(int x, int y, int width, int height);
  explicit Recti(const Rect& source);
  bool contains(ivec2 point) const;
  bool contains(const Recti& other) const;
  bool intersects(const Recti& other) const;
  bool clipBy(const Recti& other);
  void envelop(const Recti& other);
  void normalize();
  bool operator == (const Recti& other) const;
  bool operator != (const Recti& other) const;
  ivec2 center() const { return position + size / 2; }
  void setCenter(ivec2 newCenter);
  void bounds(int& minX, int& minY, int& maxX, int& maxY) const;
  void setBounds(int minX, int minY, int maxX, int maxY);
  void set(ivec2 newPosition, ivec2 newSize);
  void set(int x, int y, int width, int height);
  ivec2 position;
  ivec2 size;
};

Rect rectCast(const std::string& string);
Recti rectiCast(const std::string& string);

std::string stringCast(const Rect& v);
std::string stringCast(const Recti& v);

template<typename T>
class RectClipStack
{
public:
  bool push(const T& rectangle);
  void pop();
  bool isEmpty() const;
  uint count() const;
  const T& top() const;
  const T& total() const;
private:
  struct Entry
  {
    T local;
    T total;
  };
  std::vector<Entry> entries;
};

typedef RectClipStack<Rect> RectClipStackf;
typedef RectClipStack<Recti> RectClipStacki;

template <typename T>
inline bool RectClipStack<T>::push(const T& rectangle)
{
  Entry entry;
  entry.total = rectangle;
  entry.local = rectangle;

  if (!entries.empty())
  {
    if (!entry.total.clipBy(entries.back().total))
      return false;
  }

  entries.push_back(entry);
  return true;
}

template <typename T>
inline void RectClipStack<T>::pop()
{
  assert(!entries.empty());
  entries.pop_back();
}

template <typename T>
inline bool RectClipStack<T>::isEmpty() const
{
  return entries.empty();
}

template <typename T>
inline uint RectClipStack<T>::count() const
{
  return (uint) entries.size();
}

template <typename T>
inline const T& RectClipStack<T>::top() const
{
  assert(!entries.empty());
  return entries.back().local;
}

template <typename T>
inline const T& RectClipStack<T>::total() const
{
  assert(!entries.empty());
  return entries.back().total;
}

} /*namespace nori*/

