#ifndef SDF_H
#define SDF_H

#include <vector>
#include <cmath>

/**
 * @file sdf.h
 * @brief 2D Signed Distance Field (SDF) calculation module
 * 
 * This module provides functions to calculate signed distance fields
 * for 2D polygons defined by a set of points.
 */

// Point structure for 2D coordinates
struct Point2D {
    float x, y;
};

// Triangle structure
struct Triangle {
    Point2D v0, v1, v2;
};

/**
 * @brief Calculate the shortest distance from a point to a line segment
 * 
 * @param p The query point
 * @param a First endpoint of the line segment
 * @param b Second endpoint of the line segment
 * @return float The shortest distance from point p to segment ab
 */
float pointToSegmentDistance(const Point2D& p, const Point2D& a, const Point2D& b);

/**
 * @brief Check if a point is inside a polygon using winding number algorithm
 * 
 * @param p The query point
 * @param polygon Vector of points defining the polygon (closed loop)
 * @return true if point is inside the polygon
 * @return false if point is outside the polygon
 */
bool isPointInPolygon(const Point2D& p, const std::vector<Point2D>& polygon);

/**
 * @brief Calculate the signed distance field value for a point relative to a polygon
 * 
 * The SDF value represents:
 * - Positive: Point is outside the polygon (distance to nearest surface)
 * - Negative: Point is inside the polygon (distance to nearest surface)
 * - Zero: Point is on the boundary
 * 
 * @param p The query point
 * @param polygon Vector of points defining the polygon
 * @return float The signed distance value
 */
float calculatePolygonSDF(const Point2D& p, const std::vector<Point2D>& polygon);

/**
 * @brief Generate a 2D SDF texture/grid for a polygon
 * 
 * Samples the SDF at regular grid points within a specified world space region.
 * 
 * @param width Width of the output grid
 * @param height Height of the output grid
 * @param worldMinX Minimum X coordinate in world space
 * @param worldMaxX Maximum X coordinate in world space
 * @param worldMinY Minimum Y coordinate in world space
 * @param worldMaxY Maximum Y coordinate in world space
 * @param polygon Vector of points defining the polygon
 * @param sdfData Output buffer (must be pre-allocated with width*height floats)
 */
void generateSDFGrid(int width, int height,
                     float worldMinX, float worldMaxX,
                     float worldMinY, float worldMaxY,
                     const std::vector<Point2D>& polygon,
                     float* sdfData);

/**
 * @brief Calculate axis-aligned bounding box for a polygon
 * 
 * @param polygon Vector of points defining the polygon
 * @param outMinX Output: minimum X coordinate
 * @param outMaxX Output: maximum X coordinate
 * @param outMinY Output: minimum Y coordinate
 * @param outMaxY Output: maximum Y coordinate
 */
void calculatePolygonBounds(const std::vector<Point2D>& polygon,
                            float& outMinX, float& outMaxX,
                            float& outMinY, float& outMaxY);

/**
 * @brief Extract iso-contour from SDF grid using Marching Squares algorithm
 * 
 * This function extracts a contour line where the SDF value equals the threshold.
 * The contour is represented as a series of line segments connecting points.
 * 
 * @param sdfData The SDF grid data (row-major order)
 * @param width Width of the SDF grid
 * @param height Height of the SDF grid
 * @param worldMinX Minimum X coordinate in world space
 * @param worldMaxX Maximum X coordinate in world space
 * @param worldMinY Minimum Y coordinate in world space
 * @param worldMaxY Maximum Y coordinate in world space
 * @param isoValue The SDF threshold value to extract (typically 0.0 for boundary)
 * @return Vector of points forming the contour line segments (pairs of points)
 */
std::vector<Point2D> extractIsoContour(const float* sdfData,
                                       int width, int height,
                                       float worldMinX, float worldMaxX,
                                       float worldMinY, float worldMaxY,
                                       float isoValue);

/**
 * @brief Connect line segments into an ordered closed contour
 * 
 * Marching Squares produces unordered line segments. This function connects them
 * into a continuous closed contour.
 * 
 * @param segments Line segments (pairs of points: P0,P1, P2,P3, ...)
 * @return Ordered contour points forming a closed loop
 */
std::vector<Point2D> connectLineSegments(const std::vector<Point2D>& segments);

/**
 * @brief Resample a contour to have a specific number of uniformly distributed points
 * 
 * This function resamples a contour curve by distributing points evenly along
 * the arc length of the curve.
 * 
 * @param contour Original contour points
 * @param targetCount Target number of points
 * @return Vector of resampled points
 */
std::vector<Point2D> resampleContour(const std::vector<Point2D>& contour, int targetCount);

/**
 * @brief Triangulate the ring region between two contours using Triangle Strip method
 * 
 * This function creates triangles filling the area between an outer and inner contour.
 * Both contours are resampled to have the same number of points, then connected
 * to form a ring of triangles.
 * 
 * @param outerContour Outer contour points (should be closed)
 * @param innerContour Inner contour points (should be closed)
 * @return Vector of triangles forming the ring
 */
std::vector<Triangle> triangulateRing(
    const std::vector<Point2D>& outerContour,
    const std::vector<Point2D>& innerContour);

/**
 * @brief Convert triangles to a flat vertex array for rendering
 * 
 * @param triangles Vector of triangles
 * @return Flat vector of vertices (3 per triangle)
 */
std::vector<Point2D> trianglesToVertices(const std::vector<Triangle>& triangles);

/**
 * @brief Linear interpolation helper function
 */
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

/**
 * @brief Clamp a value between min and max
 */
inline float clamp(float value, float minVal, float maxVal) {
    return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value;
}

#endif // SDF_H
