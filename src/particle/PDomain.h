/// PDomain.h
/// This file defines the pDomain class and all of the classes that derive from it.

#ifndef _pdomain_h
#define _pdomain_h

#include "../util/vec3.h"
#include <vector>

const float PI = 3.14159f;
const float P_SQRT2PI = 2.506628274631000502415765284811045253006f;
const float P_ONEOVERSQRT2PI = (1.f / P_SQRT2PI);

inline float fsqr(float f) { return f * f; }

const float P_ONEOVER_RAND_MAX = (1.0f/((float) RAND_MAX));
inline float pRandf() { return ((float) rand())*P_ONEOVER_RAND_MAX; }
inline void pSRandf(int x) { srand(x); }


const float P_PLANAR_EPSILON = 1e-3f; ///< How small the dot product must be to declare that a point is in a plane for Within().

/// A representation of a region of space.
///
/// A Domain is a representation of a region of space. For example, the Source action uses a domain to describe the volume in which a particle will be created. A random point within the domain is chosen as the initial position of the particle. The Avoid, Sink and Bounce actions, for example, use domains to describe a volume in space for particles to steer around, die when they enter, or bounce off, respectively.
///
/// Domains can be used to describe velocities. Picture the velocity vector as having its tail at the origin and its tip being in the domain. Domains can be used to describe colors in any three-valued color space. They can be used to describe three-valued sizes, such as Length, Width, Height.
///
/// Several types of domains can be specified, such as points, lines, planes, discs, spheres, gaussian blobs, etc. Each subclass of the pDomain class represents a different kind of domain.
///
/// All domains support two basic operations. The first is Generate, which returns a random point in the domain.
///
/// The second basic operation is Within, which tells whether a given point is within the domain.
///
/// The application programmer never calls the Generate or Within functions. The application will use the pDomain class and its derivatives solely as a way to communicate the domain to the API. The API's action commands will then perform operations on the domain, such as generating particles within it.
class pDomain
{
public:
    virtual bool Within(const Vec3f &) const = 0; ///< Returns true if the given point is within the domain.
    virtual Vec3f Generate() const = 0; ///< Returns a random point in the domain.
    virtual float Size() const = 0; ///< Returns the size of the domain (length, area, or volume).

    virtual pDomain *copy() const = 0; // Returns a pointer to a heap-allocated copy of the derived class

    virtual ~pDomain() {}
};

/// A CSG union of multiple domains.
///
/// A point is within this domain if it is within domain A OR B.
/// Generate returns a point from either domain.
/// Within returns true if pos is within A or within B.
///
/// All domains have a Size() that is used to apportion probability between the domains in the union. Sizes of domains of the same dimensionality are commensurate but sizes of differing dimensionality are not.
/// Thus, to properly distribute probability of Generate() choosing each domain, it is wise to only combine domains that have the same dimensionality. Note that thin shelled cylinders, cones, and spheres, where InnerRadius==OuterRadius, are considered 2D, not 3D. Thin shelled discs (circles) are considered 1D.
class PDUnion : public pDomain
{
public:
    std::vector<pDomain *> Doms;
    float TotalSize;

public:
    PDUnion() /// Use this one to create an empty PDUnion then call .insert() to add each item to it.
    {
        TotalSize = 0.0f;
    }

    PDUnion(const pDomain &A, const pDomain &B)
    {
        TotalSize = A.Size() + B.Size();
        Doms.push_back(A.copy());
        Doms.push_back(B.copy());
    }

    PDUnion(const pDomain &A, const pDomain &B, const pDomain &C)
    {
        TotalSize = A.Size() + B.Size() + C.Size();
        Doms.push_back(A.copy());
        Doms.push_back(B.copy());
        Doms.push_back(C.copy());
    }

    /// Makes a copy of all the subdomains and point to the copies.
    ///
    /// Note that the Generate() function gets a performance boost if you supply DomList with the largest domains first.
    PDUnion(const std::vector<pDomain *> &DomList)
    {
        TotalSize = 0.0f;
        for(std::vector<pDomain *>::const_iterator it = DomList.begin(); it != DomList.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize += (*it)->Size();
        }
    }

    // Makes a copy of all the subdomains and point to the copies.
    PDUnion(const PDUnion &P)
    {
        TotalSize = 0.0f;
        for(std::vector<pDomain *>::const_iterator it = P.Doms.begin(); it != P.Doms.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize += (*it)->Size();
        }
    }

    ~PDUnion()
    {
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            delete (*it);
        }
    }

    /// Insert another domain into this PDUnion.
    void insert(const pDomain &A)
    {
        TotalSize += A.Size();
        Doms.push_back(A.copy());
    }

    bool Within(const Vec3f &pos) const /// Returns true if pos is within any of the domains.
    {
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++)
            if((*it)->Within(pos)) return true;
        return false;
    }

    Vec3f Generate() const /// Generate a point in any subdomain, chosen by the ratio of their sizes.
    {
        float Choose = pRandf() * TotalSize, PastProb = 0.0f;
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            PastProb += (*it)->Size();
            if(Choose <= PastProb)
                return (*it)->Generate();
        }

        return Vec3f(0,0,0); //Should never get here
    }

    float Size() const
    {
        return TotalSize;
    }

    pDomain *copy() const
    {
        PDUnion *P = new PDUnion(*this);
        return P;
    }
};

///START INTERSECTION
class PDIntersection : public pDomain
{
public:
    std::vector<pDomain *> Doms;
    float TotalSize;

public:
    PDIntersection() /// Use this one to create an empty PDIntersection then call .insert() to add each item to it.
    {
        TotalSize = 0.0f;
    }

    PDIntersection(const pDomain &A, const pDomain &B)
    {
        TotalSize = A.Size() + B.Size();
        Doms.push_back(A.copy());
        Doms.push_back(B.copy());
    }

    PDIntersection(const pDomain &A, const pDomain &B, const pDomain &C)
    {
        TotalSize = A.Size() + B.Size() + C.Size();
        Doms.push_back(A.copy());
        Doms.push_back(B.copy());
        Doms.push_back(C.copy());
    }

    /// Makes a copy of all the subdomains and point to the copies.
    ///
    /// Note that the Generate() function gets a performance boost if you supply DomList with the largest domains first.
    PDIntersection(const std::vector<pDomain *> &DomList)
    {
        TotalSize = 0.0f;
        for(std::vector<pDomain *>::const_iterator it = DomList.begin(); it != DomList.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize += (*it)->Size();
        }
    }

    // Makes a copy of all the subdomains and point to the copies.
    PDIntersection(const PDIntersection &P)
    {
        TotalSize = 0.0f;
        for(std::vector<pDomain *>::const_iterator it = P.Doms.begin(); it != P.Doms.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize += (*it)->Size();
        }
    }

    ~PDIntersection()
    {
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            delete (*it);
        }
    }

    /// Insert another domain into this PDIntersection.
    void insert(const pDomain &A)
    {
        TotalSize += A.Size();
        Doms.push_back(A.copy());
    }

    bool Within(const Vec3f &pos) const /// Returns true if pos is within all of the domains.
    {
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            if(!(*it)->Within(pos)) {
                return false;
            }
        }
        return true;
    }

    Vec3f Generate() const /// Generate a point in all of the subdomains.
    {
        float Choose = pRandf() * TotalSize, PastProb = 0.0f;
        pDomain* domain;
        Vec3f point;
        bool flag;

        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            PastProb += (*it)->Size();
            if(Choose <= PastProb) {
                domain = (*it);
            }
        }

        do {
            flag = true;
            point = domain->Generate();
            
            for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
                if (!(*it)->Within(point)) {
                    flag = false;
                }
            }

        }while(!flag);

        return point;
    }

    float Size() const
    {
        return TotalSize;
    }

    pDomain *copy() const
    {
        PDIntersection *P = new PDIntersection(*this);
        return P;
    }
};

///END INTERSECTION


/// START SUBTRACTIVE
//To create the domain, specify a base domain and then 0 to n domains that subtract from the base.
class PDSubtractive : public pDomain
{
public:
    std::vector<pDomain *> Doms;
    pDomain* base;
    float TotalSize;

public:
    PDSubtractive() /// Use this one to create an empty PDSubtractive then call .insert() to add each item to it.
    {
        TotalSize = 0.0f;
    }

    PDSubtractive(const pDomain &base) {
        this->base = base.copy();
        TotalSize = base.Size();
    }

    PDSubtractive(const pDomain &base, const pDomain &B)
    {
        TotalSize = base.Size() - B.Size();
        this->base = base.copy();
        Doms.push_back(B.copy());
    }

    PDSubtractive(const pDomain &base, const pDomain &B, const pDomain &C)
    {
        TotalSize = base.Size() - B.Size() - C.Size();
        this->base = base.copy();
        Doms.push_back(B.copy());
        Doms.push_back(C.copy());
    }

    /// Makes a copy of all the subdomains and point to the copies.
    ///
    /// Note that the Generate() function gets a performance boost if you supply DomList with the largest domains first.
    PDSubtractive(const pDomain &base, const std::vector<pDomain *> &DomList)
    {
        TotalSize = base.Size();
        this->base = base.copy();
        for(std::vector<pDomain *>::const_iterator it = DomList.begin(); it != DomList.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize -= (*it)->Size();
        }
    }

    // Makes a copy of all the subdomains and point to the copies.
    PDSubtractive(const PDSubtractive &P)
    {
        TotalSize = P.base->Size();
        this->base = P.base->copy();
        for(std::vector<pDomain *>::const_iterator it = P.Doms.begin(); it != P.Doms.end(); it++) {
            Doms.push_back((*it)->copy());
            TotalSize += (*it)->Size();
        }
    }

    ~PDSubtractive()
    {
        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            delete (*it);
        }

        delete(base);
    }

    /// Insert another domain into this PDSubtractive.
    void insert(const pDomain &A)
    {
        TotalSize += A.Size();
        Doms.push_back(A.copy());
    }

    bool Within(const Vec3f &pos) const /// Returns true if pos is within the specified subtractive domain.
    {
        if (!base->Within(pos)) {
            return false;
        }

        for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
            if((*it)->Within(pos)) return false;
        }
        return true;
    }

    Vec3f Generate() const /// Generate a point within the subtractive domain
    {
        Vec3f point;
        bool flag;

        do {
            flag = true;
            point = base->Generate();
            for(std::vector<pDomain *>::const_iterator it = Doms.begin(); it != Doms.end(); it++) {
                if ((*it)->Within(point)) {
                    flag = false;
                }
            }

        }while(!flag);

        return point;
    }

    float Size() const
    {
        return TotalSize;
    }

    pDomain *copy() const
    {
        PDSubtractive *P = new PDSubtractive(*this);
        return P;
    }
};


/// END SUBTRACTIVE

/// A single point.
///
/// Generate always returns this point. Within returns true if the point is exactly equal.
class PDPoint : public pDomain
{
public:
    Vec3f p;

public:
    PDPoint(const Vec3f &p0)
    {
        p = p0;
    }

    ~PDPoint()
    {
        // std::cerr << "del " << typeid(*this).name() << this << std::endl;
    }

    bool Within(const Vec3f &pos) const /// Always returns false.
    {
        return p == pos ? true : false;
    }

    Vec3f Generate() const /// Returns true if the point is exactly equal.
    {
        return p;
    }

    float Size() const
    {
        return 1.0f;
    }

    pDomain *copy() const
    {
        PDPoint *P = new PDPoint(*this);
        return P;
    }
};

/// A line segment.
///
/// e0 and e1 are the endpoints of the segment.
///
/// Generate returns a random point on this segment. Within returns true for points within epsilon of the line segment.
class PDLine : public pDomain
{
public:
    Vec3f p0, vec, vecNrm;
    float len;

public:
    PDLine(const Vec3f &e0, const Vec3f &e1)
    {
        p0 = e0;
        vec = e1 - e0;
        vecNrm = vec;
        vecNrm.normalize();
        len = vec.length();
    }

    ~PDLine()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true for points within epsilon of the line segment.
    {
        Vec3f to = pos - p0;
        float d = vecNrm.dot(to);
        float dif = fabs(d - to.length()) / len; // Has a sqrt(). Kind of slow.
        return dif < 1e-7f; // It's inaccurate, so we need this epsilon.
    }

    Vec3f Generate() const /// Returns a random point on this segment.
    {
        return p0 + vec * pRandf();
    }

    float Size() const
    {
        return len;
    }

    pDomain *copy() const
    {
        PDLine *P = new PDLine(*this);
        return P;
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

/// A Triangle.
///
/// p0, p1, and p2 are the vertices of the triangle. The triangle can be used to define an arbitrary geometrical model for particles to bounce off, or generate particles on its surface (and explode them), etc.
///
/// Generate returns a random point in the triangle. Within returns true for points within epsilon of the triangle. Currently it is not possible to sink particles that enter/exit a polygonal model. Suggestions?]
class PDTriangle : public pDomain
{
public:
    Vec3f p, u, v, uNrm, vNrm, nrm, s1, s2;
    float uLen, vLen, D, area;

public:
    PDTriangle(const Vec3f &p0, const Vec3f &p1, const Vec3f &p2)
    {
        p = p0;
        u = p1 - p0;
        v = p2 - p0;

        uLen = u.length();
        uNrm = u / uLen;
        vLen = v.length();
        vNrm = v / vLen;

        nrm = uNrm.cross(vNrm); // This is the non-unit normal.
        nrm.normalize(); // Must normalize it.

        D = -p.dot(nrm);

        NewBasis(u, v, s1, s2); // Compute the inverse matrix of the plane basis.

        // Compute the area
		Vec3f Hgt = v - uNrm * uNrm.dot(v);
        float h = Hgt.length();
        area = 0.5f * uLen * h;
    }

    ~PDTriangle()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true for points within epsilon of the triangle.
    {
        Vec3f offset = pos - p;
        float d = offset.dot(nrm);

        if(d > P_PLANAR_EPSILON) return false;

        // Dot product with basis vectors of old frame
        // in terms of new frame gives position in uv frame.
        float upos = offset.dot(s1);
        float vpos = offset.dot(s2);

        // Did it cross plane outside triangle?
        return !(upos < 0 || vpos < 0 || (upos + vpos) > 1);
    }

    Vec3f Generate() const /// Returns a random point in the triangle.
    {
        float r1 = pRandf();
        float r2 = pRandf();
        Vec3f pos;
        if(r1 + r2 < 1.0f)
            pos = p + u * r1 + v * r2;
        else
            pos = p + u * (1.0f-r1) + v * (1.0f-r2);

        return pos;
    }

    float Size() const
    {
        return area;
    }

    pDomain *copy() const
    {
        PDTriangle *P = new PDTriangle(*this);
        return P;
    }
};

/// Rhombus-shaped planar region.
///
/// p0 is a point on the plane. u0 and v0 are (non-parallel) basis vectors in the plane. They don't need to be normal or orthogonal.
///
/// Generate returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v. Within returns true for points within epsilon of the patch.
class PDRectangle : public pDomain
{
public:
    Vec3f p, u, v, uNrm, vNrm, nrm, s1, s2;
    float uLen, vLen, D, area;

public:
    PDRectangle(const Vec3f &p0, const Vec3f &u0, const Vec3f &v0)
    {
        p = p0;
        u = u0;
        v = v0;

        uLen = u.length();
        uNrm = u / uLen;
        vLen = v.length();
        vNrm = v / vLen;

        nrm = uNrm.cross(vNrm); //This is the non-unit normal.
        nrm.normalize(); // Must normalize it.

        D = -p.dot(nrm);

        NewBasis(u, v, s1, s2); // Compute the inverse matrix of the plane basis.

        // Compute the area
        Vec3f Hgt = v - uNrm * uNrm.dot(v);
        float h = Hgt.length();
        area = uLen * h;
    }

    ~PDRectangle()
    {
        // std::cerr << "del " << typeid(*this).name() << this << std::endl;
    }

    bool Within(const Vec3f &pos) const /// Returns true for points within epsilon of the patch.
    {
        Vec3f offset = pos - p;
		float d = offset.dot(nrm);

        if(d > P_PLANAR_EPSILON) return false;

        // Dot product with basis vectors of old frame
        // in terms of new frame gives position in uv frame.
        float upos = offset.dot(s1);
        float vpos = offset.dot(s2);

        // Did it cross plane outside triangle?
        return !(upos < 0 || upos > 1 || vpos < 0 || vpos > 1);
    }

    Vec3f Generate() const /// Returns a random point in the diamond-shaped patch whose corners are o, o+u, o+u+v, and o+v.
    {
        Vec3f pos = p + u * pRandf() + v * pRandf();
        return pos;
    }

    float Size() const
    {
        return area;
    }

    pDomain *copy() const
    {
        PDRectangle *P = new PDRectangle(*this);
        return P;
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
    Vec3f p, nrm, u, v;
    float radIn, radOut, radInSqr, radOutSqr, dif, D, area;

public:
    PDDisc(const Vec3f &Center, const Vec3f Normal, const float OuterRadius, const float InnerRadius = 0.0f)
    {
        p = Center;
        nrm = Normal;
        nrm.normalize();

        //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
        //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

        if(OuterRadius > InnerRadius) {
            radOut = OuterRadius; radIn = InnerRadius;
        } else {
            radOut = InnerRadius; radIn = OuterRadius;
        }

        radInSqr = fsqr(radIn);
        radOutSqr = fsqr(radOut);
        dif = radOut - radIn;

        // Find a vector orthogonal to n.
        Vec3f basis(1.0f, 0.0f, 0.0f);
       
        if (fabs(basis.dot(nrm)) > 0.999f)
            basis = Vec3f(0.0f, 1.0f, 0.0f);

        // Project away N component, normalize and cross to get
        // second orthonormal vector.
		u = basis - nrm * basis.dot(nrm);
        u.normalize();
        v = nrm.cross(u);
        D = -p.dot(nrm);

        if(dif == 0.0f)
            area = 2.0f * PI * radOut; // Length of circle
        else
            area = PI * radOutSqr - PI * radInSqr; // Area or disc minus hole
    }

    ~PDDisc()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true for points within epsilon of the disc.
    {
        Vec3f offset = pos - p;
		float d = offset.dot(nrm);

        if(d > P_PLANAR_EPSILON) return false;

        float squareLen = offset.squareLength();
        return squareLen >= radInSqr && squareLen <= radOutSqr;
    }

    Vec3f Generate() const /// Returns a point inside the disc shell.
    {
        // Might be faster to generate a point in a square and reject if outside the circle
        float theta = pRandf() * 2.0f * float(PI); // Angle around normal
        // Distance from center
        float r = radIn + pRandf() * dif;

        float x = r * cos(theta); // Weighting of each frame vector
        float y = r * sin(theta);

        Vec3f pos = p + u * x + v * y;
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
    Vec3f p, nrm;
    float D;

public:
    PDPlane(const Vec3f &p0, const Vec3f &Normal)
    {
        p = p0;
        nrm = Normal;
        nrm.normalize(); // Must normalize it.
        D = -p.dot(nrm);
    }

    ~PDPlane()
    {
    }

    // Distance from plane = n * p + d
    // Inside is the positive half-space.
    bool Within(const Vec3f &pos) const /// Returns true if the point is in the positive half-space of the plane (in the plane or on the side that Normal points to).
    {
        return nrm.dot(pos) >= -D;
    }

    // How do I sensibly make a point on an infinite plane?
    Vec3f Generate() const /// Returns the point p0.
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
    Vec3f p0, p1, dif;
    float vol;

public:
    PDBox(const Vec3f &e0, const Vec3f &e1)
    {
        p0 = e0;
        p1 = e1;
        if(e1.x < e0.x) { p0.x = e1.x; p1.x = e1.x; }
        if(e1.y < e0.y) { p0.y = e1.y; p1.y = e1.y; }
        if(e1.z < e0.z) { p0.z = e1.z; p1.z = e1.z; }

        dif = p1 - p0;
        vol = dif.dot(Vec3f(1,1,1));
    }

    ~PDBox()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true if the point is in the box.
    {
        return !((pos.x < p0.x) || (pos.x > p1.x) ||
            (pos.y < p0.y) || (pos.y > p1.y) ||
            (pos.z < p0.z) || (pos.z > p1.z));
    }

    Vec3f Generate() const /// Returns a random point in this box.
    {
        return p0 + Vec3f(pRandf()*dif.x, pRandf()*dif.y, pRandf()*dif.z);
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
    Vec3f apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
    float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol;
    bool ThinShell;

public:
    PDCylinder(const Vec3f &e0, const Vec3f &e1, const float OuterRadius, const float InnerRadius = 0.0f)
    {
        apex = e0;
        axis = e1 - e0;

        //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
        //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

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
        Vec3f n = axis;
        float axisLenSqr = axis.squareLength();
        float len = axis.length();
        axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
        n *= sqrt(axisLenInvSqr);

        // Find a vector orthogonal to n.
        Vec3f basis(1.0f, 0.0f, 0.0f);
		if (fabs(basis.dot(n)) > 0.999f)
            basis = Vec3f(0.0f, 1.0f, 0.0f);

        // Project away N component, normalize and cross to get
        // second orthonormal vector.
        u = basis - n * basis.dot(n);
        u.normalize();
        v = n.cross(u);

        float EndCapArea = PI * radOutSqr - PI * radInSqr;
        if(ThinShell)
            vol = len * 2.0f * PI * radOut; // Surface area of cylinder
        else
            vol = EndCapArea * len; // Volume of cylindrical shell
    }

    ~PDCylinder()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true if the point is within the cylindrical shell.
    {
        // This is painful and slow. Might be better to do quick accept/reject tests.
        // Axis is vector from base to tip of the cylinder.
        // x is vector from base to pos.
        //         x . axis
        // dist = ---------- = projected distance of x along the axis
        //        axis. axis   ranging from 0 (base) to 1 (tip)
        //
        // rad = x - dist * axis = projected vector of x along the base

        Vec3f x = pos - apex;

        // Check axial distance
        float dist = axis.dot(x) * axisLenInvSqr;
        if(dist < 0.0f || dist > 1.0f)
            return false;

        // Check radial distance
        Vec3f xrad = x - axis * dist; // Radial component of x
        float rSqr = xrad.squareLength();

        return (rSqr >= radInSqr && rSqr <= radOutSqr);
    }

    Vec3f Generate() const /// Returns a random point in the cylindrical shell.
    {
        float dist = pRandf(); // Distance between base and tip
        float theta = pRandf() * 2.0f * float(PI); // Angle around axis
        // Distance from axis
        float r = radIn + pRandf() * radDif;

        // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
        float x = r * cos(theta);
        float y = r * sin(theta);

        Vec3f pos = apex + axis * dist + u * x + v * y;
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
    Vec3f apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
    float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol;
    bool ThinShell;

public:
    PDCone(const Vec3f &e0, const Vec3f &e1, const float OuterRadius, const float InnerRadius = 0.0f)
    {
        apex = e0;
        axis = e1 - e0;

        //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
        //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

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
        Vec3f n = axis;
        float axisLenSqr = axis.squareLength();
        float len = axis.length();
        axisLenInvSqr = axisLenSqr ? 1.0f / axisLenSqr : 0.0f;
        n *= sqrt(axisLenInvSqr);

        // Find a vector orthogonal to n.
        Vec3f basis(1.0f, 0.0f, 0.0f);
		if (fabs(basis.dot(n)) > 0.999f)
            basis = Vec3f(0.0f, 1.0f, 0.0f);

        // Project away N component, normalize and cross to get
        // second orthonormal vector.
		u = basis - n * basis.dot(n);
        u.normalize();
        v = n.cross(u);

        if(ThinShell)
            vol = sqrt(axisLenSqr + radOutSqr) * PI * radOut; // Surface area of cone (not counting endcap)
        else {
            float OuterVol = 0.33333333f * PI * radOutSqr * len; 
            float InnerVol = 0.33333333f * PI * radInSqr * len;
            vol = OuterVol - InnerVol; // Volume of conical shell
        }
    }

    ~PDCone()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true if the point is within the conical shell.
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

        Vec3f x = pos - apex;

        // Check axial distance
        // axisLenInvSqr stores 1 / dot(axis, axis)
        float dist = axis.dot(x) * axisLenInvSqr;
        if(dist < 0.0f || dist > 1.0f)
            return false;

        // Check radial distance; scale radius along axis for cones
        Vec3f xrad = x - axis * dist; // Radial component of x
        float rSqr = xrad.squareLength();

        return (rSqr >= fsqr(dist * radIn) && rSqr <= fsqr(dist * radOut));
    }

    Vec3f Generate() const /// Returns a random point in the conical shell.
    {
        float dist = pRandf(); // Distance between base and tip
        float theta = pRandf() * 2.0f * float(PI); // Angle around axis
        // Distance from axis
        float r = radIn + pRandf() * radDif;

        // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
        float x = r * cos(theta);
        float y = r * sin(theta);

        // Scale radius along axis for cones
        x *= dist;
        y *= dist;

        Vec3f pos = apex + axis * dist + u * x + v * y;
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
    Vec3f ctr;
    float radOut, radIn, radOutSqr, radInSqr, radDif, vol;
    bool ThinShell;

public:
    PDSphere(const Vec3f &Center, const float OuterRadius, const float InnerRadius = 0.0f)
    {
        ctr = Center;

        //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
        //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

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
            vol = 4.0f * PI * radOutSqr; // Surface area of sphere
        else {
            float OuterVol = 1.33333333f * PI * radOutSqr * radOut; 
            float InnerVol = 1.33333333f * PI * radInSqr * radIn;
            vol = OuterVol - InnerVol; // Volume of spherical shell
        }
    }

    ~PDSphere()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true if the point lies within the thick shell.
    {
        Vec3f rvec(pos - ctr);
        float rSqr = rvec.squareLength();
        return rSqr <= radOutSqr && rSqr >= radInSqr;
    }

    Vec3f Generate() const /// Returns a random point in the thick spherical shell.
    {
        Vec3f pos;

        do {
            pos = Vec3f(pRandf(), pRandf(), pRandf()) - Vec3f(0.5, 0.5, 0.5); // Point on [-0.5,0.5] box
        } while (pos.squareLength() > fsqr(0.5)); // Make sure it's also on r=0.5 sphere.
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


/// Ellipsoid
//a = radius in the x, b = radius in the y, c = radius in the z
class PDEllipsoid : public pDomain
{
public:
    Vec3f ctr;
    float vol;
    float a, b, c;
    float maxDist;

public:
    PDEllipsoid(const Vec3f &Center, const float a, const float b, const float c)
    {
        ctr = Center;
        this->a = a;
        this->b = b;
        this->c = c;

        if (a > b && a > c) {
            maxDist = a;
        }else if (b > a && b > c) {
            maxDist = b;
        }else {
            maxDist = c;
        }

        vol = (4/3) * PI * a * b * c;
    }

    ~PDEllipsoid()
    {
    }

    bool Within(const Vec3f &pos) const /// Returns true if the point lies within the ellipsoid
    {
        Vec3f rvec(pos - ctr);
        float val = (rvec.x * rvec.x) / (a * a) + (rvec.y * rvec.y) / (b * b) + (rvec.z * rvec.z) / (c * c);
        return val <= 1.0 + P_PLANAR_EPSILON; //account for rounding errors
    }

    Vec3f Generate() const /// Returns a random point in the ellipsoid
    {
        Vec3f pos;
        float val = 0;
        do {
            pos = Vec3f(pRandf()*2*a - a, pRandf()*2*b - b, pRandf()*2*c - c) ;
            val = (pos.x * pos.x) / (a * a) + (pos.y * pos.y) / (b * b) + (pos.z * pos.z) / (c * c);
        }while (abs(val) > 1 + P_PLANAR_EPSILON);

        return pos + ctr;
    }

    float Size() const 
    {
        return vol;
    }

    float distanceToCenter(Vec3f p) {
        return (p - ctr).length();
    }

    float normDistanceToCenter(Vec3f p) {
        return (p-ctr).length() / maxDist;
    }

    pDomain *copy() const
    {
        PDEllipsoid *P = new PDEllipsoid(*this);
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
    Vec3f ctr;
    float stdev, Scale1, Scale2;

public:
    PDBlob(const Vec3f &Center, const float StandardDev)
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

    bool Within(const Vec3f &pos) const /// Has a probability of returning true equal to the probability density at the specified point.
    {
        Vec3f x = pos - ctr;
        float Gx = exp(x.squareLength() * Scale1) * Scale2;
        return (pRandf() < Gx);
    }

    Vec3f Generate() const /// Returns a point with normal probability density.
    {
		float x, y, r2;
		do {
			x = pRandf()*2.0f-1.0f;
			y = pRandf()*2.0f-1.0f;
			r2 = x*x+y*y;
		}
		while(r2 > 1.0f || r2 == 0.0f);

		float m = sqrt(-2.0f * log(r2)/r2);

		float px = x*m*stdev;
		float py = y*m*stdev;
		
		do {
            x = pRandf()*2.0f-1.0f;
            y = pRandf()*2.0f-1.0f;
            r2 = x*x+y*y;
        }
        while(r2 > 1.0 || r2 == 0.0);

        m = sqrt(-2.0f * log(r2)/r2);

        float pz = x*m*stdev;

        return ctr + Vec3f(px, py, pz);
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



#endif
