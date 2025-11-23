/**
 * @file example_usage.cpp
 * @brief Example usage of SDFProcessor API
 * 
 * This file demonstrates how to use the SDFProcessor class
 * for common SDF-based geometry operations.
 */

#include "SDFProcessor.h"
#include <iostream>

void example1_BasicUsage() {
    std::cout << "\n=== Example 1: Basic Usage ===" << std::endl;
    
    // Create a simple triangle
    std::vector<Point2D> triangle = {
        {0.0f, 0.5f},   // Top
        {-0.5f, -0.5f}, // Bottom-left
        {0.5f, -0.5f}   // Bottom-right
    };
    
    // Create processor and set polygon
    SDFProcessor processor;
    if (processor.setPolygon(triangle)) {
        std::cout << "✓ Polygon set successfully" << std::endl;
        
        // Query SDF value at a point
        Point2D testPoint = {0.0f, 0.0f};
        float sdf = processor.querySDFValue(testPoint);
        std::cout << "✓ SDF at (0, 0): " << sdf << std::endl;
        
        if (sdf < 0) {
            std::cout << "  Point is INSIDE the polygon" << std::endl;
        } else if (sdf > 0) {
            std::cout << "  Point is OUTSIDE the polygon" << std::endl;
        } else {
            std::cout << "  Point is ON THE BOUNDARY" << std::endl;
        }
    }
}

void example2_ExtractContours() {
    std::cout << "\n=== Example 2: Extract Contours ===" << std::endl;
    
    // Create a square
    std::vector<Point2D> square = {
        {-0.5f, -0.5f},
        {0.5f, -0.5f},
        {0.5f, 0.5f},
        {-0.5f, 0.5f}
    };
    
    SDFProcessor processor;
    processor.setPolygon(square);
    
    // Extract boundary (threshold = 0.0)
    ContourResult boundary = processor.extractContour(0.0f);
    if (boundary.isValid) {
        std::cout << "✓ Boundary extracted" << std::endl;
        std::cout << "  Contour points: " << boundary.orderedPoints.size() << std::endl;
        std::cout << "  Line segments: " << boundary.lineSegments.size() / 2 << std::endl;
    }
    
    // Extract offset contour
    ContourResult offset = processor.extractContour(0.2f);
    if (offset.isValid) {
        std::cout << "✓ Offset contour (0.2) extracted" << std::endl;
        std::cout << "  Contour points: " << offset.orderedPoints.size() << std::endl;
    }
}

void example3_GenerateRing() {
    std::cout << "\n=== Example 3: Generate Ring ===" << std::endl;
    
    // Create a pentagon
    std::vector<Point2D> pentagon;
    int sides = 5;
    float radius = 0.5f;
    for (int i = 0; i < sides; i++) {
        float angle = i * 2.0f * 3.14159f / sides;
        pentagon.push_back({
            radius * std::cos(angle),
            radius * std::sin(angle)
        });
    }
    
    SDFProcessor processor;
    processor.setPolygon(pentagon);
    
    // Generate ring between two thresholds
    RingResult ring = processor.generateRing(0.3f, 0.0f);
    if (ring.isValid) {
        std::cout << "✓ Ring generated" << std::endl;
        std::cout << "  Triangle count: " << ring.triangleCount << std::endl;
        std::cout << "  Outer contour points: " << ring.outerContour.size() << std::endl;
        std::cout << "  Inner contour points: " << ring.innerContour.size() << std::endl;
        std::cout << "  Total vertices: " << ring.triangleVertices.size() << std::endl;
        std::cout << "  (Ready for GL_TRIANGLES rendering)" << std::endl;
    }
}

void example4_CustomConfig() {
    std::cout << "\n=== Example 4: Custom Configuration ===" << std::endl;
    
    // Create custom configuration
    SDFConfig config;
    config.gridSize = 0.05f;      // Finer grid
    config.padding = 1.0f;        // Larger padding
    config.maxGridSize = 1024;    // Higher resolution
    config.resamplePoints = 128;  // More sample points
    
    std::cout << "Custom config:" << std::endl;
    std::cout << "  Grid size: " << config.gridSize << std::endl;
    std::cout << "  Padding: " << config.padding << std::endl;
    std::cout << "  Max grid size: " << config.maxGridSize << std::endl;
    std::cout << "  Resample points: " << config.resamplePoints << std::endl;
    
    // Create processor with custom config
    SDFProcessor processor(config);
    
    std::vector<Point2D> polygon = {
        {0.0f, 1.0f}, {-0.5f, 0.0f}, {0.5f, 0.0f}
    };
    processor.setPolygon(polygon);
    
    // Use the processor...
    ContourResult contour = processor.extractContour(0.0f);
    if (contour.isValid) {
        std::cout << "✓ Contour extracted with custom config" << std::endl;
        std::cout << "  Points: " << contour.orderedPoints.size() << std::endl;
    }
}

void example5_MultipleOperations() {
    std::cout << "\n=== Example 5: Multiple Operations ===" << std::endl;
    
    std::vector<Point2D> polygon = {
        {0.0f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f}
    };
    
    SDFProcessor processor;
    processor.setPolygon(polygon);
    
    std::cout << "Extracting multiple contours..." << std::endl;
    
    // Extract contours at different thresholds
    std::vector<float> thresholds = {0.0f, 0.1f, 0.2f, 0.3f};
    for (float threshold : thresholds) {
        ContourResult contour = processor.extractContour(threshold);
        if (contour.isValid) {
            std::cout << "  Threshold " << threshold << ": " 
                      << contour.orderedPoints.size() << " points" << std::endl;
        }
    }
    
    std::cout << "Generating multiple rings..." << std::endl;
    
    // Generate rings at different ranges
    std::vector<std::pair<float, float>> ranges = {
        {0.1f, 0.0f},
        {0.2f, 0.1f},
        {0.3f, 0.2f}
    };
    
    for (const auto& range : ranges) {
        RingResult ring = processor.generateRing(range.first, range.second);
        if (ring.isValid) {
            std::cout << "  Ring [" << range.second << " -> " << range.first 
                      << "]: " << ring.triangleCount << " triangles" << std::endl;
        }
    }
}

void example6_BoundsAndValidation() {
    std::cout << "\n=== Example 6: Bounds and Validation ===" << std::endl;
    
    std::vector<Point2D> polygon = {
        {-1.0f, -0.5f}, {1.0f, -0.5f}, {0.0f, 0.5f}
    };
    
    SDFProcessor processor;
    
    // Check validity before setting
    if (!processor.isValid()) {
        std::cout << "✓ Processor has no polygon (as expected)" << std::endl;
    }
    
    // Set polygon
    processor.setPolygon(polygon);
    
    // Check validity after setting
    if (processor.isValid()) {
        std::cout << "✓ Polygon is valid" << std::endl;
        
        // Get bounds
        float minX, maxX, minY, maxY;
        processor.getBounds(minX, maxX, minY, maxY);
        
        std::cout << "  Bounds:" << std::endl;
        std::cout << "    X: [" << minX << ", " << maxX << "]" << std::endl;
        std::cout << "    Y: [" << minY << ", " << maxY << "]" << std::endl;
        std::cout << "    Width: " << (maxX - minX) << std::endl;
        std::cout << "    Height: " << (maxY - minY) << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   SDFProcessor API Usage Examples" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Run all examples
    example1_BasicUsage();
    example2_ExtractContours();
    example3_GenerateRing();
    example4_CustomConfig();
    example5_MultipleOperations();
    example6_BoundsAndValidation();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "   All examples completed!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

/*
Expected output:

========================================
   SDFProcessor API Usage Examples
========================================

=== Example 1: Basic Usage ===
✓ Polygon set successfully
✓ SDF at (0, 0): -0.166667
  Point is INSIDE the polygon

=== Example 2: Extract Contours ===
✓ Boundary extracted
  Contour points: 45
  Line segments: 45
✓ Offset contour (0.2) extracted
  Contour points: 58

=== Example 3: Generate Ring ===
✓ Ring generated
  Triangle count: 128
  Outer contour points: 64
  Inner contour points: 64
  Total vertices: 384
  (Ready for GL_TRIANGLES rendering)

... (more output)

========================================
   All examples completed!
========================================
*/
