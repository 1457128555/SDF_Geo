#include "sdf.h"
#include <algorithm>
#include <limits>

/**
 * @brief Calculate the shortest distance from a point to a line segment
 */
float pointToSegmentDistance(const Point2D& p, const Point2D& a, const Point2D& b)
{
    // Vector from a to p
    Point2D pa = {p.x - a.x, p.y - a.y};
    
    // Vector from a to b
    Point2D ba = {b.x - a.x, b.y - a.y};
    
    // Project pa onto ba to find the closest point on the segment
    float baDotBa = ba.x * ba.x + ba.y * ba.y;
    
    // Handle degenerate case where a and b are the same point
    if (baDotBa < 1e-10f)
    {
        return std::sqrt(pa.x * pa.x + pa.y * pa.y);
    }
    
    // Parameter t represents the projection point on the segment (0 = a, 1 = b)
    float t = (pa.x * ba.x + pa.y * ba.y) / baDotBa;
    
    // Clamp t to [0, 1] to stay within the segment
    t = clamp(t, 0.0f, 1.0f);
    
    // Calculate the closest point on the segment
    float closestX = a.x + ba.x * t;
    float closestY = a.y + ba.y * t;
    
    // Return distance from p to closest point
    float dx = p.x - closestX;
    float dy = p.y - closestY;
    
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief Check if a point is inside a polygon using winding number algorithm
 * 
 * This implementation uses the winding number algorithm which is robust
 * and works correctly for both convex and concave polygons.
 */
bool isPointInPolygon(const Point2D& p, const std::vector<Point2D>& polygon)
{
    if (polygon.size() < 3)
        return false;
    
    int winding = 0;
    int n = polygon.size();
    
    for (int i = 0; i < n; i++)
    {
        const Point2D& a = polygon[i];
        const Point2D& b = polygon[(i + 1) % n];
        
        if (a.y <= p.y)
        {
            if (b.y > p.y)  // Upward crossing
            {
                // Compute cross product to determine if point is left of edge
                float cross = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
                if (cross > 0)
                    winding++;
            }
        }
        else
        {
            if (b.y <= p.y)  // Downward crossing
            {
                // Compute cross product to determine if point is left of edge
                float cross = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
                if (cross < 0)
                    winding--;
            }
        }
    }
    
    return winding != 0;
}

/**
 * @brief Calculate the signed distance field value for a point relative to a polygon
 */
float calculatePolygonSDF(const Point2D& p, const std::vector<Point2D>& polygon)
{
    if (polygon.size() < 3)
    {
        // Invalid polygon, return a large positive distance
        return std::numeric_limits<float>::max();
    }
    
    // Find minimum distance to any edge
    float minDist = std::numeric_limits<float>::max();
    int n = polygon.size();
    
    for (int i = 0; i < n; i++)
    {
        const Point2D& a = polygon[i];
        const Point2D& b = polygon[(i + 1) % n];
        
        float dist = pointToSegmentDistance(p, a, b);
        minDist = std::min(minDist, dist);
    }
    
    // Determine if point is inside or outside and apply sign
    bool inside = isPointInPolygon(p, polygon);
    
    return inside ? -minDist : minDist;
}

/**
 * @brief Generate a 2D SDF grid for a polygon
 */
void generateSDFGrid(int width, int height,
                     float worldMinX, float worldMaxX,
                     float worldMinY, float worldMaxY,
                     const std::vector<Point2D>& polygon,
                     float* sdfData)
{
    if (sdfData == nullptr || width <= 0 || height <= 0)
        return;
    
    // Calculate step size for each grid cell
    float stepX = (worldMaxX - worldMinX) / width;
    float stepY = (worldMaxY - worldMinY) / height;
    
    // Sample SDF at each grid point
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Calculate world position for this grid cell
            Point2D worldPos = {
                worldMinX + (x + 0.5f) * stepX,  // Center of the cell
                worldMinY + (y + 0.5f) * stepY
            };
            
            // Calculate SDF value
            float sdf = calculatePolygonSDF(worldPos, polygon);
            
            // Store in row-major order
            sdfData[y * width + x] = sdf;
        }
    }
}

/**
 * @brief Calculate axis-aligned bounding box for a polygon
 */
void calculatePolygonBounds(const std::vector<Point2D>& polygon,
                            float& outMinX, float& outMaxX,
                            float& outMinY, float& outMaxY)
{
    if (polygon.empty())
    {
        outMinX = outMaxX = outMinY = outMaxY = 0.0f;
        return;
    }
    
    // Initialize with first point
    outMinX = outMaxX = polygon[0].x;
    outMinY = outMaxY = polygon[0].y;
    
    // Find min and max for all points
    for (size_t i = 1; i < polygon.size(); i++)
    {
        outMinX = std::min(outMinX, polygon[i].x);
        outMaxX = std::max(outMaxX, polygon[i].x);
        outMinY = std::min(outMinY, polygon[i].y);
        outMaxY = std::max(outMaxY, polygon[i].y);
    }
    
    // Add small padding to avoid edge cases
    float padding = 0.1f;
    outMinX -= padding;
    outMaxX += padding;
    outMinY -= padding;
    outMaxY += padding;
}

/**
 * @brief Calculate distance between two points
 */
static float distance(const Point2D& a, const Point2D& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief Calculate the total arc length of a contour
 */
static float calculateContourLength(const std::vector<Point2D>& contour)
{
    if (contour.size() < 2)
        return 0.0f;
    
    float totalLength = 0.0f;
    
    // Sum all edge lengths
    for (size_t i = 0; i < contour.size() - 1; i++)
    {
        totalLength += distance(contour[i], contour[i + 1]);
    }
    
    // For closed contours, add the closing edge
    // (Marching Squares produces line segments, so we check if it's already closed)
    if (contour.size() > 2)
    {
        float closingDist = distance(contour.back(), contour.front());
        if (closingDist > 0.001f) // Not already closed
        {
            totalLength += closingDist;
        }
    }
    
    return totalLength;
}

/**
 * @brief Connect line segments into an ordered closed contour
 */
std::vector<Point2D> connectLineSegments(const std::vector<Point2D>& segments)
{
    std::vector<Point2D> contour;
    
    if (segments.size() < 4) // Need at least 2 line segments (4 points)
        return contour;
    
    // Convert pairs of points into edges
    struct Edge {
        Point2D p0, p1;
        bool used;
    };
    
    std::vector<Edge> edges;
    for (size_t i = 0; i < segments.size(); i += 2)
    {
        if (i + 1 < segments.size())
        {
            edges.push_back({segments[i], segments[i + 1], false});
        }
    }
    
    if (edges.empty())
        return contour;
    
    // Start with the first edge
    contour.push_back(edges[0].p0);
    contour.push_back(edges[0].p1);
    edges[0].used = true;
    
    Point2D currentEnd = edges[0].p1;
    const float epsilon = 0.001f;
    
    // Greedily connect edges
    bool foundNext = true;
    while (foundNext && contour.size() < edges.size() * 2)
    {
        foundNext = false;
        
        for (size_t i = 1; i < edges.size(); i++)
        {
            if (edges[i].used)
                continue;
            
            // Check if this edge connects to current end
            float dist0 = distance(currentEnd, edges[i].p0);
            float dist1 = distance(currentEnd, edges[i].p1);
            
            if (dist0 < epsilon)
            {
                // Connect via p0 -> p1
                contour.push_back(edges[i].p1);
                currentEnd = edges[i].p1;
                edges[i].used = true;
                foundNext = true;
                break;
            }
            else if (dist1 < epsilon)
            {
                // Connect via p1 -> p0
                contour.push_back(edges[i].p0);
                currentEnd = edges[i].p0;
                edges[i].used = true;
                foundNext = true;
                break;
            }
        }
    }
    
    // Remove duplicate points (especially closing point)
    if (contour.size() > 1)
    {
        float closingDist = distance(contour.back(), contour.front());
        if (closingDist < epsilon)
        {
            contour.pop_back();
        }
    }
    
    return contour;
}

/**
 * @brief Resample a contour to have uniformly distributed points
 */
std::vector<Point2D> resampleContour(const std::vector<Point2D>& contour, int targetCount)
{
    std::vector<Point2D> resampled;
    
    if (contour.size() < 2 || targetCount < 3)
        return resampled;
    
    // Calculate total length
    float totalLength = calculateContourLength(contour);
    if (totalLength < 0.001f)
        return resampled;
    
    float segmentLength = totalLength / targetCount;
    
    // Always start with the first point
    resampled.push_back(contour[0]);
    
    float accumulatedLength = 0.0f;
    float nextSampleDistance = segmentLength;
    
    // Build a closed contour for iteration
    std::vector<Point2D> closedContour = contour;
    if (distance(contour.back(), contour.front()) > 0.001f)
    {
        closedContour.push_back(contour.front());
    }
    
    // Walk along the contour and sample points
    for (size_t i = 0; i < closedContour.size() - 1; i++)
    {
        Point2D p0 = closedContour[i];
        Point2D p1 = closedContour[i + 1];
        float edgeLength = distance(p0, p1);
        
        // Check if we need to sample on this edge
        while (accumulatedLength + edgeLength >= nextSampleDistance && resampled.size() < targetCount)
        {
            // Calculate interpolation parameter
            float remainingDistance = nextSampleDistance - accumulatedLength;
            float t = remainingDistance / edgeLength;
            
            // Interpolate position
            Point2D sample = {
                lerp(p0.x, p1.x, t),
                lerp(p0.y, p1.y, t)
            };
            
            resampled.push_back(sample);
            nextSampleDistance += segmentLength;
        }
        
        accumulatedLength += edgeLength;
        
        if (resampled.size() >= targetCount)
            break;
    }
    
    return resampled;
}

/**
 * @brief Triangulate the ring region between two contours
 */
std::vector<Triangle> triangulateRing(
    const std::vector<Point2D>& outerContour,
    const std::vector<Point2D>& innerContour)
{
    std::vector<Triangle> triangles;
    
    if (outerContour.size() < 3 || innerContour.size() < 3)
        return triangles;
    
    // Determine target point count (use the larger one, minimum 32)
    int targetCount = std::max(
        static_cast<int>(outerContour.size()),
        static_cast<int>(innerContour.size())
    );
    targetCount = std::max(targetCount, 32);
    
    // Resample both contours to have the same number of points
    std::vector<Point2D> outer = resampleContour(outerContour, targetCount);
    std::vector<Point2D> inner = resampleContour(innerContour, targetCount);
    
    // Make sure both resampling succeeded
    if (outer.size() < 3 || inner.size() < 3)
        return triangles;
    
    // Use the minimum size in case resampling didn't produce exact counts
    size_t pointCount = std::min(outer.size(), inner.size());
    
    // Generate triangle strip
    for (size_t i = 0; i < pointCount; i++)
    {
        size_t next = (i + 1) % pointCount;
        
        // First triangle: outer[i] -> inner[i] -> outer[next]
        Triangle tri1;
        tri1.v0 = outer[i];
        tri1.v1 = inner[i];
        tri1.v2 = outer[next];
        triangles.push_back(tri1);
        
        // Second triangle: outer[next] -> inner[i] -> inner[next]
        Triangle tri2;
        tri2.v0 = outer[next];
        tri2.v1 = inner[i];
        tri2.v2 = inner[next];
        triangles.push_back(tri2);
    }
    
    return triangles;
}

/**
 * @brief Convert triangles to flat vertex array for rendering
 */
std::vector<Point2D> trianglesToVertices(const std::vector<Triangle>& triangles)
{
    std::vector<Point2D> vertices;
    vertices.reserve(triangles.size() * 3);
    
    for (const auto& tri : triangles)
    {
        vertices.push_back(tri.v0);
        vertices.push_back(tri.v1);
        vertices.push_back(tri.v2);
    }
    
    return vertices;
}

/**
 * @brief Extract iso-contour from SDF grid using Marching Squares algorithm
 */
std::vector<Point2D> extractIsoContour(const float* sdfData,
                                       int width, int height,
                                       float worldMinX, float worldMaxX,
                                       float worldMinY, float worldMaxY,
                                       float isoValue)
{
    std::vector<Point2D> contourPoints;
    
    if (sdfData == nullptr || width < 2 || height < 2)
        return contourPoints;
    
    // Calculate cell size in world coordinates
    float cellWidth = (worldMaxX - worldMinX) / (width - 1);
    float cellHeight = (worldMaxY - worldMinY) / (height - 1);
    
    // Marching Squares lookup table for edge intersections
    // Each entry represents which edges to connect for a given case
    // Edges: 0=bottom, 1=right, 2=top, 3=left
    const int edgeTable[16][4] = {
        {-1, -1, -1, -1}, // Case 0: no intersection
        {0, 3, -1, -1},   // Case 1: bottom-left
        {0, 1, -1, -1},   // Case 2: bottom-right
        {1, 3, -1, -1},   // Case 3: bottom edge
        {1, 2, -1, -1},   // Case 4: top-right
        {0, 3, 1, 2},     // Case 5: ambiguous (saddle)
        {0, 2, -1, -1},   // Case 6: right edge
        {2, 3, -1, -1},   // Case 7: top-right corner
        {2, 3, -1, -1},   // Case 8: top-left
        {0, 2, -1, -1},   // Case 9: left edge
        {0, 1, 2, 3},     // Case 10: ambiguous (saddle)
        {1, 2, -1, -1},   // Case 11: top-left corner
        {1, 3, -1, -1},   // Case 12: top edge
        {0, 1, -1, -1},   // Case 13: top-right corner
        {0, 3, -1, -1},   // Case 14: bottom-left corner
        {-1, -1, -1, -1}  // Case 15: no intersection
    };
    
    // Process each 2x2 cell in the grid
    for (int y = 0; y < height - 1; y++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            // Get the 4 corner values
            float v0 = sdfData[y * width + x];           // bottom-left
            float v1 = sdfData[y * width + (x + 1)];     // bottom-right
            float v2 = sdfData[(y + 1) * width + (x + 1)]; // top-right
            float v3 = sdfData[(y + 1) * width + x];     // top-left
            
            // Calculate case index (4-bit value)
            int caseIndex = 0;
            if (v0 > isoValue) caseIndex |= 1;
            if (v1 > isoValue) caseIndex |= 2;
            if (v2 > isoValue) caseIndex |= 4;
            if (v3 > isoValue) caseIndex |= 8;
            
            // Skip if no intersection
            if (caseIndex == 0 || caseIndex == 15)
                continue;
            
            // Calculate world coordinates of cell corners
            float x0 = worldMinX + x * cellWidth;
            float x1 = worldMinX + (x + 1) * cellWidth;
            float y0 = worldMinY + y * cellHeight;
            float y1 = worldMinY + (y + 1) * cellHeight;
            
            // Calculate intersection points on edges using linear interpolation
            Point2D edgePoints[4];
            
            // Edge 0: bottom (v0 to v1)
            float t0 = (isoValue - v0) / (v1 - v0);
            edgePoints[0] = {lerp(x0, x1, t0), y0};
            
            // Edge 1: right (v1 to v2)
            float t1 = (isoValue - v1) / (v2 - v1);
            edgePoints[1] = {x1, lerp(y0, y1, t1)};
            
            // Edge 2: top (v3 to v2)
            float t2 = (isoValue - v3) / (v2 - v3);
            edgePoints[2] = {lerp(x0, x1, t2), y1};
            
            // Edge 3: left (v0 to v3)
            float t3 = (isoValue - v0) / (v3 - v0);
            edgePoints[3] = {x0, lerp(y0, y1, t3)};
            
            // Add line segments based on the lookup table
            const int* edges = edgeTable[caseIndex];
            for (int i = 0; i < 4 && edges[i] != -1; i += 2)
            {
                if (i + 1 < 4 && edges[i + 1] != -1)
                {
                    contourPoints.push_back(edgePoints[edges[i]]);
                    contourPoints.push_back(edgePoints[edges[i + 1]]);
                }
            }
        }
    }
    
    return contourPoints;
}
