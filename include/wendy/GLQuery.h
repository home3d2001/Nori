///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
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
#ifndef WENDY_GLQUERY_H
#define WENDY_GLQUERY_H
///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

class Context;

///////////////////////////////////////////////////////////////////////

class OcclusionQuery
{
public:
  ~OcclusionQuery(void);
  void begin(void);
  void end(void);
  bool isActive(void) const;
  bool hasResultAvailable(void) const;
  unsigned int getResult(void) const;
  static OcclusionQuery* create(Context& context);
private:
  OcclusionQuery(Context& context);
  bool init(void);
  Context& context;
  unsigned int queryID;
  bool active;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_GLQUERY_H*/
///////////////////////////////////////////////////////////////////////
