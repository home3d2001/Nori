///////////////////////////////////////////////////////////////////////
// Wendy core library
// Copyright (c) 2005 Camilla Berglund <elmindreda@elmindreda.org>
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
#ifndef WENDY_RAY_H
#define WENDY_RAY_H
///////////////////////////////////////////////////////////////////////

namespace wendy
{

///////////////////////////////////////////////////////////////////////

/*! Simple 3D ray.
 */
class Ray3
{
public:
  /*! Default constructor.
   */
  Ray3(void);
  /*! Constructor.
   *  @param origin [in] The initial origin.
   *  @param direction [in] The initial direction.
   */
  Ray3(const Vec3& origin, const Vec3& direction);
  /*! Transforms this ray by the specified transform.
   *  @param transform [in] The transform to use.
   */
  void transformBy(const Transform3& transform);
  /*! Resets this ray to default values.
   *  The default ray is positioned at origo, and points along the positive z-axis.
   */
  void setDefaults(void);
  /*! Sets the values of this ray.
   *  @param newOrigin [in] The desired origin.
   *  @param newDirection [in] The desired direction.
   */
  void set(const Vec3& newOrigin, const Vec3& newDirection);
  /*! The origin of this ray.
   */
  Vec3 origin;
  /*! The direction of this ray.
   */
  Vec3 direction;
};

///////////////////////////////////////////////////////////////////////

} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_RAY_H*/
///////////////////////////////////////////////////////////////////////