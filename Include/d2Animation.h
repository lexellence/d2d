 /**************************************************************************************\
** File: d2Animation.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for Animation and related classes
**
\**************************************************************************************/
#pragma once
#include "d2Texture.h"
#include "d2Color.h"
namespace d2d
{
	class AnimationFrame
	{
	public:
		AnimationFrame(const Texture& texture, float frameTime = 0.0f,
			const b2Vec2& relativeSize = { 1.0f, 1.0f },
			const b2Vec2& relativePosition = b2Vec2_zero, float relativeAngle = 0.0f);
		void Draw(const b2Vec2& animationSize) const;
		float GetFrameTime() const;

	private:
		Texture const* m_texturePtr{ nullptr };
		float m_frameTime;
		b2Vec2 m_relativeSize;
		b2Vec2 m_relativePosition;
		float m_relativeAngle;
		};
	enum class AnimationType
	{
		STATIC, SINGLE_PASS, LOOP, PENDULUM
	};
	struct AnimationDef
	{
		std::vector<AnimationFrame> frameList;
		AnimationType type { AnimationType::STATIC };
		unsigned firstFrame{ 0 };
		bool startForward{ true };
	};
	class Animation
	{
	public:
		void Init(const AnimationDef& animationDef,
			const b2Vec2& relativeSize = { 1.0f, 1.0f },
			const b2Vec2& relativePosition = b2Vec2_zero, float relativeAngle = 0.0f,
			const Color& tint = WHITE_OPAQUE);
		void FlipX();
		void FlipY();
		void Update(float dt);
		void Draw(const b2Vec2& entitySize) const;
		bool IsEnabled() const;
		bool IsAnimated() const;
		void Restart();
		void SetTint(const Color& tint = WHITE_OPAQUE);

	private:
		AnimationDef m_def;
		bool m_enabled{ false };
		unsigned m_currentFrame;
		bool m_forward;
		float m_frameTimeAccumulator;
		bool m_flipX;
		bool m_flipY;
		b2Vec2 m_relativeSize;
		b2Vec2 m_relativePosition;
		float m_relativeAngle;
		Color m_tintColor;

		const AnimationFrame& GetCurrentFrame() const;
	};
}
