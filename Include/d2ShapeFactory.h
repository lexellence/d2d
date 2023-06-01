/**************************************************************************************\
** File: d2ShapeFactory.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the ShapeFactory class
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	const unsigned int maxPolygonVertices{ b2_maxPolygonVertices };
	struct PolygonShape
	{
		unsigned int numVertices;
		b2Vec2 vertices[maxPolygonVertices];
	};
	struct CircleShape
	{
		b2Vec2 center;
		float radiusRelativeToWidth;
	};
	struct FixtureGroup
	{
		bool isCircle;
		CircleShape circle;
		std::vector<PolygonShape> polygons;
	};
	struct Body
	{
		b2Vec2 sourceImageSize;
		std::vector<FixtureGroup> fixtureGroups;
	};
	const float MATERIAL_DEFAULT_DENSITY	{ 1.0f };
	const float MATERIAL_DEFAULT_FRICTION	{ 0.4f };
	const float MATERIAL_DEFAULT_RESTITUTION{ 0.4f };
	struct Material
	{
		float density;
		float friction;
		float restitution;

		Material()
			: density{ MATERIAL_DEFAULT_DENSITY },
			friction{ MATERIAL_DEFAULT_FRICTION },
			restitution{ MATERIAL_DEFAULT_RESTITUTION } {}
		Material(float newDensity, float newFriction, float newRestitution)
			: density{ newDensity }, friction{ newFriction }, restitution{ newRestitution }	{ }
	};
	const unsigned FILTER_DEFAULT_CATEGORY_BITS{ 0x0001 };
	const unsigned FILTER_DEFAULT_MASK_BITS	   { 0xFFFF };
	const int	   FILTER_DEFAULT_GROUP_INDEX  { 0 };
	struct Filter
	{
	public:
		Filter(unsigned newCategoryBits = FILTER_DEFAULT_CATEGORY_BITS,
			unsigned newMaskBits = FILTER_DEFAULT_MASK_BITS,
			int newGroupIndex = FILTER_DEFAULT_GROUP_INDEX)
		{
			filter.categoryBits = newCategoryBits;
			filter.maskBits = newMaskBits;
			filter.groupIndex = newGroupIndex;
		}
		b2Filter filter;
	};
	class ShapeFactory
	{
	public:
		void LoadFrom(const std::string& filePath);
		void AddCircleShape(b2Body& bodyRef, float size,
			const d2d::Material& material, const Filter& filter,
			bool isSensor = false, const b2Vec2& position = b2Vec2_zero);
		void AddRectShape(b2Body& bodyRef, const b2Vec2& size,
			const d2d::Material& material, const Filter& filter,
			bool isSensor = false, const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);
		void AddShapes(b2Body& bodyRef, const b2Vec2& size, const std::string& modelName,
			const Material& material, const Filter& filter, bool isSensor=false,
			const b2Vec2& position = b2Vec2_zero, float angle = 0.0f);

	private:
		std::map<std::string, Body> m_bodyMap;
	};
}
