/**************************************************************************************\
** File: d2ShapeFactory.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the ShapeFactory class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2ShapeFactory.h"
#include "d2Utility.h"
#include "d2Exception.h"
namespace d2d
{
	// Load shapes from file and add to bodyMap, overwriting existing entries with the same name
	void ShapeFactory::LoadFrom(const std::string& filePath)
	{
		boost::property_tree::ptree data;
		boost::property_tree::read_xml(filePath, data);
		for(auto const& bodyNode : data.get_child("bodydef.bodies"))
		{
			if(bodyNode.first == "body")
			{
				// name
				std::string bodyName{ bodyNode.second.get<std::string>("<xmlattr>.name") };
				SDL_assert_release(bodyName.length() > 0);

				// overwrite any existing body
				m_bodyMap[bodyName].fixtureGroups.clear();
				m_bodyMap[bodyName].sourceImageSize.SetZero();

				// size relative to entity
				float width{ bodyNode.second.get<float>("<xmlattr>.width") };
				float height{ bodyNode.second.get<float>("<xmlattr>.height") };
				SDL_assert_release(width > 0.0f);
				SDL_assert_release(height > 0.0f);
				m_bodyMap[bodyName].sourceImageSize.Set(width, height);

				for(auto const& fixtureGroupNode : bodyNode.second.get_child("fixtureGroups"))
				{
					if(fixtureGroupNode.first == "fixtureGroup")
					{
						// fixture properties
						FixtureGroup fixtureGroup;
						std::string shapeType{ fixtureGroupNode.second.get<std::string>("<xmlattr>.type") };
						SDL_assert_release(shapeType == "CIRCLE" || shapeType == "POLYGON");
						if(shapeType == "CIRCLE")
						{
							// circle
							fixtureGroup.isCircle = true;
							auto const& circleNode{ fixtureGroupNode.second.get_child("circle") };

							float radius{ circleNode.get<float>("<xmlattr>.r") };
							SDL_assert_release(radius >= 0.0f);

							float x{ circleNode.get<float>("<xmlattr>.x") };
							float y{ circleNode.get<float>("<xmlattr>.y") };

							fixtureGroup.circle.center.x = x / width;
							fixtureGroup.circle.center.y = y / height;
							fixtureGroup.circle.radiusRelativeToWidth = radius / width;
						}
						else
						{
							// polygons
							fixtureGroup.isCircle = false;
							for(auto const& polygonNode : fixtureGroupNode.second.get_child("polygons"))
							{
								if(polygonNode.first == "polygon")
								{
									PolygonShape poly;
									poly.numVertices = 0;

									// for each point
									for(auto const& pointNode : polygonNode.second.get_child(""))
									{
										if(pointNode.first == "point")
										{
											float x{ pointNode.second.get<float>("<xmlattr>.x") };
											float y{ pointNode.second.get<float>("<xmlattr>.y") };
											poly.vertices[poly.numVertices].Set(x / width, y / height);
											++poly.numVertices;
											SDL_assert_release(poly.numVertices <= maxPolygonVertices);
										}
									}
									fixtureGroup.polygons.push_back(poly);
								}
							}
						}
						m_bodyMap[bodyName].fixtureGroups.push_back(fixtureGroup);
					}
				}
			}
		}
	}
	void ShapeFactory::AddCircleShape(b2Body& bodyRef, float size,
		const d2d::Material& material, const Filter& filter,
		bool isSensor, const b2Vec2& position)
	{
		b2CircleShape circle;
		circle.m_p = position;
		circle.m_radius = 0.5f * size;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circle;
		fixtureDef.density = material.density;
		fixtureDef.friction = material.friction;
		fixtureDef.restitution = material.restitution;
		fixtureDef.filter = filter.filter;
		fixtureDef.isSensor = isSensor;
		bodyRef.CreateFixture(&fixtureDef);
	}
	void ShapeFactory::AddRectShape(b2Body& bodyRef, const b2Vec2& size,
		const d2d::Material& material, const Filter& filter, bool isSensor,
		const b2Vec2& position, float angle)
	{
		const float hx{ size.x * 0.5f };
		const float hy{ size.y * 0.5f };
		const int numVertices{ 4 };
		b2Transform localTransform{ position, b2Rot{ angle } };
		b2Vec2 vertices[numVertices] = { b2Mul(localTransform,{ -hx,-hy }),
			b2Mul(localTransform,{ hx,-hy }),
			b2Mul(localTransform,{ hx, hy }),
			b2Mul(localTransform,{ -hx, hy }) };
		b2PolygonShape poly;
		poly.Set(vertices, numVertices);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &poly;
		fixtureDef.density = material.density;
		fixtureDef.friction = material.friction;
		fixtureDef.restitution = material.restitution;
		fixtureDef.filter = filter.filter;
		fixtureDef.isSensor = isSensor;
		bodyRef.CreateFixture(&fixtureDef);
	}
	void ShapeFactory::AddShapes(b2Body& b2BodyRef, const b2Vec2& size,
		const std::string& modelName, const Material& material, const Filter& filter,
		bool isSensor, const b2Vec2& position, float angle)
	{
		auto bodyIterator = m_bodyMap.find(modelName);
		if (bodyIterator == m_bodyMap.end())
			throw InitException{ "Failed to get body definition: " + modelName };

		b2Transform localTransform{ position, b2Rot{ angle } };
		for (auto& fixtureGroup : bodyIterator->second.fixtureGroups)
		{
			b2FixtureDef fixtureDef;
			fixtureDef.density = material.density;
			fixtureDef.friction = material.friction;
			fixtureDef.restitution = material.restitution;
			fixtureDef.filter = filter.filter;
			fixtureDef.isSensor = isSensor;
			if (fixtureGroup.isCircle)
			{
				b2CircleShape circleShape;
				circleShape.m_p = fixtureGroup.circle.center * size;
				circleShape.m_radius = fixtureGroup.circle.radiusRelativeToWidth * size.x;
				fixtureDef.shape = &circleShape;
				b2BodyRef.CreateFixture(&fixtureDef);
			}
			else
			{
				for (auto& polygon : fixtureGroup.polygons)
				{
					b2Vec2 vertices[maxPolygonVertices];
					unsigned numVertices{ polygon.numVertices };

					if (numVertices > maxPolygonVertices)
						throw InitException{ "Polygon vertex limit exceeded" };

					for(unsigned i = 0; i < polygon.numVertices; ++i)
						vertices[i] = b2Mul(localTransform, polygon.vertices[i] * size);

					b2PolygonShape polygonShape;
					polygonShape.Set(vertices, polygon.numVertices);
					fixtureDef.shape = &polygonShape;
					b2BodyRef.CreateFixture(&fixtureDef);
				}
			}
		}
	}
}
