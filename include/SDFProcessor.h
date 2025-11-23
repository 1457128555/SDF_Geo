#ifndef SDF_PROCESSOR_H
#define SDF_PROCESSOR_H

#include "sdf.h"
#include <vector>

/**
 * @file SDFProcessor.h
 * @brief High-level API for SDF-based geometry processing
 * 
 * This class provides a simple interface for common SDF operations:
 * - Generating SDF fields from polygons
 * - Extracting iso-contours at specified thresholds
 * - Creating ring regions between two contours
 * - Triangulating regions for rendering
 * 
 * External users don't need to know about internal details like
 * Marching Squares, line segment connection, or resampling algorithms.
 */

/**
 * @brief Result structure for contour extraction
 */
struct ContourResult {
    std::vector<Point2D> orderedPoints;  // Ordered contour points (closed loop)
    std::vector<Point2D> lineSegments;   // Line segments for rendering (GL_LINES)
    bool isValid;                         // Whether extraction succeeded
};

/**
 * @brief Result structure for ring generation
 */
struct RingResult {
    std::vector<Point2D> triangleVertices;  // Triangle vertices for rendering (GL_TRIANGLES)
    int triangleCount;                       // Number of triangles
    std::vector<Point2D> outerContour;      // Outer contour points
    std::vector<Point2D> innerContour;      // Inner contour points
    bool isValid;                            // Whether generation succeeded
};

/**
 * @brief Configuration for SDF processing
 */
struct SDFConfig {
    float gridSize;      // Grid cell size (default: 0.1)
    float padding;       // Extra padding around polygon (default: 0.5)
    int maxGridSize;     // Maximum grid dimension (default: 512)
    int resamplePoints;  // Target points for resampling (default: 64)
    
    SDFConfig() 
        : gridSize(0.1f), padding(0.5f), maxGridSize(512), resamplePoints(64) {}
};

/**
 * @brief High-level SDF geometry processor
 * 
 * Usage example:
 * @code
 *   SDFProcessor processor;
 *   processor.setPolygon(points);
 *   
 *   // Extract a contour
 *   ContourResult contour = processor.extractContour(0.0f);
 *   if (contour.isValid) {
 *       renderLines(contour.lineSegments);
 *   }
 *   
 *   // Generate a ring
 *   RingResult ring = processor.generateRing(0.2f, 0.0f);
 *   if (ring.isValid) {
 *       renderTriangles(ring.triangleVertices);
 *   }
 * @endcode
 */
class SDFProcessor {
public:
    /**
     * @brief Constructor
     * @param config Optional configuration parameters
     */
    SDFProcessor(const SDFConfig& config = SDFConfig());
    
    /**
     * @brief Set the polygon to process
     * @param polygon Vector of points defining the polygon
     * @return true if polygon is valid (>= 3 points)
     */
    bool setPolygon(const std::vector<Point2D>& polygon);
    
    /**
     * @brief Get the current polygon
     * @return Reference to the polygon points
     */
    const std::vector<Point2D>& getPolygon() const { return m_polygon; }
    
    /**
     * @brief Calculate SDF value at a specific point
     * @param point Query point
     * @return SDF value (negative inside, positive outside, 0 on boundary)
     */
    float querySDFValue(const Point2D& point) const;
    
    /**
     * @brief Extract an iso-contour at a specific threshold
     * @param threshold SDF threshold value (0.0 = boundary)
     * @return ContourResult containing ordered points and line segments
     */
    ContourResult extractContour(float threshold);
    
    /**
     * @brief Generate a ring region between two thresholds
     * @param outerThreshold Outer contour threshold (should be > innerThreshold)
     * @param innerThreshold Inner contour threshold
     * @return RingResult containing triangle vertices for rendering
     */
    RingResult generateRing(float outerThreshold, float innerThreshold);
    
    /**
     * @brief Get the axis-aligned bounding box of the polygon
     * @param outMinX Output minimum X
     * @param outMaxX Output maximum X
     * @param outMinY Output minimum Y
     * @param outMaxY Output maximum Y
     */
    void getBounds(float& outMinX, float& outMaxX, 
                   float& outMinY, float& outMaxY) const;
    
    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void setConfig(const SDFConfig& config) { m_config = config; }
    
    /**
     * @brief Get current configuration
     * @return Current configuration
     */
    const SDFConfig& getConfig() const { return m_config; }
    
    /**
     * @brief Check if a valid polygon is loaded
     * @return true if polygon has >= 3 points
     */
    bool isValid() const { return m_polygon.size() >= 3; }
    
private:
    // Internal methods
    void generateSDFGrid();
    void clearSDFCache();
    
    // Member variables
    std::vector<Point2D> m_polygon;      // Input polygon
    SDFConfig m_config;                   // Configuration
    
    // Cached SDF data (generated on demand)
    float* m_sdfData;
    int m_sdfWidth;
    int m_sdfHeight;
    float m_sdfMinX, m_sdfMaxX;
    float m_sdfMinY, m_sdfMaxY;
    bool m_sdfCacheValid;
};

#endif // SDF_PROCESSOR_H
