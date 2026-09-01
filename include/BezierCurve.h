#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class BezierCurve {
private:
    std::vector<glm::vec3> controlPoints;
    float totalLength;
    std::vector<float> arcLengths; // For uniform speed
    
    // Factorial function for binomial coefficients
    float factorial(int n) {
        float result = 1.0f;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }
        return result;
    }
    
    // Binomial coefficient (n choose k)
    float binomial(int n, int k) {
        return factorial(n) / (factorial(k) * factorial(n - k));
    }
    
    // Evaluate Bézier curve at parameter t (0 to 1)
    glm::vec3 evaluateBezier(float t) {
        int n = controlPoints.size() - 1;
        glm::vec3 result(0.0f);
        
        for (int i = 0; i <= n; i++) {
            float bernstein = binomial(n, i) * pow(t, i) * pow(1 - t, n - i);
            result += controlPoints[i] * bernstein;
        }
        
        return result;
    }
    glm::vec3 evaluateDerivative(float t) {
        int n = controlPoints.size() - 1;
        glm::vec3 result(0.0f);

        for (int i = 0; i < n; i++) {
            float bernstein = binomial(n - 1, i) * pow(t, i) * pow(1 - t, (n - 1) - i);
            result += (controlPoints[i + 1] - controlPoints[i]) * bernstein;
        }

        return result * (float)n;
    }
            
    // // Calculate derivative (tangent) at parameter t
    // glm::vec3 evaluateDerivative(float t) {
    //     int n = controlPoints.size() - 1;
    //     glm::vec3 result(0.0f);
        
    //     for (int i = 0; i <= n; i++) {
    //         if (i > 0) {
    //             float bernsteinDerivative = binomial(n, i) * i * pow(t, i-1) * pow(1 - t, n - i);
    //             result += controlPoints[i] * bernsteinDerivative;
    //         }
    //         if (i < n) {
    //             float bernsteinDerivative = -binomial(n, i) * (n - i) * pow(t, i) * pow(1 - t, n - i - 1);
    //             result += controlPoints[i] * bernsteinDerivative;
    //         }
    //     }
        
    //     return result;
    // }
    
    // Precompute arc lengths for uniform speed
    void computeArcLengths() {
        arcLengths.clear();
        arcLengths.push_back(0.0f);
        totalLength = 0.0f;
        
        int segments = 1000;
        glm::vec3 prevPos = evaluateBezier(0.0f);
        
        for (int i = 1; i <= segments; i++) {
            float t = (float)i / segments;
            glm::vec3 currentPos = evaluateBezier(t);
            float segmentLength = glm::distance(prevPos, currentPos);
            totalLength += segmentLength;
            arcLengths.push_back(totalLength);
            prevPos = currentPos;
        }
    }
    
    // Convert uniform distance to parameter t
    float distanceToT(float distance) {
        if (distance <= 0) return 0.0f;
        if (distance >= totalLength) return 1.0f;
        
        // Binary search for the correct t
        int low = 0;
        int high = arcLengths.size() - 1;
        
        while (low <= high) {
            int mid = (low + high) / 2;
            if (arcLengths[mid] < distance) {
                low = mid + 1;
            } else if (arcLengths[mid] > distance) {
                high = mid - 1;
            } else {
                return (float)mid / (arcLengths.size() - 1);
            }
        }
        
        // Interpolate between nearest points
        float t1 = (float)high / (arcLengths.size() - 1);
        float t2 = (float)low / (arcLengths.size() - 1);
        float d1 = arcLengths[high];
        float d2 = arcLengths[low];
        
        if (d2 - d1 < 0.0001f) return t1;
        
        float interp = (distance - d1) / (d2 - d1);
        return t1 + interp * (t2 - t1);
    }
    
public:
    BezierCurve() : totalLength(0.0f) {}
    
    void setControlPoints(const std::vector<glm::vec3>& points) {
        controlPoints = points;
        computeArcLengths();
    }

    glm::vec3 getTangent(float t) {
        if (controlPoints.empty()) return glm::vec3(0.0f, 0.0f, 1.0f);
        
        int n = controlPoints.size() - 1;
        glm::vec3 derivative(0.0f);
        
        // Derivative of Bézier curve
        for (int i = 0; i <= n; i++) {
            if (i > 0) {
                float bernsteinDerivative = binomial(n, i) * i * pow(t, i-1) * pow(1 - t, n - i);
                derivative += controlPoints[i] * bernsteinDerivative;
            }
            if (i < n) {
                float bernsteinDerivative = -binomial(n, i) * (n - i) * pow(t, i) * pow(1 - t, n - i - 1);
                derivative += controlPoints[i] * bernsteinDerivative;
            }
        }
        
        if (glm::length(derivative) < 0.0001f) {
            return glm::vec3(0.0f, 0.0f, 1.0f);
        }
        
        return glm::normalize(derivative);
    }
    
    // Get position at parameter t (0 to 1) with uniform speed
    glm::vec3 getPosition(float t) {
        if (controlPoints.empty()) return glm::vec3(0.0f);
        
        // Convert from uniform speed t to curve parameter
        float curveT = distanceToT(t * totalLength);
        return evaluateBezier(curveT);
    }
    
    // Get forward direction (where the camera should look)
    glm::vec3 getForward(float t) {
        if (controlPoints.empty()) return glm::vec3(0.0f, 0.0f, -1.0f);
        
        float curveT = distanceToT(t * totalLength);
        glm::vec3 tangent = evaluateDerivative(curveT);
        
        // Normalize the tangent to get direction
        if (glm::length(tangent) < 0.0001f) {
            return glm::vec3(0.0f, 0.0f, -1.0f);
        }
        
        return glm::normalize(tangent);
    }
    
    // Get up direction (can be constant or also interpolated)
    glm::vec3 getUp(float t) {
        // Keep constant up direction for smooth camera movement
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }
    
    // Get right direction (cross product of forward and up)
    glm::vec3 getRight(float t) {
        glm::vec3 forward = getForward(t);
        glm::vec3 up = getUp(t);
        return glm::normalize(glm::cross(forward, up));
    }
    
    float getTotalLength() {
        return totalLength;
    }
};