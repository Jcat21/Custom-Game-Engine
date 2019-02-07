#include "stdafx.h"
#include "Physics.h"
#include <math.h>
#include "Components\CollisionComponent.h"

Physics::AABB::AABB()
{
}

Physics::AABB::AABB(Vector3 pMin, Vector3 pMax)
	: mMin(pMin)
	, mMax(pMax)
{
}

bool Physics::Intersect(const AABB & a, const AABB & b, AABB * pOverlap)
{
	Vector3 min = Vector3(max(a.mMin.x, b.mMin.x), max(a.mMin.y, b.mMin.y), max(a.mMin.z, b.mMin.z));
	Vector3 max = Vector3(min(a.mMax.x, b.mMax.x), min(a.mMax.y, b.mMax.y), min(a.mMax.z, b.mMax.z));
	
	pOverlap->mMax = max;
	pOverlap->mMin = min;

	return (a.mMin.x <= b.mMax.x && a.mMax.x >= b.mMin.x) &&
			(a.mMin.y <= b.mMax.y && a.mMax.y >= b.mMin.y) &&
			(a.mMin.z <= b.mMax.z && a.mMax.z >= b.mMin.z);
}

bool Physics::Intersect(const Ray & ray, const AABB & box, Vector3 * pHitPoint)
{
	Vector3 d = ray.mTo - ray.mFrom;
	float tmin = 0.0;
	float tmax = FLT_MAX;

	if (abs(d.x) < FLT_EPSILON) {
		if (ray.mFrom.x < box.mMin.x || ray.mFrom.x > box.mMax.x)
			return false;
	}
	else {
		float ood = 1.0f / d.x;
		float t1 = (box.mMin.x - ray.mFrom.x) * ood;
		float t2 = (box.mMax.x - ray.mFrom.x) * ood;
		// Make t1 be intersection with near plane, t2 with far plane
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}
		// Compute the intersection of slab intersection intervals
		if (t1 > tmin) tmin = t1;
		if (t2 > tmax) tmax = t2;
		// Exit with no collision as soon as slab intersection becomes empty
		if (tmin > tmax) return false;
	}

	if (abs(d.y) < FLT_EPSILON) {
		if (ray.mFrom.y < box.mMin.y || ray.mFrom.y > box.mMax.y)
			return false;
	}
	else {
		float ood = 1.0f / d.y;
		float t1 = (box.mMin.y - ray.mFrom.y) * ood;
		float t2 = (box.mMax.y - ray.mFrom.y) * ood;
		// Make t1 be intersection with near plane, t2 with far plane
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}
		// Compute the intersection of slab intersection intervals
		if (t1 > tmin) tmin = t1;
		if (t2 > tmax) tmax = t2;
		// Exit with no collision as soon as slab intersection becomes empty
		if (tmin > tmax) return false;
	}

	if (abs(d.z) < FLT_EPSILON) {
		if (ray.mFrom.z < box.mMin.z || ray.mFrom.z > box.mMax.z)
			return false;
	}
	else {
		float ood = 1.0f / d.z;
		float t1 = (box.mMin.z - ray.mFrom.z) * ood;
		float t2 = (box.mMax.z - ray.mFrom.z) * ood;
		// Make t1 be intersection with near plane, t2 with far plane
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}
		// Compute the intersection of slab intersection intervals
		if (t1 > tmin) tmin = t1;
		if (t2 > tmax) tmax = t2;
		// Exit with no collision as soon as slab intersection becomes empty
		if (tmin > tmax) return false;
	}
	if (!(tmin > 0.0f && tmin < 1.0f))
	{
		return false;
	}
	*pHitPoint = ray.mFrom + (d * tmin);
	return true;
}

bool Physics::UnitTest()
{
	bool isOk = true;
	AABB overlap;
	Vector3 hitPoint = Vector3::Zero;

	// AABB vs AABB Tests
	{
		// Overlap
		isOk &= (Intersect(AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == 0.0f && overlap.mMin.y == 0.0f && overlap.mMin.z == 0.0f) && (overlap.mMax.x = 10.0f && overlap.mMax.y == 10.0f && overlap.mMax.z == 10.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == -100.0f && overlap.mMin.y == -10.0f && overlap.mMin.z == -10.0f) && (overlap.mMax.x = -90.0f && overlap.mMax.y == 10.0f && overlap.mMax.z == 10.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == 90.0f && overlap.mMin.y == -10.0f && overlap.mMin.z == -10.0f) && (overlap.mMax.x = 100.0f && overlap.mMax.y == 10.0f && overlap.mMax.z == 10.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == -10.0f && overlap.mMin.y == -100.0f && overlap.mMin.z == -10.0f) && (overlap.mMax.x = 10.0f && overlap.mMax.y == -90.0f && overlap.mMax.z == 10.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == -10.0f && overlap.mMin.y == 90.0f && overlap.mMin.z == -10.0f) && (overlap.mMax.x = 10.0f && overlap.mMax.y == 100.0f && overlap.mMax.z == 10.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == -10.0f && overlap.mMin.y == -10.0f && overlap.mMin.z == -100.0f) && (overlap.mMax.x = 10.0f && overlap.mMax.y == 10.0f && overlap.mMax.z == -90.0f));
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)), &overlap) == true);
		isOk &= ((overlap.mMin.x == -10.0f && overlap.mMin.y == -10.0f && overlap.mMin.z == 90.0f) && (overlap.mMax.x = 10.0f && overlap.mMax.y == 10.0f && overlap.mMax.z == 100.0f));

		// No overlap
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)), &overlap) == false);
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)), &overlap) == false);
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)), &overlap) == false);
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)), &overlap) == false);
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, -10.0f, -110.0f)), &overlap) == false);
		isOk &= (Intersect(AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)), &overlap) == false);
	}

	{
		// Collision
		isOk &= (Intersect(Ray(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= ( hitPoint.x == -100.0f && hitPoint.y == 0.0f && hitPoint.z == 0.0f );
		isOk &= (Intersect(Ray(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= (hitPoint.x == 0.0f && hitPoint.y == -100.0f && hitPoint.z == 0.0f);
		isOk &= (Intersect(Ray(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= (hitPoint.x == 0.0f && hitPoint.y == 0.0f && hitPoint.z == -100.0f);
		isOk &= (Intersect(Ray(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= (hitPoint.x == 100.0f && hitPoint.y == 0.0f && hitPoint.z == 0.0f);
		isOk &= (Intersect(Ray(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= (hitPoint.x == 0.0f && hitPoint.y == 100.0f && hitPoint.z == 0.0f);
		isOk &= (Intersect(Ray(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == true);
		isOk &= (hitPoint.x == 0.0f && hitPoint.y == 0.0f && hitPoint.z == 100.0f);

		// No collision
		isOk &= (Intersect(Ray(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
		isOk &= (Intersect(Ray(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
		isOk &= (Intersect(Ray(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
		isOk &= (Intersect(Ray(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
		isOk &= (Intersect(Ray(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
		isOk &= (Intersect(Ray(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)), AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)), &hitPoint) == false);
	}

	return isOk;
}

void Physics::AddObj(CollisionComponent * pObj)
{
	mObj.push_back(pObj);
}

void Physics::RemoveObj(CollisionComponent * pObj)
{
	auto position = std::find(mObj.begin(), mObj.end(), pObj);
	if (position != mObj.end())
		mObj.erase(position);
}

bool Physics::RayCast(const Ray & ray, Vector3 * pHitPoint)
{
	Vector3 pos = Vector3::Zero;
	bool ret = false;
	for (unsigned int i = 0; i < mObj.size(); ++i)
	{
		bool temp = Intersect(ray, mObj[i]->GetAABB(), &pos);
		if (temp)
		{
			ret = true;
			Vector3 diff = pos - ray.mFrom;
			Vector3 diff2 = *pHitPoint - ray.mFrom;
			if (diff.Length() < diff2.Length())
			{
				*pHitPoint = pos;
			}
		}
	}
	return ret;
}

Physics::Ray::Ray()
{
}

Physics::Ray::Ray(Vector3 pFrom, Vector3 pTo)
	: mFrom(pFrom)
	, mTo(pTo)
{
}
