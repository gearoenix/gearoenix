#include "math-frustum.hpp"
#include "../system/sys-log.hpp"
#include "math-sphere.hpp"

gearoenix::math::Frustum::Frustum(const Plane (&planes)[6]) noexcept
    : planes { planes[0], planes[1], planes[2], planes[3], planes[4], planes[5] }
{
}

gearoenix::math::IntersectionStatus::Type gearoenix::math::Frustum::check_intersection(const Sphere& s) const noexcept
{
    IntersectionStatus::Type state = IntersectionStatus::In;
    for (const Plane& p : planes) {
        switch (p.check_intersection(s)) {
        case IntersectionStatus::Under:
            return IntersectionStatus::Out;
        case IntersectionStatus::Cut:
            state = IntersectionStatus::Cut;
            break;
        default:
            break;
        }
    }
    return state;
}

//for (int i = 0; i < 6; i++)
//{
//	int out = 0;
//	out += ((dot(fru.mPlane[i], vec4(box.mMinX, box.mMinY, box.mMinZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMaxX, box.mMinY, box.mMinZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMinX, box.mMaxY, box.mMinZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMaxX, box.mMaxY, box.mMinZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMinX, box.mMinY, box.mMaxZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMaxX, box.mMinY, box.mMaxZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMinX, box.mMaxY, box.mMaxZ, 1.0f)) < 0.0) ? 1 : 0);
//	out += ((dot(fru.mPlane[i], vec4(box.mMaxX, box.mMaxY, box.mMaxZ, 1.0f)) < 0.0) ? 1 : 0);
//	if (out == 8) return false;
//}
//
//// check frustum outside/inside box
//int out;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].x > box.mMaxX) ? 1 : 0); if (out == 8) return false;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].x < box.mMinX) ? 1 : 0); if (out == 8) return false;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].y > box.mMaxY) ? 1 : 0); if (out == 8) return false;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].y < box.mMinY) ? 1 : 0); if (out == 8) return false;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].z > box.mMaxZ) ? 1 : 0); if (out == 8) return false;
//out = 0; for (int i = 0; i < 8; i++) out += ((fru.mPoints[i].z < box.mMinZ) ? 1 : 0); if (out == 8) return false;