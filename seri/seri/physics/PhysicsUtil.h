#pragma once

namespace seri::physics
{
	enum class ColliderShape
	{
		box = 0,
		sphere = 1,
		capsule = 2,
	};

	inline const char* ColliderShapeToString(ColliderShape shape)
	{
		switch (shape)
		{
			case ColliderShape::box: return "box";
			case ColliderShape::sphere: return "sphere";
			case ColliderShape::capsule: return "capsule";
			default: return "unknown";
		}
	}
}
