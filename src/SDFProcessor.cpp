#include "SDFProcessor.h"
#include <algorithm>
#include <iostream>

SDFProcessor::SDFProcessor(const SDFConfig& config)
    : m_config(config)
    , m_sdfData(nullptr)
    , m_sdfWidth(0)
    , m_sdfHeight(0)
    , m_sdfMinX(0.0f)
    , m_sdfMaxX(0.0f)
    , m_sdfMinY(0.0f)
    , m_sdfMaxY(0.0f)
    , m_sdfCacheValid(false)
{
}

bool SDFProcessor::setPolygon(const std::vector<Point2D>& polygon)
{
    // Clear previous cache
    clearSDFCache();
    
    // Validate polygon
    if (polygon.size() < 3)
    {
        m_polygon.clear();
        return false;
    }
    
    m_polygon = polygon;
    return true;
}

float SDFProcessor::querySDFValue(const Point2D& point) const
{
    if (!isValid())
        return 0.0f;
    
    // Direct calculation (no need for grid)
    return ::calculatePolygonSDF(point, m_polygon);
}

ContourResult SDFProcessor::extractContour(float threshold)
{
    ContourResult result;
    result.isValid = false;
    
    if (!isValid())
    {
        std::cerr << "SDFProcessor: No valid polygon set" << std::endl;
        return result;
    }
    
    // Generate SDF grid if not cached
    if (!m_sdfCacheValid)
    {
        generateSDFGrid();
        if (!m_sdfCacheValid)
        {
            std::cerr << "SDFProcessor: Failed to generate SDF grid" << std::endl;
            return result;
        }
    }
    
    // Extract line segments using Marching Squares
    std::vector<Point2D> segments = ::extractIsoContour(
        m_sdfData, m_sdfWidth, m_sdfHeight,
        m_sdfMinX, m_sdfMaxX, m_sdfMinY, m_sdfMaxY,
        threshold
    );
    
    if (segments.size() < 4) // Need at least 2 line segments
    {
        std::cerr << "SDFProcessor: No contour found at threshold " << threshold << std::endl;
        return result;
    }
    
    // Connect line segments into ordered contour
    result.orderedPoints = ::connectLineSegments(segments);
    result.lineSegments = segments;
    result.isValid = result.orderedPoints.size() >= 3;
    
    if (!result.isValid)
    {
        std::cerr << "SDFProcessor: Failed to connect line segments" << std::endl;
    }
    
    return result;
}

RingResult SDFProcessor::generateRing(float outerThreshold, float innerThreshold)
{
    RingResult result;
    result.triangleCount = 0;
    result.isValid = false;
    
    if (!isValid())
    {
        std::cerr << "SDFProcessor: No valid polygon set" << std::endl;
        return result;
    }
    
    if (outerThreshold <= innerThreshold)
    {
        std::cerr << "SDFProcessor: outerThreshold must be > innerThreshold" << std::endl;
        return result;
    }
    
    // Extract both contours
    ContourResult outerContour = extractContour(outerThreshold);
    ContourResult innerContour = extractContour(innerThreshold);
    
    if (!outerContour.isValid || !innerContour.isValid)
    {
        std::cerr << "SDFProcessor: Failed to extract contours" << std::endl;
        return result;
    }
    
    // Store contours
    result.outerContour = outerContour.orderedPoints;
    result.innerContour = innerContour.orderedPoints;
    
    // Triangulate the ring
    std::vector<Triangle> triangles = ::triangulateRing(
        result.outerContour,
        result.innerContour
    );
    
    if (triangles.empty())
    {
        std::cerr << "SDFProcessor: Triangulation failed" << std::endl;
        return result;
    }
    
    // Convert to vertex array
    result.triangleVertices = ::trianglesToVertices(triangles);
    result.triangleCount = static_cast<int>(triangles.size());
    result.isValid = true;
    
    return result;
}

void SDFProcessor::getBounds(float& outMinX, float& outMaxX, 
                             float& outMinY, float& outMaxY) const
{
    if (!isValid())
    {
        outMinX = outMaxX = outMinY = outMaxY = 0.0f;
        return;
    }
    
    ::calculatePolygonBounds(m_polygon, outMinX, outMaxX, outMinY, outMaxY);
}

void SDFProcessor::generateSDFGrid()
{
    clearSDFCache();
    
    if (!isValid())
        return;
    
    // Calculate bounds with padding
    float minX, maxX, minY, maxY;
    ::calculatePolygonBounds(m_polygon, minX, maxX, minY, maxY);
    
    m_sdfMinX = minX - m_config.padding;
    m_sdfMaxX = maxX + m_config.padding;
    m_sdfMinY = minY - m_config.padding;
    m_sdfMaxY = maxY + m_config.padding;
    
    // Calculate grid dimensions
    float rangeX = m_sdfMaxX - m_sdfMinX;
    float rangeY = m_sdfMaxY - m_sdfMinY;
    
    m_sdfWidth = static_cast<int>(rangeX / m_config.gridSize) + 1;
    m_sdfHeight = static_cast<int>(rangeY / m_config.gridSize) + 1;
    
    // Limit grid size
    m_sdfWidth = std::min(m_sdfWidth, m_config.maxGridSize);
    m_sdfHeight = std::min(m_sdfHeight, m_config.maxGridSize);
    
    // Allocate and generate SDF data
    m_sdfData = new float[m_sdfWidth * m_sdfHeight];
    
    // Call global function from sdf.h (not the member function)
    ::generateSDFGrid(
        m_sdfWidth, m_sdfHeight,
        m_sdfMinX, m_sdfMaxX, m_sdfMinY, m_sdfMaxY,
        m_polygon, m_sdfData
    );
    
    m_sdfCacheValid = true;
}

void SDFProcessor::clearSDFCache()
{
    if (m_sdfData != nullptr)
    {
        delete[] m_sdfData;
        m_sdfData = nullptr;
    }
    
    m_sdfWidth = 0;
    m_sdfHeight = 0;
    m_sdfCacheValid = false;
}
