#ifndef	__PARTICLE_DOMAIN_H
#define __PARTICLE_DOMAIN_H

#include "../util/vec3.h"

// How small the dot product must be to declare that a point is in a plane for Within().
const float PLANAR_EPSILON = 1e-3f; 
const float LINEAR_EPSILON = 1e-7f;

const float P_ONEOVER_RAND_MAX = (1.0f/((float) RAND_MAX));
inline float pRandf() { return ((float) rand())*P_ONEOVER_RAND_MAX; }
inline void pSRandf(int x) { srand(x); }

class ParticleDomain {
public:
	virtual ~ParticleDomain() {}

	virtual bool within(const Vec3f&) = 0;
	virtual Vec3f generatePoint() = 0;
	virtual float size() = 0;
};

class PDomainPoint : public ParticleDomain {
private:
	Vec3f p;
public:
	PDomainPoint(const Vec3f& pt) {
		p = pt;
	}

	bool within(const Vec3f& pt) {
		return (p == pt);
	}

	Vec3f generatePoint() {
		return p;
	}

	float size() {
		return 1.0f;
	}

	Vec3f getPoint() {
		return p;
	}
};

class PDomainLine : public ParticleDomain {
private:
	Vec3f p0, vec, normalizedVec;
	float length;
public:
	PDomainLine(const Vec3f& pt0, const Vec3f& pt1) {
		p0 = pt0;
		vec = pt1 - pt0;
	
		normalizedVec = vec;
		normalizedVec.normalize();
		length = vec.length();
	}

	//Return true if the point lies on the line
	bool within(const Vec3f& pt) {
		Vec3f to = pt - p0;
		float d = normalizedVec.dot(to);
        float dif = fabs(d - to.length()) / length;
        return dif < LINEAR_EPSILON; //Overcome floating point rounding errors.
	}

	//Returns a random point on the line.
	Vec3f generatePoint() {
		return p0 + vec*(1.0f*rand()/RAND_MAX);
	}

	float size() {
		return length;
	}
};

// Compute the inverse matrix of the plane basis.
static inline void NewBasis(const Vec3f &u, const Vec3f &v, Vec3f &s1, Vec3f &s2)
{
    Vec3f w = u.cross(v);

    float det = 1.0f / (w.z*u.x*v.y - w.z*u.y*v.x - u.z*w.x*v.y - u.x*v.z*w.y + v.z*w.x*u.y + u.z*v.x*w.y);

    s1 = Vec3f((v.y*w.z - v.z*w.y), (v.z*w.x - v.x*w.z), (v.x*w.y - v.y*w.x));
    s1 *= det;
    s2 = Vec3f((u.y*w.z - u.z*w.y), (u.z*w.x - u.x*w.z), (u.x*w.y - u.y*w.x));
    s2 *= -det;
}

class PDomainTriangle : public ParticleDomain {
private:
	Vec3f p0; //p1, p2;
	Vec3f v0, v1;
	float area;

public:
	PDomainTriangle(const Vec3f& pt0, const Vec3f& pt1, const Vec3f& pt2) {
		p0 = pt0;
		//p1 = pt1;
		//p2 = pt2;
		v0 = pt1 - pt0;
		v1 = pt2 - pt0;

		// 1/2 * |(p1-p0) X (p2-p0)| 
		area = 0.5f * (pt1 - pt0).cross(pt2 - pt0).length();
	}

	bool within(const Vec3f& pt) {
		Vec3f v2 = pt - p0;

		float dot00 = v0.dot(v0);
		float dot01 = v0.dot(v1);
		float dot02 = v0.dot(v2);
		float dot10 = v1.dot(v0);
		float dot11 = v1.dot(v1);
		float dot12 = v1.dot(v2);

		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return (u > 0) && (v > 0) && (u + v < 1);
	}

	//Returns a random point within the triangle.
	Vec3f generatePoint() {
		float rand1 = 1.0f * rand() / RAND_MAX;
		float rand2 = 1.0f * rand() / RAND_MAX;

		if (rand1+rand2 < 1.0f) {
			return v0*rand1 + v1*rand2;		
		}else {
			return v0*(1.0f-rand1) + v1*(1.0f-rand2);
		}
	}

	float size() {
		return area;	
	}
};

/// Rhombus-shaped planar region.
///
/// p0 is a point on the plane. u0 and v0 are (non-parallel) basis vectors in the plane. They don't need to be normal or orthogonal.
///
/// Generate returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v. Within returns true for points within epsilon of the patch.
class PDomainRectangle : public ParticleDomain
{
public:
    Vec3f p, u, v, uNrm, vNrm, nrm, s1, s2;
    float uLen, vLen, D, area;

public:
    PDomainRectangle(const Vec3f &p0, const Vec3f &u0, const Vec3f &v0)
    {
        p = p0;
        u = u0;
        v = v0;

		uLen = u.length();
        uNrm = u / uLen;
        vLen = v.length();
        vNrm = v / vLen;

		nrm =  uNrm.cross(vNrm);// This is the non-unit normal.
        nrm.normalize(); // Must normalize it.

		D = -p.dot(nrm);

        NewBasis(u, v, s1, s2); // Compute the inverse matrix of the plane basis.

        // Compute the area
		Vec3f Hgt = v - uNrm * uNrm.dot(v);
        float h = Hgt.length();
        area = uLen * h;
    }

    ~PDomainRectangle()
    {
    }

    bool within(const Vec3f &pos) const /// Returns true for points within epsilon of the patch.
    {
        Vec3f offset = pos - p;
		float d = offset.dot(nrm);

        if(d > PLANAR_EPSILON) return false;

        // Dot product with basis vectors of old frame
        // in terms of new frame gives position in uv frame.
		float upos = offset.dot(s1);
        float vpos = offset.dot(s2);

        // Did it cross plane outside triangle?
        return !(upos < 0 || upos > 1 || vpos < 0 || vpos > 1);
    }

    Vec3f generate() const /// Returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v.
    {
        Vec3f pos = p + u*pRandf() + v*pRandf();
        return pos;
    }

    float size() const
    {
        return area;
    }
};


    /// Arbitrarily-oriented disc
    ///
    /// The point Center is the center of a disc in the plane with normal Normal. The disc has an OuterRadius. If InnerRadius is greater than 0, the domain is a flat washer, rather than a disc. The normal will get normalized, so it need not already be unit length.
    ///
    /// Generate returns a point inside the disc shell. Within returns true for points within epsilon of the disc.
    class PDDisc : public pDomain
    {
    public:
        pVec p, nrm, u, v;
        float radIn, radOut, radInSqr, radOutSqr, dif, D, area;

    public:
        PDDisc(const pVec &Center, const pVec Normal, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            p = Center;
            nrm = Normal;
            nrm.normalize();

            if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
            if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

            if(OuterRadius > InnerRadius) {
                radOut = OuterRadius; radIn = InnerRadius;
            } else {
                radOut = InnerRadius; radIn = OuterRadius;
            }

            radInSqr = fsqr(radIn);
            radOutSqr = fsqr(radOut);
            dif = radOut - radIn;

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, nrm)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - nrm * dot(basis, nrm);
            u.normalize();
            v = Cross(nrm, u);
            D = -dot(p, nrm);

            if(dif == 0.0f)
                area = 2.0f * M_PI * radOut; // Length of circle
            else
                area = M_PI * radOutSqr - M_PI * radInSqr; // Area or disc minus hole
        }

        ~PDDisc()
        {
        }

        bool Within(const pVec &pos) const /// Returns true for points within epsilon of the disc.
        {
            pVec offset = pos - p;
            float d = dot(offset, nrm);

            if(d > P_PLANAR_EPSILON) return false;

            float len = offset.length2();
            return len >= radInSqr && len <= radOutSqr;
        }

        pVec Generate() const /// Returns a point inside the disc shell.
        {
            // Might be faster to generate a point in a square and reject if outside the circle
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around normal
            // Distance from center
            float r = radIn + pRandf() * dif;

            float x = r * cosf(theta); // Weighting of each frame vector
            float y = r * sinf(theta);

            pVec pos = p + u * x + v * y;
            return pos;
        }

        float Size() const
        {
            return 1.0f; // A plane is infinite, so what sensible thing can I return?
        }

        pDomain *copy() const
        {
            PDDisc *P = new PDDisc(*this);
            return P;
        }
    };

    /// Arbitrarily-oriented plane.
    ///
    /// The point p0 is a point on the plane. Normal is the normal vector of the plane. If you have a plane in a,b,c,d form remember that n = [a,b,c] and you can compute a suitable point p0 as p0 = -n*d. The normal will get normalized, so it need not already be unit length.
    ///
    /// Generate returns the point p0. Within returns true if the point is in the positive half-space of the plane (in the plane or on the side that Normal points to).
    class PDPlane : public pDomain
    {
    public:
        pVec p, nrm;
        float D;

    public:
        PDPlane(const pVec &p0, const pVec &Normal)
        {
            p = p0;
            nrm = Normal;
            nrm.normalize(); // Must normalize it.
            D = -dot(p, nrm);
        }

        ~PDPlane()
        {
        }

        // Distance from plane = n * p + d
        // Inside is the positive half-space.
        bool Within(const pVec &pos) const /// Returns true if the point is in the positive half-space of the plane (in the plane or on the side that Normal points to).
        {
            return dot(nrm, pos) >= -D;
        }

        // How do I sensibly make a point on an infinite plane?
        pVec Generate() const /// Returns the point p0.
        {
            return p;
        }

        float Size() const
        {
            return 1.0f; // A plane is infinite, so what sensible thing can I return?
        }

        pDomain *copy() const
        {
            PDPlane *P = new PDPlane(*this);
            return P;
        }
    };

    /// Axis-aligned box
    ///
    /// e0 and e1 are opposite corners of an axis-aligned box. It doesn't matter which of each coordinate is min and which is max.
    ///
    /// Generate returns a random point in this box. Within returns true if the point is in the box.
    ///
    /// It is only possible to bounce particles off the outside of the box, not the inside. Likewise, particles can only Avoid the box from the outside. To use the Avoid action inside a box, define the box as six planes.
    class PDBox : public pDomain
    {
    public:
        // p0 is the min corner. p1 is the max corner.
        pVec p0, p1, dif;
        float vol;

    public:
        PDBox(const pVec &e0, const pVec &e1)
        {
            p0 = e0;
            p1 = e1;
            if(e1.x() < e0.x()) { p0.x() = e1.x(); p1.x() = e1.x(); }
            if(e1.y() < e0.y()) { p0.y() = e1.y(); p1.y() = e1.y(); }
            if(e1.z() < e0.z()) { p0.z() = e1.z(); p1.z() = e1.z(); }

            dif = p1 - p0;
            vol = dot(dif, pVec(1,1,1));
        }

        ~PDBox()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is in the box.
        {
            return !((pos.x() < p0.x()) || (pos.x() > p1.x()) ||
                (pos.y() < p0.y()) || (pos.y() > p1.y()) ||
                (pos.z() < p0.z()) || (pos.z() > p1.z()));
        }

        pVec Generate() const /// Returns a random point in this box.
        {
            // Scale and translate [0,1] random to fit box
            return p0 + CompMult(pRandVec(), dif);
        }

        float Size() const
        {
            return vol;
        }

        pDomain *copy() const
        {
            PDBox *P = new PDBox(*this);
            return P;
        }
    };

    /// Cylinder
    ///
    /// e0 and e1 are the endpoints of the axis of the right cylinder. OuterRadius is the outer radius, and InnerRadius is the inner radius for a cylindrical shell. InnerRadius = 0 for a solid cylinder with no empty space in the middle.
    ///
    /// Generate returns a random point in the cylindrical shell. Within returns true if the point is within the cylindrical shell.
    class PDCylinder : public pDomain
    {
    public:
        pVec apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
        float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol;
        bool ThinShell;

    public:
        PDCylinder(const pVec &e0, const pVec &e1, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            apex = e0;
            axis = e1 - e0;

            if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
            if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }

            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;

            // Given an arbitrary nonzero vector n, make two orthonormal
            // vectors u and v forming a frame [u,v,n.normalize()].
            pVec n = axis;
            float axisLenSqr = axis.length2();
            float len = sqrtf(axisLenSqr);
            axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
            n *= sqrtf(axisLenInvSqr);

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, n)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - n * dot(basis, n);
            u.normalize();
            v = Cross(n, u);

            float EndCapArea = M_PI * radOutSqr - M_PI * radInSqr;
            if(ThinShell)
                vol = len * 2.0f * M_PI * radOut; // Surface area of cylinder
            else
                vol = EndCapArea * len; // Volume of cylindrical shell
        }

        ~PDCylinder()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is within the cylindrical shell.
        {
            // This is painful and slow. Might be better to do quick accept/reject tests.
            // Axis is vector from base to tip of the cylinder.
            // x is vector from base to pos.
            //         x . axis
            // dist = ---------- = projected distance of x along the axis
            //        axis. axis   ranging from 0 (base) to 1 (tip)
            //
            // rad = x - dist * axis = projected vector of x along the base

            pVec x = pos - apex;

            // Check axial distance
            float dist = dot(axis, x) * axisLenInvSqr;
            if(dist < 0.0f || dist > 1.0f)
                return false;

            // Check radial distance
            pVec xrad = x - axis * dist; // Radial component of x
            float rSqr = xrad.length2();

            return (rSqr >= radInSqr && rSqr <= radOutSqr);
        }

        pVec Generate() const /// Returns a random point in the cylindrical shell.
        {
            float dist = pRandf(); // Distance between base and tip
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around axis
            // Distance from axis
            float r = radIn + pRandf() * radDif;

            // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
            float x = r * cosf(theta);
            float y = r * sinf(theta);

            pVec pos = apex + axis * dist + u * x + v * y;
            return pos;
        }

        float Size() const/// Returns the thick cylindrical shell volume or the thin cylindrical shell area if OuterRadius==InnerRadius.
        {
            return vol;
        }

        pDomain *copy() const
        {
            PDCylinder *P = new PDCylinder(*this);
            return P;
        }
    };

    ///  Cone
    ///
    /// e0 is the apex of the cone and e1 is the endpoint of the axis at the cone's base. OuterRadius is the radius of the base of the cone. InnerRadius is the radius of the base of a cone to subtract from the first cone to create a conical shell. This is similar to the cylindrical shell, which can be thought of as a large cylinder with a smaller cylinder subtracted from the middle. Both cones share the same apex and axis, which implies that the thickness of the conical shell tapers to 0 at the apex. InnerRadius = 0 for a solid cone with no empty space in the middle.
    ///
    /// Generate returns a random point in the conical shell. Within returns true if the point is within the conical shell.
    class PDCone : public pDomain
    {
    public:
        pVec apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
        float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol;
        bool ThinShell;

    public:
        PDCone(const pVec &e0, const pVec &e1, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            apex = e0;
            axis = e1 - e0;

            if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
            if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }

            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;

            // Given an arbitrary nonzero vector n, make two orthonormal
            // vectors u and v forming a frame [u,v,n.normalize()].
            pVec n = axis;
            float axisLenSqr = axis.length2();
            float len = sqrtf(axisLenSqr);
            axisLenInvSqr = axisLenSqr ? 1.0f / axisLenSqr : 0.0f;
            n *= sqrtf(axisLenInvSqr);

            // Find a vector orthogonal to n.
            pVec basis(1.0f, 0.0f, 0.0f);
            if (fabsf(dot(basis, n)) > 0.999f)
                basis = pVec(0.0f, 1.0f, 0.0f);

            // Project away N component, normalize and cross to get
            // second orthonormal vector.
            u = basis - n * dot(basis, n);
            u.normalize();
            v = Cross(n, u);

            if(ThinShell)
                vol = sqrtf(axisLenSqr + radOutSqr) * M_PI * radOut; // Surface area of cone (not counting endcap)
            else {
                float OuterVol = 0.33333333f * M_PI * radOutSqr * len; 
                float InnerVol = 0.33333333f * M_PI * radInSqr * len;
                vol = OuterVol - InnerVol; // Volume of conical shell
            }
        }

        ~PDCone()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point is within the conical shell.
        {
            // This is painful and slow. Might be better to do quick
            // accept/reject tests.
            // Let axis = vector from base to tip of the cone
            // x = vector from base to test point
            //         x . axis
            // dist = ---------- = projected distance of x along the axis
            //        axis. axis   ranging from 0 (base) to 1 (tip)
            //
            // rad = x - dist * axis = projected vector of x along the base

            pVec x = pos - apex;

            // Check axial distance
            // axisLenInvSqr stores 1 / dot(axis, axis)
            float dist = dot(axis, x) * axisLenInvSqr;
            if(dist < 0.0f || dist > 1.0f)
                return false;

            // Check radial distance; scale radius along axis for cones
            pVec xrad = x - axis * dist; // Radial component of x
            float rSqr = xrad.length2();

            return (rSqr >= fsqr(dist * radIn) && rSqr <= fsqr(dist * radOut));
        }

        pVec Generate() const /// Returns a random point in the conical shell.
        {
            float dist = pRandf(); // Distance between base and tip
            float theta = pRandf() * 2.0f * float(M_PI); // Angle around axis
            // Distance from axis
            float r = radIn + pRandf() * radDif;

            // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
            float x = r * cosf(theta);
            float y = r * sinf(theta);

            // Scale radius along axis for cones
            x *= dist;
            y *= dist;

            pVec pos = apex + axis * dist + u * x + v * y;
            return pos;
        }

        float Size() const /// Returns the thick conical shell volume or the thin conical shell area if OuterRadius==InnerRadius.
        {
            return vol;
        }

        pDomain *copy() const
        {
            PDCone *P = new PDCone(*this);
            return P;
        }
    };

    /// Sphere
    ///
    /// The point Center is the center of the sphere. OuterRadius is the outer radius of the spherical shell and InnerRadius is the inner radius.
    ///
    /// Generate returns a random point in the thick shell at a distance between OuterRadius and InnerRadius from point Center. If InnerRadius is 0, then it is the whole sphere. Within returns true if the point lies within the thick shell at a distance between InnerRadius to OuterRadius from point Center.
    class PDSphere : public pDomain
    {
    public:
        pVec ctr;
        float radOut, radIn, radOutSqr, radInSqr, radDif, vol;
        bool ThinShell;

    public:
        PDSphere(const pVec &Center, const float OuterRadius, const float InnerRadius = 0.0f)
        {
            ctr = Center;

            if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
            if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

            if(OuterRadius < InnerRadius) {
                radOut = InnerRadius; radIn = OuterRadius;
            } else {
                radOut = OuterRadius; radIn = InnerRadius;
            }

            radOutSqr = fsqr(radOut);
            radInSqr = fsqr(radIn);

            ThinShell = (radIn == radOut);
            radDif = radOut - radIn;

            if(ThinShell)
                vol = 4.0f * M_PI * radOutSqr; // Surface area of sphere
            else {
                float OuterVol = 1.33333333f * M_PI * radOutSqr * radOut; 
                float InnerVol = 1.33333333f * M_PI * radInSqr * radIn;
                vol = OuterVol - InnerVol; // Volume of spherical shell
            }
        }

        ~PDSphere()
        {
        }

        bool Within(const pVec &pos) const /// Returns true if the point lies within the thick shell.
        {
            pVec rvec(pos - ctr);
            float rSqr = rvec.length2();
            return rSqr <= radOutSqr && rSqr >= radInSqr;
        }

        pVec Generate() const /// Returns a random point in the thick spherical shell.
        {
            pVec pos;

            do {
                pos = pRandVec() - vHalf; // Point on [-0.5,0.5] box
            } while (pos.length2() > fsqr(0.5)); // Make sure it's also on r=0.5 sphere.
            pos.normalize(); // Now it's on r=1 spherical shell

            // Scale unit sphere pos by [0..r] and translate
            if(ThinShell) // I can't remember why I make this distinction. Is it for precision or speed? Speed doesn't seem likely.
                pos = ctr + pos * radOut;
            else
                pos = ctr + pos * (radIn + pRandf() * radDif);

            return pos;
        }

        float Size() const /// Returns the thick spherical shell volume or the thin spherical shell area if OuterRadius==InnerRadius.
        {
            return vol;
        }

        pDomain *copy() const
        {
            PDSphere *P = new PDSphere(*this);
            return P;
        }
    };

    /// Gaussian blob
    ///
    /// The point Center is the center of a normal probability density of standard deviation StandardDev. The density is radially symmetrical. The blob domain allows for some very natural-looking effects because there is no sharp, artificial-looking boundary at the edge of the domain.
    ///
    /// Generate returns a point with normal probability density. Within has a probability of returning true equal to the probability density at the specified point.
    class PDBlob : public pDomain
    {
    public:
        pVec ctr;
        float stdev, Scale1, Scale2;

    public:
        PDBlob(const pVec &Center, const float StandardDev)
        {
            ctr = Center;
            stdev = StandardDev;
            float oneOverSigma = 1.0f/(stdev+0.000000000001f);
            Scale1 = -0.5f*fsqr(oneOverSigma);
            Scale2 = P_ONEOVERSQRT2PI * oneOverSigma;
        }

        ~PDBlob()
        {
        }

        bool Within(const pVec &pos) const /// Has a probability of returning true equal to the probability density at the specified point.
        {
            pVec x = pos - ctr;
            float Gx = expf(x.length2() * Scale1) * Scale2;
            return (pRandf() < Gx);
        }

        pVec Generate() const /// Returns a point with normal probability density.
        {
            return ctr + pNRandVec(stdev);
        }

        float Size() const /// Returns the probability density integral, which is 1.0.
        {
            return 1.0f;
        }

        pDomain *copy() const
        {
            PDBlob *P = new PDBlob(*this);
            return P;
        }
    };

};

#endif