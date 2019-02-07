#pragma once
#include "engineMath.h"
#include <vector>

class CollisionComponent;

class Physics
{
public:
	class AABB
	{
	public:
		Vector3 mMin;
		Vector3 mMax;

		AABB();
		AABB(Vector3 pMin, Vector3 pMax);
	};

	class Ray
	{
	public:
		Vector3 mFrom;
		Vector3 mTo;

		Ray();
		Ray(Vector3 pFrom, Vector3 pTo);
	};

	static bool Intersect(const AABB& a, const AABB& b,
		AABB* pOverlap = nullptr);
	static bool Intersect(const Ray& ray, const AABB& box,
		Vector3* pHitPoint = nullptr);
	static bool UnitTest();

	std::vector<CollisionComponent*> mObj;

	void AddObj(CollisionComponent* pObj);
	void RemoveObj(CollisionComponent* pObj);

	bool RayCast(const Ray& ray, Vector3* pHitPoint = nullptr);
};