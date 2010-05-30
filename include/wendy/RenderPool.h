///////////////////////////////////////////////////////////////////////
// Wendy default renderer
// Copyright (c) 2010 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_RENDERPOOL_H
#define WENDY_RENDERPOOL_H
///////////////////////////////////////////////////////////////////////

#include <wendy/Core.h>
#include <wendy/Signal.h>

#include <wendy/GLImage.h>
#include <wendy/GLBuffer.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace render
  {

///////////////////////////////////////////////////////////////////////

/*! @brief Geometry pool.
 *  @ingroup render
 */
class GeometryPool : public Trackable, public Singleton<GeometryPool>
{
public:
  /*! Allocates a range of temporary indices of the specified type.
   *  @param[out] range The newly allocated index range.
   *  @param[in] count The number of indices to allocate.
   *  @param[in] type The type of indices to allocate.
   *  @return @c true if the allocation succeeded, or @c false if an
   *  error occurred.
   *  @remarks The allocated index range is only valid until the end of the
   *  current frame.
   */
  bool allocateIndices(GL::IndexRange& range,
		       unsigned int count,
                       GL::IndexBuffer::Type type);
  /*! Allocates a range of temporary vertices of the specified format.
   *  @param[out] range The newly allocated vertex range.
   *  @param[in] count The number of vertices to allocate.
   *  @param[in] format The format of vertices to allocate.
   *  @return @c true if the allocation succeeded, or @c false if an
   *  error occurred.
   *  @remarks The allocated vertex range is only valid until the end of the
   *  current frame.
   */
  bool allocateVertices(GL::VertexRange& range,
			unsigned int count,
                        const VertexFormat& format);
  /*! @return The context this renderer was created for.
   */
  GL::Context& getContext(void) const;
  /*! Creates the renderer singleton.
   *  @param[in] context The context to create the renderer for.
   *  @return @c true if successful, or @c false if an error occurred.
   */
  static bool create(GL::Context& context);
private:
  /*! @internal
   */
  struct IndexBufferSlot
  {
    Ref<GL::IndexBuffer> indexBuffer;
    unsigned int available;
  };
  /*! @internal
   */
  struct VertexBufferSlot
  {
    Ref<GL::VertexBuffer> vertexBuffer;
    unsigned int available;
  };
  GeometryPool(GL::Context& context);
  bool init(void);
  void onContextFinish(void);
  typedef std::list<IndexBufferSlot> IndexBufferList;
  typedef std::list<VertexBufferSlot> VertexBufferList;
  GL::Context& context;
  IndexBufferList indexBufferPool;
  VertexBufferList vertexBufferPool;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace render*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_RENDERPOOL_H*/
///////////////////////////////////////////////////////////////////////