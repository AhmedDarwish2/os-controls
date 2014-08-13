#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveGeometry.h"
#include "RenderState.h"

class Sphere : public PrimitiveBase {
public:

  Sphere();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  double m_Radius;
};

class Ellipsoid : public PrimitiveBase {

};

class Cylinder : public PrimitiveBase {
public:

  Cylinder();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  double m_Radius;
  double m_Height;
};

class Box : public PrimitiveBase {
public:

  Box();
  const Vector3& Size() const { return m_Size; }
  void SetSize(const Vector3& size) { m_Size = size; }

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  Vector3 m_Size;
};

class Disk : public PrimitiveBase {
public:

  Disk();
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  double m_Radius;
};

class Ellipse : public PrimitiveBase {

};

class RectanglePrim : public PrimitiveBase {
public:

  RectanglePrim();
  const Vector2& Size() const { return m_Size; }
  void SetSize(const Vector2& size) { m_Size = size; }

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  Vector2 m_Size;
};

class PartialDisk : public PrimitiveBase {
public:

  PartialDisk();

  double InnerRadius() const { return m_InnerRadius; }
  void SetInnerRadius(double innerRad) {
    if (m_InnerRadius != innerRad) {
      m_RecomputeGeometry = true;
    }
    m_InnerRadius = innerRad;
  }

  double OuterRadius() const { return m_OuterRadius; }
  void SetOuterRadius(double outerRad) {
    if (m_OuterRadius != outerRad) {
      m_RecomputeGeometry = true;
    }
    m_OuterRadius = outerRad;
  }

  double StartAngle() const { return m_StartAngle; }
  void SetStartAngle(double startAngleRadians) {
    if (m_StartAngle != startAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_StartAngle = startAngleRadians;
  }
  
  double EndAngle() const { return m_EndAngle; }
  void SetEndAngle(double endAngleRadians) {
    if (m_EndAngle != endAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_EndAngle = endAngleRadians;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  void RecomputeGeometry() const;

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;
  mutable bool m_RecomputeGeometry;

  double m_InnerRadius;
  double m_OuterRadius;
  double m_StartAngle;
  double m_EndAngle;
};
