/**********************************************************************
*  Copyright (c) 2008-2013, Alliance for Sustainable Energy.  
*  All rights reserved.
*  
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*  
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*  
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#include <utilities/geometry/Geometry.hpp>
#include <utilities/geometry/Intersection.hpp>

#include <utilities/core/Assert.hpp>
#include <utilities/core/Logger.hpp>

#include <boost/foreach.hpp>

#undef BOOST_UBLAS_TYPE_CHECK
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/multi/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

typedef boost::geometry::model::d2::point_xy<double> BoostPoint;
typedef boost::geometry::model::polygon<BoostPoint> BoostPolygon;
typedef boost::geometry::model::ring<BoostPoint> BoostRing;
typedef boost::geometry::model::multi_polygon<BoostPolygon> BoostMultiPolygon;

namespace openstudio{

  // Private implementation functions

  // convert a Point3d to a BoostPoint
  boost::tuple<double, double> boostPointFromPoint3d(const Point3d& point3d, std::vector<Point3d>& allPoints, double tol)
  {
    OS_ASSERT(abs(point3d.z()) <= tol);

    // simple method
    //return boost::make_tuple(point3d.x(), point3d.y());

    // detailed method, try to combine points within tolerance
    BOOST_FOREACH(const Point3d& otherPoint, allPoints){
      if (std::sqrt(std::pow(point3d.x()-otherPoint.x(), 2) + std::pow(point3d.y()-otherPoint.y(), 2)) < tol){
        return boost::make_tuple(otherPoint.x(), otherPoint.y());
      }
    }
    allPoints.push_back(point3d);
    return boost::make_tuple(point3d.x(), point3d.y());
  }

  // convert vertices to a boost polygon, all vertices must lie on z = 0 plane
  boost::optional<BoostRing> boostPolygonFromVertices(const std::vector<Point3d>& vertices, std::vector<Point3d>& allPoints, double tol)
  {
    if (vertices.size () < 3){
      return boost::none;
    }

    BoostRing polygon;
    BOOST_FOREACH(const Point3d& vertex, vertices){

      // should all have zero z coordinate now
      double z = vertex.z();
      if (abs(z) > tol){
        LOG_FREE(Error, "utilities.geometry.boostPolygonFromVertices", "All points must be on z = 0 plane for intersection methods");
        return boost::none;
      }

      // use helper method which combines close points
      boost::geometry::append(polygon, boostPointFromPoint3d(vertex, allPoints, tol));
    }

    // close polygon, use helper method which combines close points
    boost::geometry::append(polygon, boostPointFromPoint3d(vertices[0], allPoints, tol));

    boost::geometry::correct(polygon);

    return polygon;
  }

  // convert a boost polygon to vertices
  std::vector<Point3d> verticesFromBoostPolygon(const BoostPolygon& polygon)
  {
    std::vector<Point3d> result;

    BoostRing outer = polygon.outer();
    if (outer.empty()){
      return result;
    }

    // add point for each vertex except final vertex
    for(unsigned i = 0; i < outer.size() - 1; ++i){
      result.push_back(Point3d(outer[i].x(), outer[i].y(), 0.0));
    }
    Point3d lastPoint = result.back();

    BOOST_FOREACH(const BoostRing& inner, polygon.inners()){
      if (!inner.empty()){
        // inner loop already in reverse order
        BOOST_FOREACH(const BoostPoint& p, inner){
          result.push_back(Point3d(p.x(), p.y(), 0.0));
        }
        result.push_back(lastPoint);
      }
    }

    result = removeColinear(result);

    return result;
  }

  // struct used to sort polygons in descending area by area
  struct BoostPolygonAreaGreater{
    bool operator()(const BoostPolygon& left, const BoostPolygon& right){
      boost::optional<double> leftA = getArea(verticesFromBoostPolygon(left));
      if (!leftA){
        leftA = 0;
      }
      boost::optional<double> rightA = getArea(verticesFromBoostPolygon(right));
      if (!rightA){
        rightA = 0;
      }
      return (*leftA > *rightA);
    }
  };

  // Public functions
  
  IntersectionResult::IntersectionResult(const std::vector<Point3d>& polygon1, 
                                         const std::vector<Point3d>& polygon2, 
                                         const std::vector< std::vector<Point3d> >& newPolygons1, 
                                         const std::vector< std::vector<Point3d> >& newPolygons2)
    : m_polygon1(polygon1), m_polygon2(polygon2), m_newPolygons1(newPolygons1), m_newPolygons2(newPolygons2)
  {
  }

  std::vector<Point3d> IntersectionResult::polygon1() const
  {
    return m_polygon1;
  }

  std::vector<Point3d> IntersectionResult::polygon2() const
  {
    return m_polygon2;
  }

  std::vector< std::vector<Point3d> > IntersectionResult::newPolygons1() const
  {
    return m_newPolygons1;
  }

  std::vector< std::vector<Point3d> > IntersectionResult::newPolygons2() const
  {
    return m_newPolygons2;
  }

  boost::optional<IntersectionResult> intersect(const std::vector<Point3d>& polygon1, const std::vector<Point3d>& polygon2, double tol)
  {
    std::vector<Point3d> resultPolygon1;
    std::vector<Point3d> resultPolygon2;
    std::vector< std::vector<Point3d> > newPolygons1;
    std::vector< std::vector<Point3d> > newPolygons2;

    // convert vertices to boost rings
    std::vector<Point3d> allPoints;
    
    boost::optional<BoostRing> boostPolygon1 = boostPolygonFromVertices(polygon1, allPoints, tol);
    if (!boostPolygon1){
      return boost::none;
    }
    // check if polygon overlaps itself
    try{
      boost::geometry::detail::overlay::has_self_intersections(*boostPolygon1);
    }catch(const boost::geometry::overlay_invalid_input_exception&){
      LOG_FREE(Error, "utilities.geometry.boostPolygonFromVertices", "Cannot intersect self intersecting polygon");
      return boost::none;
    }

    boost::optional<BoostRing> boostPolygon2 = boostPolygonFromVertices(polygon2, allPoints, tol);
    if (!boostPolygon2){
      return boost::none;
    }
    // check if polygon overlaps itself
    try{
      boost::geometry::detail::overlay::has_self_intersections(*boostPolygon2);
    }catch(const boost::geometry::overlay_invalid_input_exception&){
      LOG_FREE(Error, "utilities.geometry.boostPolygonFromVertices", "Cannot intersect self intersecting polygon");
      return boost::none;
    }

    // intersect the points in face coordinates, 
    std::vector<BoostPolygon> intersectionResult;
    try{
      boost::geometry::intersection(*boostPolygon1, *boostPolygon2, intersectionResult);
    }catch(const boost::geometry::overlay_invalid_input_exception&){
      LOG_FREE(Error, "utilities.geometry.boostPolygonFromVertices", "overlay_invalid_input_exception");
      return boost::none;
    }

    // check if intersection is empty
    if (intersectionResult.empty()){
      //LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Intersection is empty");
      return boost::none;
    }

    // check for multiple intersections
    if (intersectionResult.size() > 1){
      LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Intersection has " << intersectionResult.size() << " elements");
      std::sort(intersectionResult.begin(), intersectionResult.end(), BoostPolygonAreaGreater());
    }
    
    // check that largest intersection is ok
    std::vector<Point3d> intersectionVertices = verticesFromBoostPolygon(intersectionResult[0]);
    boost::optional<double> testArea = getArea(intersectionVertices);
    if (!testArea){
      LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Cannot compute area of largest intersection");
      return boost::none;
    }else if (*testArea < tol*tol){
      LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Largest intersection has very small area of " << *testArea << "m^2");
      return boost::none;
    }
    try{
      boost::geometry::detail::overlay::has_self_intersections(intersectionResult[0]);
    }catch(const boost::geometry::overlay_invalid_input_exception&){
      LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Largest intersection is self intersecting");
      return boost::none;
    }
    if (!intersectionResult[0].inners().empty()){
      LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Largest intersection has inner loops");
      return boost::none;
    };

    // intersections are the same
    resultPolygon1 = intersectionVertices;
    resultPolygon2 = intersectionVertices;

    // create new polygon for each remaining intersection
    for (unsigned i = 1; i < intersectionResult.size(); ++i){

      std::vector<Point3d> newPolygon = verticesFromBoostPolygon(intersectionResult[i]);

      testArea = getArea(newPolygon);
      if (!testArea){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Cannot compute area of intersection, result will not include this polygon, " << newPolygon);
        continue;
      }else if (*testArea < tol*tol){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Intersection has very small area of " << *testArea << "m^2, result will not include this polygon, " << newPolygon);
        continue;
      }
      try{
        boost::geometry::detail::overlay::has_self_intersections(intersectionResult[i]);
      }catch(const boost::geometry::overlay_invalid_input_exception&){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Intersection is self intersecting, result will not include this polygon, " << newPolygon);
        continue;
      }

      newPolygons1.push_back(newPolygon);
      newPolygons2.push_back(newPolygon);
    }

    // polygon1 minus polygon2
    std::vector<BoostPolygon> differenceResult1;
    boost::geometry::difference(*boostPolygon1, *boostPolygon2, differenceResult1);
    
    // create new polygon for each difference
    for (unsigned i = 0; i < differenceResult1.size(); ++i){

      std::vector<Point3d> newPolygon1 = verticesFromBoostPolygon(differenceResult1[i]);

      testArea = getArea(newPolygon1);
      if (!testArea){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Cannot compute area of face difference, result will not include this polygon, " << newPolygon1);
        continue;
      }else if (*testArea < tol*tol){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Face difference has very small area of " << *testArea << "m^2, result will not include this polygon, " << newPolygon1);
        continue;
      }
      try{
        boost::geometry::detail::overlay::has_self_intersections(differenceResult1[i]);
      }catch(const boost::geometry::overlay_invalid_input_exception&){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Face difference is self intersecting, result will not include this polygon, " << newPolygon1);
        continue;
      }

      newPolygons1.push_back(newPolygon1);
    }

    // polygon2 minus polygon1
    std::vector<BoostPolygon> differenceResult2;
    boost::geometry::difference(*boostPolygon2, *boostPolygon1, differenceResult2);

    // create new polygon for each difference
    for (unsigned i = 0; i < differenceResult2.size(); ++i){

      std::vector<Point3d> newPolygon2 = verticesFromBoostPolygon(differenceResult2[i]);

      testArea = getArea(newPolygon2);
      if (!testArea){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Cannot compute area of face difference, result will not include this polygon, " << newPolygon2);
        continue;
      }else if (*testArea < tol*tol){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Face difference has very small area of " << *testArea << "m^2, result will not include this polygon, " << newPolygon2);
        continue;
      }
      try{
        boost::geometry::detail::overlay::has_self_intersections(differenceResult2[i]);
      }catch(const boost::geometry::overlay_invalid_input_exception&){
        LOG_FREE(Info, "utilities.geometry.boostPolygonFromVertices", "Face difference is self intersecting, result will not include this polygon, " << newPolygon2);
        continue;
      }

      newPolygons2.push_back(newPolygon2);
    }

    return IntersectionResult(resultPolygon1, resultPolygon2, newPolygons1, newPolygons2);
  }

} // openstudio
