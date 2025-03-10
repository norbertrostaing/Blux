/**
 *  ============================================================================
 *  MIT License
 *
 *  Copyright (c) 2016 Eric Phillips
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *  ============================================================================
 *
 *
 *  This file implements a series of math functions for manipulating a
 *  quaternion.
 *
 *  Created by Eric Phillips on October 21, 2016.
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#define SMALL_DOUBLE 0.0000000001


/**
 * Attempt to include a header file if the file exists.
 * If the file does not exist, create a dummy data structure for that type.
 * If it cannot be determined if it exists, just attempt to include it.
 */
#ifdef __has_include
#   if __has_include("Vector3.hpp")
#       include "Vector3.hpp"
#   elif !defined(GMATH_VECTOR3)
        #define GMATH_VECTOR3
        struct Vector3
        {
            union
            {
                struct
                {
                    double X;
                    double Y;
                    double Z;
                };
                double data[3];
            };

            inline Vector3() : X(0), Y(0), Z(0) {}
            inline Vector3(double data[]) : X(data[0]), Y(data[1]), Z(data[2])
                {}
            inline Vector3(double value) : X(value), Y(value), Z(value) {}
            inline Vector3(double x, double y) : X(x), Y(y), Z(0) {}
            inline Vector3(double x, double y, double z) : X(x), Y(y), Z(z) {}

            static inline Vector3 Cross(Vector3 lhs, Vector3 rhs)
            {
                double x = lhs.Y * rhs.Z - lhs.Z * rhs.Y;
                double y = lhs.Z * rhs.X - lhs.X * rhs.Z;
                double z = lhs.X * rhs.Y - lhs.Y * rhs.X;
                return Vector3(x, y, z);
            }

            static inline double Dot(Vector3 lhs, Vector3 rhs)
            {
                return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
            }

            static inline Vector3 Normalized(Vector3 v)
            {
                double mag = sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
                if (mag == 0)
                    return Vector3::Zero();
                return v / mag;
            }

            static inline Vector3 Orthogonal(Vector3 v)
            {
                return v.Z < v.X ?
                    Vector3(v.Y, -v.X, 0) : Vector3(0, -v.Z, v.Y);
            }

            static inline double SqrMagnitude(Vector3 v)
            {
                return v.X * v.X + v.Y * v.Y + v.Z * v.Z;
            }
        };


        inline Vector3 operator+(Vector3 lhs, const Vector3 rhs)
        {
            return Vector3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
        }

        inline Vector3 operator*(Vector3 lhs, const double rhs)
        {
            return Vector3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
        }
#   endif
#else
#   include "Vector3.hpp"
#endif

namespace unityports
{

    struct Quaternion
    {
        union
        {
            struct
            {
                double X;
                double Y;
                double Z;
                double W;
            };
            double data[4];
        };


        /**
         * Constructors.
         */
        inline Quaternion();
        inline Quaternion(double data[]);
        inline Quaternion(Vector3 vector, double scalar);
        inline Quaternion(double x, double y, double z, double w);


        /**
         * Constants for common quaternions.
         */
        static inline Quaternion Identity();


        /**
         * Returns the angle between two quaternions.
         * The quaternions must be normalized.
         * @param a: The first quaternion.
         * @param b: The second quaternion.
         * @return: A scalar value.
         */
        static inline double Angle(Quaternion a, Quaternion b);

        /**
         * Returns the conjugate of a quaternion.
         * @param rotation: The quaternion in question.
         * @return: A new quaternion.
         */
        static inline Quaternion Conjugate(Quaternion rotation);

        /**
         * Returns the dot product of two quaternions.
         * @param lhs: The left side of the multiplication.
         * @param rhs: The right side of the multiplication.
         * @return: A scalar value.
         */
        static inline double Dot(Quaternion lhs, Quaternion rhs);

        /**
         * Creates a new quaternion from the angle-axis representation of
         * a rotation.
         * @param angle: The rotation angle in radians.
         * @param axis: The vector about which the rotation occurs.
         * @return: A new quaternion.
         */
        static inline Quaternion FromAngleAxis(double angle, Vector3 axis);

        /**
         * Create a new quaternion from the euler angle representation of
         * a rotation. The z, x and y values represent rotations about those
         * axis in that respective order.
         * @param rotation: The x, y and z rotations.
         * @return: A new quaternion.
         */
        static inline Quaternion FromEuler(Vector3 rotation);

        /**
         * Create a new quaternion from the euler angle representation of
         * a rotation. The z, x and y values represent rotations about those
         * axis in that respective order.
         * @param x: The rotation about the x-axis in radians.
         * @param y: The rotation about the y-axis in radians.
         * @param z: The rotation about the z-axis in radians.
         * @return: A new quaternion.
         */
        static inline Quaternion FromEuler(double x, double y, double z);

        /**
         * Create a quaternion rotation which rotates "fromVector" to "toVector".
         * @param fromVector: The vector from which to start the rotation.
         * @param toVector: The vector at which to end the rotation.
         * @return: A new quaternion.
         */
        static inline Quaternion FromToRotation(Vector3 fromVector,
            Vector3 toVector);

        /**
         * Returns the inverse of a rotation.
         * @param rotation: The quaternion in question.
         * @return: A new quaternion.
         */
        static inline Quaternion Inverse(Quaternion rotation);

        /**
         * Interpolates between a and b by t, which is clamped to the range [0-1].
         * The result is normalized before being returned.
         * @param a: The starting rotation.
         * @param b: The ending rotation.
         * @return: A new quaternion.
         */
        static inline Quaternion Lerp(Quaternion a, Quaternion b, double t);

        /**
         * Interpolates between a and b by t. This normalizes the result when
         * complete.
         * @param a: The starting rotation.
         * @param b: The ending rotation.
         * @param t: The interpolation value.
         * @return: A new quaternion.
         */
        static inline Quaternion LerpUnclamped(Quaternion a, Quaternion b,
            double t);

        /**
         * Creates a rotation with the specified forward direction. This is the
         * same as calling LookRotation with (0, 1, 0) as the upwards vector.
         * The output is undefined for parallel vectors.
         * @param forward: The forward direction to look toward.
         * @return: A new quaternion.
         */
        static inline Quaternion LookRotation(Vector3 forward);

        /**
         * Creates a rotation with the specified forward and upwards directions.
         * The output is undefined for parallel vectors.
         * @param forward: The forward direction to look toward.
         * @param upwards: The direction to treat as up.
         * @return: A new quaternion.
         */
        static inline Quaternion LookRotation(Vector3 forward, Vector3 upwards);

        /**
         * Returns the norm of a quaternion.
         * @param rotation: The quaternion in question.
         * @return: A scalar value.
         */
        static inline double Norm(Quaternion rotation);

        /**
         * Returns a quaternion with identical rotation and a norm of one.
         * @param rotation: The quaternion in question.
         * @return: A new quaternion.
         */
        static inline Quaternion Normalized(Quaternion rotation);

        /**
         * Returns a new Quaternion created by rotating "from" towards "to" by
         * "maxRadiansDelta". This will not overshoot, and if a negative delta is
         * applied, it will rotate till completely opposite "to" and then stop.
         * @param from: The rotation at which to start.
         * @param to: The rotation at which to end.
         # @param maxRadiansDelta: The maximum number of radians to rotate.
         * @return: A new Quaternion.
         */
        static inline Quaternion RotateTowards(Quaternion from, Quaternion to,
            double maxRadiansDelta);

        /**
         * Returns a new quaternion interpolated between a and b, using spherical
         * linear interpolation. The variable t is clamped to the range [0-1]. The
         * resulting quaternion will be normalized.
         * @param a: The starting rotation.
         * @param b: The ending rotation.
         * @param t: The interpolation value.
         * @return: A new quaternion.
         */
        static inline Quaternion Slerp(Quaternion a, Quaternion b, double t);

        /**
         * Returns a new quaternion interpolated between a and b, using spherical
         * linear interpolation. The resulting quaternion will be normalized.
         * @param a: The starting rotation.
         * @param b: The ending rotation.
         * @param t: The interpolation value.
         * @return: A new quaternion.
         */
        static inline Quaternion SlerpUnclamped(Quaternion a, Quaternion b,
            double t);

        /**
         * Outputs the angle axis representation of the provided quaternion.
         * @param rotation: The input quaternion.
         * @param angle: The output angle.
         * @param axis: The output axis.
         */
        static inline void ToAngleAxis(Quaternion rotation, double& angle,
            Vector3& axis);

        /**
         * Returns the Euler angle representation of a rotation. The resulting
         * vector contains the rotations about the z, x and y axis, in that order.
         * @param rotation: The quaternion to convert.
         * @return: A new vector.
         */
        static inline Vector3 ToEuler(Quaternion rotation);

        /**
         * Operator overloading.
         */
        inline struct Quaternion& operator+=(const double rhs);
        inline struct Quaternion& operator-=(const double rhs);
        inline struct Quaternion& operator*=(const double rhs);
        inline struct Quaternion& operator/=(const double rhs);
        inline struct Quaternion& operator+=(const Quaternion rhs);
        inline struct Quaternion& operator-=(const Quaternion rhs);
        inline struct Quaternion& operator*=(const Quaternion rhs);
    };

    inline Quaternion operator-(Quaternion rhs);
    inline Quaternion operator+(Quaternion lhs, const double rhs);
    inline Quaternion operator-(Quaternion lhs, const double rhs);
    inline Quaternion operator*(Quaternion lhs, const double rhs);
    inline Quaternion operator/(Quaternion lhs, const double rhs);
    inline Quaternion operator+(const double lhs, Quaternion rhs);
    inline Quaternion operator-(const double lhs, Quaternion rhs);
    inline Quaternion operator*(const double lhs, Quaternion rhs);
    inline Quaternion operator/(const double lhs, Quaternion rhs);
    inline Quaternion operator+(Quaternion lhs, const Quaternion rhs);
    inline Quaternion operator-(Quaternion lhs, const Quaternion rhs);
    inline Quaternion operator*(Quaternion lhs, const Quaternion rhs);
    inline Vector3 operator*(Quaternion lhs, const Vector3 rhs);
    inline bool operator==(const Quaternion lhs, const Quaternion rhs);
    inline bool operator!=(const Quaternion lhs, const Quaternion rhs);



    /*******************************************************************************
     * Implementation
     */

    Quaternion::Quaternion() : X(0), Y(0), Z(0), W(1) {}
    Quaternion::Quaternion(double data[]) : X(data[0]), Y(data[1]), Z(data[2]),
        W(data[3]) {}
    Quaternion::Quaternion(Vector3 vector, double scalar) : X(vector.X),
        Y(vector.Y), Z(vector.Z), W(scalar) {}
    Quaternion::Quaternion(double x, double y, double z, double w) : X(x), Y(y),
        Z(z), W(w) {}


    Quaternion Quaternion::Identity() { return Quaternion(0, 0, 0, 1); }


    double Quaternion::Angle(Quaternion a, Quaternion b)
    {
        double dot = Dot(a, b);
        return acos(fmin(fabs(dot), 1)) * 2;
    }

    Quaternion Quaternion::Conjugate(Quaternion rotation)
    {
        return Quaternion(-rotation.X, -rotation.Y, -rotation.Z, rotation.W);
    }

    double Quaternion::Dot(Quaternion lhs, Quaternion rhs)
    {
        return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W;
    }

    Quaternion Quaternion::FromAngleAxis(double angle, Vector3 axis)
    {
        Quaternion q;
        double m = sqrt(axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z);
        double s = sin(angle / 2) / m;
        q.X = axis.X * s;
        q.Y = axis.Y * s;
        q.Z = axis.Z * s;
        q.W = cos(angle / 2);
        return q;
    }

    Quaternion Quaternion::FromEuler(Vector3 rotation)
    {
        return FromEuler(rotation.X, rotation.Y, rotation.Z);
    }

    Quaternion Quaternion::FromEuler(double x, double y, double z)
    {
        double cx = cos(x * 0.5);
        double cy = cos(y * 0.5);
        double cz = cos(z * 0.5);
        double sx = sin(x * 0.5);
        double sy = sin(y * 0.5);
        double sz = sin(z * 0.5);
        Quaternion q;
        q.X = cx * sy * sz + cy * cz * sx;
        q.Y = cx * cz * sy - cy * sx * sz;
        q.Z = cx * cy * sz - cz * sx * sy;
        q.W = sx * sy * sz + cx * cy * cz;
        return q;
    }

    Quaternion Quaternion::FromToRotation(Vector3 fromVector, Vector3 toVector)
    {
        double dot = Vector3::Dot(fromVector, toVector);
        double k = sqrt(Vector3::SqrMagnitude(fromVector) *
            Vector3::SqrMagnitude(toVector));
        if (fabs(dot / k + 1) < 0.00001)
        {
            Vector3 ortho = Vector3::Orthogonal(fromVector);
            return Quaternion(Vector3::Normalized(ortho), 0);
        }
        Vector3 cross = Vector3::Cross(fromVector, toVector);
        return Normalized(Quaternion(cross, dot + k));
    }

    Quaternion Quaternion::Inverse(Quaternion rotation)
    {
        double n = Norm(rotation);
        return Conjugate(rotation) / (n * n);
    }

    Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, double t)
    {
        if (t < 0) return Normalized(a);
        else if (t > 1) return Normalized(b);
        return LerpUnclamped(a, b, t);
    }

    Quaternion Quaternion::LerpUnclamped(Quaternion a, Quaternion b, double t)
    {
        Quaternion quaternion;
        if (Dot(a, b) >= 0)
            quaternion = a * (1 - t) + b * t;
        else
            quaternion = a * (1 - t) - b * t;
        return Normalized(quaternion);
    }

    Quaternion Quaternion::LookRotation(Vector3 forward)
    {
        return LookRotation(forward, Vector3(0, 1, 0));
    }

    Quaternion Quaternion::LookRotation(Vector3 forward, Vector3 upwards)
    {
        // Normalize inputs
        forward = Vector3::Normalized(forward);
        upwards = Vector3::Normalized(upwards);
        // Don't allow zero vectors
        if (Vector3::SqrMagnitude(forward) < SMALL_DOUBLE || Vector3::SqrMagnitude(upwards) < SMALL_DOUBLE)
            return Quaternion::Identity();
        // Handle alignment with up direction
        if (1 - fabs(Vector3::Dot(forward, upwards)) < SMALL_DOUBLE)
            return FromToRotation(Vector3::Forward(), forward);
        // Get orthogonal vectors
        Vector3 right = Vector3::Normalized(Vector3::Cross(upwards, forward));
        upwards = Vector3::Cross(forward, right);
        // Calculate rotation
        Quaternion quaternion;
        double radicand = right.X + upwards.Y + forward.Z;
        if (radicand > 0)
        {
            quaternion.W = sqrt(1.0 + radicand) * 0.5;
            double recip = 1.0 / (4.0 * quaternion.W);
            quaternion.X = (upwards.Z - forward.Y) * recip;
            quaternion.Y = (forward.X - right.Z) * recip;
            quaternion.Z = (right.Y - upwards.X) * recip;
        }
        else if (right.X >= upwards.Y && right.X >= forward.Z)
        {
            quaternion.X = sqrt(1.0 + right.X - upwards.Y - forward.Z) * 0.5;
            double recip = 1.0 / (4.0 * quaternion.X);
            quaternion.W = (upwards.Z - forward.Y) * recip;
            quaternion.Z = (forward.X + right.Z) * recip;
            quaternion.Y = (right.Y + upwards.X) * recip;
        }
        else if (upwards.Y > forward.Z)
        {
            quaternion.Y = sqrt(1.0 - right.X + upwards.Y - forward.Z) * 0.5;
            double recip = 1.0 / (4.0 * quaternion.Y);
            quaternion.Z = (upwards.Z + forward.Y) * recip;
            quaternion.W = (forward.X - right.Z) * recip;
            quaternion.X = (right.Y + upwards.X) * recip;
        }
        else
        {
            quaternion.Z = sqrt(1.0 - right.X - upwards.Y + forward.Z) * 0.5;
            double recip = 1.0 / (4.0 * quaternion.Z);
            quaternion.Y = (upwards.Z + forward.Y) * recip;
            quaternion.X = (forward.X + right.Z) * recip;
            quaternion.W = (right.Y - upwards.X) * recip;
        }
        return quaternion;
    }

    double Quaternion::Norm(Quaternion rotation)
    {
        return sqrt(rotation.X * rotation.X +
            rotation.Y * rotation.Y +
            rotation.Z * rotation.Z +
            rotation.W * rotation.W);
    }

    Quaternion Quaternion::Normalized(Quaternion rotation)
    {
        return rotation / Norm(rotation);
    }

    Quaternion Quaternion::RotateTowards(Quaternion from, Quaternion to,
        double maxRadiansDelta)
    {
        double angle = Quaternion::Angle(from, to);
        if (angle == 0)
            return to;
        maxRadiansDelta = fmax(maxRadiansDelta, angle - M_PI);
        double t = fmin(1, maxRadiansDelta / angle);
        return Quaternion::SlerpUnclamped(from, to, t);
    }

    Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, double t)
    {
        if (t < 0) return Normalized(a);
        else if (t > 1) return Normalized(b);
        return SlerpUnclamped(a, b, t);
    }

    Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, double t)
    {
        double n1;
        double n2;
        double n3 = Dot(a, b);
        bool flag = false;
        if (n3 < 0)
        {
            flag = true;
            n3 = -n3;
        }
        if (n3 > 0.999999)
        {
            n2 = 1 - t;
            n1 = flag ? -t : t;
        }
        else
        {
            double n4 = acos(n3);
            double n5 = 1 / sin(n4);
            n2 = sin((1 - t) * n4) * n5;
            n1 = flag ? -sin(t * n4) * n5 : sin(t * n4) * n5;
        }
        Quaternion quaternion;
        quaternion.X = (n2 * a.X) + (n1 * b.X);
        quaternion.Y = (n2 * a.Y) + (n1 * b.Y);
        quaternion.Z = (n2 * a.Z) + (n1 * b.Z);
        quaternion.W = (n2 * a.W) + (n1 * b.W);
        return Normalized(quaternion);
    }

    void Quaternion::ToAngleAxis(Quaternion rotation, double& angle, Vector3& axis)
    {
        if (rotation.W > 1)
            rotation = Normalized(rotation);
        angle = 2 * acos(rotation.W);
        double s = sqrt(1 - rotation.W * rotation.W);
        if (s < 0.00001) {
            axis.X = 1;
            axis.Y = 0;
            axis.Z = 0;
        }
        else {
            axis.X = rotation.X / s;
            axis.Y = rotation.Y / s;
            axis.Z = rotation.Z / s;
        }
    }

    Vector3 Quaternion::ToEuler(Quaternion rotation)
    {
        double sqw = rotation.W * rotation.W;
        double sqx = rotation.X * rotation.X;
        double sqy = rotation.Y * rotation.Y;
        double sqz = rotation.Z * rotation.Z;
        // If normalized is one, otherwise is correction factor
        double unit = sqx + sqy + sqz + sqw;
        double test = rotation.X * rotation.W - rotation.Y * rotation.Z;
        Vector3 v;
        // Singularity at north pole
        if (test > 0.4995f * unit)
        {
            v.Y = 2 * atan2(rotation.Y, rotation.X);
            v.X = M_PI_2;
            v.Z = 0;
            return v;
        }
        // Singularity at south pole
        if (test < -0.4995f * unit)
        {
            v.Y = -2 * atan2(rotation.Y, rotation.X);
            v.X = -M_PI_2;
            v.Z = 0;
            return v;
        }
        // Yaw
        v.Y = atan2(2 * rotation.W * rotation.Y + 2 * rotation.Z * rotation.X,
            1 - 2 * (rotation.X * rotation.X + rotation.Y * rotation.Y));
        // Pitch
        v.X = asin(2 * (rotation.W * rotation.X - rotation.Y * rotation.Z));
        // Roll
        v.Z = atan2(2 * rotation.W * rotation.Z + 2 * rotation.X * rotation.Y,
            1 - 2 * (rotation.Z * rotation.Z + rotation.X * rotation.X));
        return v;
    }

    struct Quaternion& Quaternion::operator+=(const double rhs)
    {
        X += rhs;
        Y += rhs;
        Z += rhs;
        W += rhs;
        return *this;
    }

    struct Quaternion& Quaternion::operator-=(const double rhs)
    {
        X -= rhs;
        Y -= rhs;
        Z -= rhs;
        W -= rhs;
        return *this;
    }

    struct Quaternion& Quaternion::operator*=(const double rhs)
    {
        X *= rhs;
        Y *= rhs;
        Z *= rhs;
        W *= rhs;
        return *this;
    }

    struct Quaternion& Quaternion::operator/=(const double rhs)
    {
        X /= rhs;
        Y /= rhs;
        Z /= rhs;
        W /= rhs;
        return *this;
    }

    struct Quaternion& Quaternion::operator+=(const Quaternion rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
        W += rhs.W;
        return *this;
    }

    struct Quaternion& Quaternion::operator-=(const Quaternion rhs)
    {
        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
        W -= rhs.W;
        return *this;
    }

    struct Quaternion& Quaternion::operator*=(const Quaternion rhs)
    {
        Quaternion q;
        q.W = W * rhs.W - X * rhs.X - Y * rhs.Y - Z * rhs.Z;
        q.X = X * rhs.W + W * rhs.X + Y * rhs.Z - Z * rhs.Y;
        q.Y = W * rhs.Y - X * rhs.Z + Y * rhs.W + Z * rhs.X;
        q.Z = W * rhs.Z + X * rhs.Y - Y * rhs.X + Z * rhs.W;
        *this = q;
        return *this;
    }

    Quaternion operator-(Quaternion rhs) { return rhs * -1; }
    Quaternion operator+(Quaternion lhs, const double rhs) { return lhs += rhs; }
    Quaternion operator-(Quaternion lhs, const double rhs) { return lhs -= rhs; }
    Quaternion operator*(Quaternion lhs, const double rhs) { return lhs *= rhs; }
    Quaternion operator/(Quaternion lhs, const double rhs) { return lhs /= rhs; }
    Quaternion operator+(const double lhs, Quaternion rhs) { return rhs += lhs; }
    Quaternion operator-(const double lhs, Quaternion rhs) { return rhs -= lhs; }
    Quaternion operator*(const double lhs, Quaternion rhs) { return rhs *= lhs; }
    Quaternion operator/(const double lhs, Quaternion rhs) { return rhs /= lhs; }
    Quaternion operator+(Quaternion lhs, const Quaternion rhs)
    {
        return lhs += rhs;
    }
    Quaternion operator-(Quaternion lhs, const Quaternion rhs)
    {
        return lhs -= rhs;
    }
    Quaternion operator*(Quaternion lhs, const Quaternion rhs)
    {
        return lhs *= rhs;
    }

    Vector3 operator*(Quaternion lhs, const Vector3 rhs)
    {
        Vector3 u = Vector3(lhs.X, lhs.Y, lhs.Z);
        double s = lhs.W;
        return u * (Vector3::Dot(u, rhs) * 2)
            + rhs * (s * s - Vector3::Dot(u, u))
            + Vector3::Cross(u, rhs) * (2.0 * s);
    }

    bool operator==(const Quaternion lhs, const Quaternion rhs)
    {
        return lhs.X == rhs.X &&
            lhs.Y == rhs.Y &&
            lhs.Z == rhs.Z &&
            lhs.W == rhs.W;
    }

    bool operator!=(const Quaternion lhs, const Quaternion rhs)
    {
        return !(lhs == rhs);
    }
}