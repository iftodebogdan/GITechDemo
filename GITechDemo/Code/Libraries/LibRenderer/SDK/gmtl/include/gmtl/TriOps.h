// GMTL is (C) Copyright 2001-2011 by Allen Bierbaum
// Distributed under the GNU Lesser General Public License 2.1 with an
// addendum covering inlined code. (See accompanying files LICENSE and
// LICENSE.addendum or http://www.gnu.org/copyleft/lesser.txt)

#ifndef _GMTL_TRIOPS_H_
#define _GMTL_TRIOPS_H_

#include <gmtl/Tri.h>
#include <gmtl/Generate.h>
#include <gmtl/VecOps.h>

namespace gmtl
{
/** @ingroup Ops Tri
 *  @name Triangle Operations
 * @{
 */

/**
 * Finds the closest point on the triangle to a given point.
 *
 * @param tri The triangle to test.
 * @param pt  The point which to test against triangle.
 *
 * @return  the point on the line segment closest to pt.
 *
 * @since 0.7.0
 */
template< class DATA_TYPE >
Point<DATA_TYPE, 3> findNearestPt(const Tri<DATA_TYPE>& tri,
                                  const Point<DATA_TYPE, 3>& pt)
{
   // inspired by Real Time Collision Detection 1st Edition p. 144 ff
   Vec<DATA_TYPE, 3> ab = tri.edge(0,1);
   Vec<DATA_TYPE, 3> ac = tri.edge(0,2);   
   Vec<DATA_TYPE, 3> ap = pt - tri.mVerts[0]; // p - a 

   // check if P in vertex region outside a
   DATA_TYPE d1 = dot(ab, ap);
   DATA_TYPE d2 = dot(ac, ap);
   if (d1 <= 0.0 && d2 <= 0.0)
   {
      return tri.mVerts[0];
   }

// check if P in vertex region outside b
   Vec<DATA_TYPE, 3> bp = pt - tri.mVerts[1]; // p -b
   DATA_TYPE d3 = dot(ab, bp);
   DATA_TYPE d4 = dot(ac, bp);

   if (d3 >= 0 && d4 <= d3)
   {
      return tri.mVerts[1];
   }

   // check if p in edge region of AB, if true return projection of P onto AB
   DATA_TYPE vc = d1*d4 - d3*d2;
   if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0)
   {
      DATA_TYPE v = d1 / (d1 - d3);
      return tri.mVerts[0] + v * ab;
   }

   // check if P in vertex region outside c
   Vec<DATA_TYPE, 3> cp = pt - tri.mVerts[2]; // p - c
   DATA_TYPE d5 = dot(ab, cp);
   DATA_TYPE d6 = dot(ac, cp);
   if (d6 >= 0.0 && d5 <= d6)
   {
      return tri.mVerts[2];
   }

   // check if p in edge region of AC, if true return projection of P onto AC
   DATA_TYPE vb = (d5 * d2) - (d1 * d6);
   if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0)
   {
      DATA_TYPE w = d2 / (d2 - d6);
      return tri.mVerts[0] + w * ac;
   }

   // check if p in edge region of BC, if true return projection of P onto BC
   DATA_TYPE va = (d3 * d6) - (d5 * d4);
   if (va <= 0.0 && (d4 -d3) >= 0.0 && (d5 - d6) >= 0.0)
   {
      DATA_TYPE w = (d4 -d3) / ((d4 - d3) + (d5 -d6));
      return tri.mVerts[1] + w * (tri.edge(1,2));
   }

   // P inside face region Compute Q trough barycentric coordinates
   DATA_TYPE denom = 1.0 / (va + vb + vc);
   DATA_TYPE v = vb * denom;
   DATA_TYPE w = vc * denom;
   return (tri.mVerts[0] + (ab * v) + (ac * w));
} 

/**
 * Computes the point at the center of the given triangle.
 *
 * @param tri     the triangle to find the center of
 *
 * @return  the point at the center of the triangle
 */
template< class DATA_TYPE >
Point<DATA_TYPE, 3> center( const Tri<DATA_TYPE>& tri )
{
   const float one_third = (1.0f/3.0f);
   return (tri[0] + tri[1] + tri[2]) * DATA_TYPE(one_third);
}

/**
 * Computes the normal for this triangle.
 *
 * @param tri     the triangle for which to compute the normal
 *
 * @return  the normal vector for tri
 */
template< class DATA_TYPE >
Vec<DATA_TYPE, 3> normal( const Tri<DATA_TYPE>& tri )
{
   Vec<DATA_TYPE, 3> normal = makeCross( gmtl::Vec<DATA_TYPE,3>(tri[1] - tri[0]), gmtl::Vec<DATA_TYPE,3>(tri[2] - tri[0]) );
   normalize( normal );
   return normal;
}
/** @} */

/** @ingroup Compare Tri
 *  @name Triangle Comparitors
 *  @{
 */

/**
 * Compare two triangles to see if they are EXACTLY the same.
 *
 * @param tri1    the first triangle to compare
 * @param tri2    the second triangle to compare
 *
 * @return  true if they are equal, false otherwise
 */
template< class DATA_TYPE >
bool operator==( const Tri<DATA_TYPE>& tri1, const Tri<DATA_TYPE>& tri2 )
{
   return ( (tri1[0] == tri2[0]) &&
            (tri1[1] == tri2[1]) &&
            (tri1[2] == tri2[2]) );
}

/**
 * Compare two triangle to see if they are not EXACTLY the same.
 *
 * @param tri1    the first triangle to compare
 * @param tri2    the second triangle to compare
 *
 * @return  true if they are not equal, false otherwise
 */
template< class DATA_TYPE >
bool operator!=( const Tri<DATA_TYPE>& tri1, const Tri<DATA_TYPE>& tri2 )
{
   return (! (tri1 == tri2));
}

/**
 * Compare two triangles to see if they are the same within the given tolerance.
 *
 * @param tri1    the first triangle to compare
 * @param tri2    the second triangle to compare
 * @param eps     the tolerance value to use
 *
 * @pre  eps must be >= 0
 *
 * @return  true if they are equal within the tolerance, false otherwise
 */
template< class DATA_TYPE >
bool isEqual( const Tri<DATA_TYPE>& tri1, const Tri<DATA_TYPE>& tri2,
              const DATA_TYPE& eps )
{
   gmtlASSERT( eps >= 0 );
   return ( isEqual(tri1[0], tri2[0], eps) &&
            isEqual(tri1[1], tri2[1], eps) &&
            isEqual(tri1[2], tri2[2], eps) );
}
/** @} */

} // namespace gmtl

#endif

