#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveGeometry.h"
#include "RenderState.h"

class GLTexture2;

class GenericShape : public PrimitiveBase {
public:

  GenericShape(GLenum drawMode = GL_TRIANGLES) : m_drawMode(drawMode) { }
  virtual ~GenericShape () { }

  // Make sure to call UploadDataToBuffers on the geometry object before drawing.
  PrimitiveGeometry &Geometry () { return m_geometry; }

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  mutable PrimitiveGeometry m_geometry;
  GLenum m_drawMode;
};

class Sphere : public PrimitiveBase {
public:

  Sphere();
  virtual ~Sphere () { }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
};

class Cylinder : public PrimitiveBase {
public:

  Cylinder();
  virtual ~Cylinder () { }
  
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
  double m_Height;
};

class Box : public PrimitiveBase {
public:

  Box();
  virtual ~Box() { }
  
  const EigenTypes::Vector3& Size() const { return m_Size; }
  void SetSize(const EigenTypes::Vector3& size) { m_Size = size; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  EigenTypes::Vector3 m_Size;
};

class Disk : public PrimitiveBase {
public:

  Disk();
  virtual ~Disk () { }
  
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
};

class RectanglePrim : public PrimitiveBase {
public:

  RectanglePrim();
  virtual ~RectanglePrim() { }
  
  const EigenTypes::Vector2& Size() const { return m_Size; }
  void SetSize(const EigenTypes::Vector2& size) { m_Size = size; }

  const std::shared_ptr<GLTexture2> &Texture () const { return m_texture; }
  void SetTexture (const std::shared_ptr<GLTexture2> &texture) { m_texture = texture; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  EigenTypes::Vector2 m_Size;
  std::shared_ptr<GLTexture2> m_texture;
};

// This is a textured RectanglePrim which sets its aspect ratio based on the texture.
// It also sets its x/y scale to the image width/height in pixels.
class ImagePrimitive : public RectanglePrim {
public:
  
  ImagePrimitive(void);
  ImagePrimitive(const std::shared_ptr<GLTexture2> &texture);
  virtual ~ImagePrimitive() { }
  
  void SetScaleBasedOnTextureSize ();
};

class PartialDisk : public PrimitiveBase {
public:

  PartialDisk();
  virtual ~PartialDisk() { }

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

protected:

  virtual void DrawContents(RenderState& renderState) const override;

protected:

  virtual void RecomputeGeometry() const;

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;

  mutable bool m_RecomputeGeometry;

  double m_InnerRadius;
  double m_OuterRadius;
  double m_StartAngle;
  double m_EndAngle;
};

class PartialDiskWithTriangle : public PartialDisk {
public:

  PartialDiskWithTriangle();
  
  enum TriangleSide { INSIDE, OUTSIDE };

  void SetTriangleSide(TriangleSide side) {
    if (m_TriangleSide != side) {
      m_RecomputeGeometry = true;
    }
    m_TriangleSide = side;
  }

  void SetTrianglePosition(double pos) {
    if (m_TrianglePosition != pos) {
      m_RecomputeGeometry = true;
    }
    m_TrianglePosition = pos;
  }

  void SetTriangleWidth(double width) {
    if (m_TriangleWidth != width) {
      m_RecomputeGeometry = true;
    }
    m_TriangleWidth = width;
  }

  void SetTriangleOffset(double offset) {
    if (m_TriangleOffset != offset) {
      m_RecomputeGeometry = true;
    }
    m_TriangleOffset = offset;
  }

protected:

  virtual void RecomputeGeometry() const override;

  TriangleSide m_TriangleSide;
  double m_TrianglePosition;
  double m_TriangleWidth;
  double m_TriangleOffset;
};

class PartialSphere : public PrimitiveBase {
public:
  PartialSphere();
  virtual ~PartialSphere() { }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double StartWidthAngle() const { return m_StartWidthAngle; }
  void SetStartWidthAngle(double startAngleRadians) {
    if (m_StartWidthAngle != startAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_StartWidthAngle = startAngleRadians;
  }

  double EndWidthAngle() const { return m_EndWidthAngle; }
  void SetEndWidthAngle(double endAngleRadians) {
    if (m_EndWidthAngle != endAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_EndWidthAngle = endAngleRadians;
  }

  double StartHeightAngle() const { return m_StartHeightAngle; }
  void SetStartHeightAngle(double startAngleRadians) {
    if (m_StartHeightAngle != startAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_StartHeightAngle = startAngleRadians;
  }

  double EndHeightAngle() const { return m_EndHeightAngle; }
  void SetEndHeightAngle(double endAngleRadians) {
    if (m_EndHeightAngle != endAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_EndHeightAngle = endAngleRadians;
  }

  virtual void MakeAdditionalModelViewTransformations(ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;
  virtual void RecomputeGeometry() const;

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;

  mutable bool m_RecomputeGeometry;
  double m_Radius;
  double m_StartHeightAngle;
  double m_EndHeightAngle;
  double m_StartWidthAngle;
  double m_EndWidthAngle;
};

class CapsulePrim : public PrimitiveBase {
public:
  CapsulePrim();
  virtual ~CapsulePrim() { }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
  double m_Height;
};

class BiCapsulePrim : public PrimitiveBase {
public:
  BiCapsulePrim();
  virtual ~BiCapsulePrim() { }

  double Radius1() const { return m_Radius1; }
  void SetRadius1(double radius) {
    if (m_Radius1 != radius) {
      m_RecomputeGeometry = true;
    }
    m_Radius1 = radius;
  }

  double Radius2() const {return m_Radius2; }
  void SetRadius2(double radius) {
    if (m_Radius2 != radius) {
      m_RecomputeGeometry = true;
    }
    m_Radius2 = radius;
  }

  double Height() const { return m_Height; }
  void SetHeight(double height) {
    if (m_Height != height) {
      m_RecomputeGeometry = true;
    }
    m_Height = height;
  }

protected:

  virtual void DrawContents(RenderState& renderState) const override;
  virtual void RecomputeGeometry() const;

private:

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Cap1;
  mutable PrimitiveGeometry m_Cap2;
  mutable PrimitiveGeometry m_Body;

  mutable bool m_RecomputeGeometry;
  double m_Radius1;
  double m_Radius2;
  double m_Height;

  mutable double m_BodyRadius1;
  mutable double m_BodyRadius2;
  mutable double m_BodyOffset1;
  mutable double m_BodyOffset2;
};

class PartialCylinder : public PrimitiveBase {
public:
  PartialCylinder();
  virtual ~PartialCylinder() { }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

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

  virtual void MakeAdditionalModelViewTransformations(ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;
  virtual void RecomputeGeometry() const;

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;

  mutable bool m_RecomputeGeometry;
  double m_Radius;
  double m_Height;
  double m_StartAngle;
  double m_EndAngle;
};

class RadialPolygonPrim : public PrimitiveBase {
public:
  RadialPolygonPrim();
  virtual ~RadialPolygonPrim() { }

  void SetNumSides(size_t numSides) {
    if (numSides != m_Sides.size()) {
      m_Sides.resize(numSides);
      m_RecomputeGeometry = true;
    }
  }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  void SetPoint(size_t idx, const EigenTypes::Vector2& point) {
    assert(idx < m_Sides.size());
    static const double CLOSENESS_THRESH = 0.0001;
    const EigenTypes::Vector3 newPoint(point.x(), 0, point.y());
    if ((newPoint - m_Sides[idx].m_Origin).squaredNorm() > CLOSENESS_THRESH) {
      m_Sides[idx].m_Origin = newPoint;
      m_RecomputeGeometry = true;
    }
  }

protected:

  virtual void DrawContents(RenderState& renderState) const override;
  virtual void RecomputeGeometry() const;

private:

  struct PerSideInfo {
    PerSideInfo() :
      m_Origin(EigenTypes::Vector3::Zero()),
      m_SphereBasis(EigenTypes::Matrix3x3::Identity()),
      m_CylinderBasis(EigenTypes::Matrix3x3::Identity()),
      m_Length(1)
    { }
    EigenTypes::Vector3 m_Origin;
    EigenTypes::Matrix3x3 m_SphereBasis;
    EigenTypes::Matrix3x3 m_CylinderBasis;
    double m_Length;
    PrimitiveGeometry m_SphereJoint;
  };

  mutable PrimitiveGeometry m_CylinderBody;
  mutable std::vector<PerSideInfo, Eigen::aligned_allocator<PerSideInfo>> m_Sides;
  mutable PrimitiveGeometry m_Polygon;

  mutable bool m_RecomputeGeometry;
  double m_Radius;
};
