#ifndef SplineVector3D_H
#define SplineVector3D_H

class SplineVector3D
{
public:
	SplineVector3D();
	SplineVector3D(double x, double y, double z);

	double x() const;
	double y() const;
	double z() const;

	SplineVector3D &operator+=(const SplineVector3D &v);
    SplineVector3D &operator-=(const SplineVector3D &v);
    SplineVector3D &operator*=(double s);
    SplineVector3D &operator/=(double s);

	friend inline const SplineVector3D operator+(const SplineVector3D &left, const SplineVector3D &right);
    friend inline const SplineVector3D operator-(const SplineVector3D &left, const SplineVector3D &right);
    friend inline const SplineVector3D operator*(double s, const SplineVector3D &v);
    friend inline const SplineVector3D operator*(const SplineVector3D &v, double s);
    friend inline const SplineVector3D operator-(const SplineVector3D &v);
    friend inline const SplineVector3D operator/(const SplineVector3D &v, double s);

    friend inline bool operator==(const SplineVector3D &left, const SplineVector3D &right);
    friend inline bool operator!=(const SplineVector3D &left, const SplineVector3D &right);

	double length() const;
    double lengthSquared() const;

    SplineVector3D normalized() const;

    static double dotProduct(const SplineVector3D& left, const SplineVector3D& right);

private:
	double xp;
	double yp;
	double zp;
};

inline SplineVector3D::SplineVector3D()
	:xp(0.0), yp(0.0), zp(0.0)
{}

inline SplineVector3D::SplineVector3D(double xpos, double ypos, double zpos)
	:xp(xpos), yp(ypos), zp(zpos)
{}

inline double SplineVector3D::x() const { return xp; }
inline double SplineVector3D::y() const { return yp; }
inline double SplineVector3D::z() const { return zp; }

inline SplineVector3D &SplineVector3D::operator+=(const SplineVector3D &v)
{
    xp += v.xp;
    yp += v.yp;
    zp += v.zp;
    return *this;
}

inline SplineVector3D &SplineVector3D::operator-=(const SplineVector3D &v)
{
    xp -= v.xp;
    yp -= v.yp;
    zp -= v.zp;
    return *this;
}

inline SplineVector3D &SplineVector3D::operator*=(double s)
{
    xp *= s;
    yp *= s;
    zp *= s;
    return *this;
}

inline SplineVector3D &SplineVector3D::operator/=(double s)
{
    xp /= s;
    yp /= s;
    zp /= s;
    return *this;
}

inline const SplineVector3D operator+(const SplineVector3D &left, const SplineVector3D &right)
{
    return SplineVector3D(left.xp + right.xp, left.yp + right.yp, left.zp + right.zp);
}

inline const SplineVector3D operator-(const SplineVector3D &left, const SplineVector3D &right)
{
    return SplineVector3D(left.xp - right.xp, left.yp - right.yp, left.zp - right.zp);
}

inline const SplineVector3D operator*(double s, const SplineVector3D &v)
{
    return SplineVector3D(v.xp * s, v.yp * s, v.zp * s);
}

inline const SplineVector3D operator*(const SplineVector3D &v, double s)
{
    return SplineVector3D(v.xp * s, v.yp * s, v.zp * s);
}

inline const SplineVector3D operator-(const SplineVector3D &v)
{
    return SplineVector3D(-v.xp, -v.yp, -v.zp);
}

inline const SplineVector3D operator/(const SplineVector3D &v, double s)
{
    return SplineVector3D(v.xp / s, v.yp / s, v.zp / s);
}

inline bool operator==(const SplineVector3D &left, const SplineVector3D &right)
{
	return left.xp == right.xp
		&& left.yp == right.yp
		&& left.zp == right.zp;
}

inline bool operator!=(const SplineVector3D &left, const SplineVector3D &right)
{
	return left.xp != right.xp
		|| left.yp != right.yp
		|| left.zp != right.zp;
}


#endif // SplineVector3D_H
