// Bradley Christensen - 2022-2023
#pragma once



struct IntVec2;
struct Vec3;



//----------------------------------------------------------------------------------------------------------------------
// Vec2
//
// A couple of floats, representing a position or change of position in 2D space.
//
struct Vec2
{
public:
    
    float x = 0.f;
    float y = 0.f;

public:
    
    Vec2() = default;
    Vec2(float x, float y);
    explicit Vec2(int x, int y);
    explicit Vec2(IntVec2 const& fromIntVec2);
    explicit Vec2(Vec3 const& fromVec3);

public:
    
    float GetLength() const;
    float GetLengthSquared() const;
    float GetAngleDegrees() const;
    void Normalize();
    Vec2 GetNormalized() const;
    Vec2 GetRotated90() const;
    Vec2 GetRotatedMinus90() const;
    Vec2 GetRotated(float degrees) const;
    Vec2 GetProjectedOntoNormal(Vec2 const& normal) const;
    Vec2 GetProjectedOnto(Vec2 const& vector) const;
    Vec2 GetFloor() const;
    float GetDistanceTo(Vec2 const& other) const;
    float GetDistanceSquaredTo(Vec2 const& other) const;
    
    void Rotate90();
    void RotateMinus90();
    void Rotate(float degrees);
    void SetLength(float length);
    void ClampLength(float maxLength);
    void ClampLengthMin(float minLength);
    void ClampLength(float minLength, float maxLength);
    void Floor();

    static Vec2 MakeFromUnitCircleDegrees(float angleDegrees);
    static Vec2 MakeFromPolarCoords(float angleDegrees, float radius);

    // Component-wise const operators
    Vec2 operator-() const;

    Vec2 operator+(Vec2 const& other) const;
    Vec2 operator-(Vec2 const& other) const;
    Vec2 operator*(Vec2 const& other) const;
    Vec2 operator/(Vec2 const& other) const;

    Vec2 operator*(float multiplier) const;
    Vec2 operator/(float divisor) const;

    // Component-wise self changing operators
    void operator+=(Vec2 const& other);
    void operator-=(Vec2 const& other);
    void operator*=(Vec2 const& other);
    void operator/=(Vec2 const& other);

    void operator*=(float multiplier);
    void operator/=(float divisor);
    
    bool operator==(Vec2 const& rhs) const;
    bool operator!=(Vec2 const& rhs) const;

    friend Vec2 const operator*(float uniformScale, Vec2 const& vecToScale);

public:
    
    // Commonly used Vec2's
    static Vec2 ZeroVector;
    static Vec2 ZeroToOne;
};
