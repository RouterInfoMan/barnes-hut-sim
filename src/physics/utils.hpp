#pragma once
#include <set>
#include <cmath>


namespace utils {
    class Vector2 {
    public:
        double x, y;

        Vector2() {}
        ~Vector2() {}

        Vector2(double d0, double d1) {
            x = d0;
            y = d1;
        }
        Vector2 operator+(Vector2 const&obj) {
            Vector2 res;
            res.x = x + obj.x;
            res.y = y + obj.y;
            return res;
        }
        Vector2& operator+=(Vector2 const&obj) {
            x += obj.x;
            y += obj.y;
            return *this;
        }
        Vector2 operator-(Vector2 const&obj) {
            Vector2 res;
            res.x = x - obj.x;
            res.y = y - obj.y;
            return res;
        }
        Vector2& operator-=(Vector2 const&obj) {
            x -= obj.x;
            y -= obj.y;
            return *this;
        }
        Vector2 operator*(Vector2 const&obj) {
            Vector2 res;
            res.x = x * obj.x;
            res.y = y * obj.y;
            return res;
        }
        Vector2& operator*=(Vector2 const&obj) {
            x *= obj.x;
            y *= obj.y;
            return *this;
        }
        Vector2 operator*(double c) {
            Vector2 res;
            res.x = x * c;
            res.y = y * c;
            return res;
        }
        Vector2& operator*=(double c) {
            x *= c;
            y *= c;
            return *this;
        }
        Vector2 operator/(Vector2 const&obj) {
            Vector2 res;
            res.x = x / obj.x;
            res.y = y / obj.y;
            return res;
        }
        Vector2 operator/(double c) {
            Vector2 res;
            res.x = x / c;
            res.y = y / c;
            return res;
        }
        Vector2& operator/=(double c) {
            x /= c;
            y /= c;
             return *this;
        }
        double norm() {
            return sqrt(x*x + y*y);
        }
        double dot(Vector2 obj) {
            double res;
            res = x * obj.x;
            res += y * obj.y;
            return res;
        }

    };

    inline Vector2 operator*(const double c, const Vector2& obj) {
        Vector2 res;
        res.x = c * obj.x;
        res.y = c * obj.y;
        return res;
    }
    
    const double G = 6.67430e-11;
}