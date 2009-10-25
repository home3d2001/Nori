///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2008 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_GLTEXTUREIO_H
#define WENDY_GLTEXTUREIO_H
///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////

class TextureCodec : public ResourceCodec<Texture>, public XML::Codec
{
public:
  TextureCodec(void);
  Texture* read(const Path& path, const String& name = "");
  Texture* read(Stream& stream, const String& name = "");
  bool write(const Path& path, const Texture& texture);
  bool write(Stream& stream, const Texture& texture);
private:
  bool onBeginElement(const String& name);
  bool onEndElement(const String& name);
  Ptr<Texture> texture;
  String textureName;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_GLTEXTUREIO_H*/
///////////////////////////////////////////////////////////////////////