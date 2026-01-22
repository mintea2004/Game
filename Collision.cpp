#include"Collision.h"



bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB,
    bool slamming
)
{
    if (!slamming)
    {
        if (positionA.y > positionB.y + heightB)
            return false;

        if (positionA.y + heightA < positionB.y) 
            return false;
    }
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;

    float range = radiusA + radiusB;
    float distSq = vx * vx + vz * vz;

   
    // Too far
    if (distSq > range * range)
        return false;

    // ✅ Always initialize outPositionB (VERY IMPORTANT)
    outPositionB = positionB;

    // ✅ Handle perfect overlap (dist == 0)
    const float eps = 1e-6f;
    float dist = sqrtf(distSq);
    if (dist < eps)
    {
        // Choose a stable push direction (any is fine)
        vx = 1.0f;
        vz = 0.0f;
        dist = 1.0f;
    }

    vx /= dist;
    vz /= dist;

 if (slamming)
    {
        // ignore vertical checks for shockwave slam
        // (or keep only a small tolerance if you want)

        // ... after normalizing dir:
        const float knockback = 5.0f; // tune this
        outPositionB = positionB;
        outPositionB.x += vx * knockback;
        outPositionB.z += vz * knockback;
        return true;
    }

    // ✅ Push out in XZ (both for slam and normal)
    outPositionB.x = positionA.x + (vx * range);
    outPositionB.z = positionA.z + (vz * range);

    // Keep Y (don’t teleport vertically)
    outPositionB.y = positionB.y;

    return true;
}




bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition
)
{
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}

	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	vx /= distXZ;
	vz /= distXZ;

	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}

