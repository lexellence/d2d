/**************************************************************************************\
** File: d2Animation.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for Animation and related classes
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Animation.h"
#include "d2Window.h"
namespace d2d
{
	//+--------------------------------\--------------------------------------
	//|		   AnimationFrame		   |
	//\--------------------------------/--------------------------------------
	AnimationFrame::AnimationFrame(const Texture& texture,
		float frameTime, const Color& tintColor,
		const b2Vec2& relativeSize, const b2Vec2& relativePosition, float relativeAngle)
		: m_texturePtr{ &texture },
		m_frameTime{ frameTime },
		m_tintColor{ tintColor },
		m_relativeSize{ relativeSize },
		m_relativePosition{ relativePosition },
		m_relativeAngle{ relativeAngle }
	{

	}
	void AnimationFrame::Draw(const b2Vec2& animationSize, const Color& animationColor) const
	{
		b2Vec2 finalSize = animationSize * m_relativeSize;
		Window::PushMatrix();
		Window::Translate(m_relativePosition);
		Window::Rotate(m_relativeAngle);
		Window::SetColor(animationColor * m_tintColor);
		Window::DrawSprite(*m_texturePtr, finalSize);
		Window::PopMatrix();
	}
	float AnimationFrame::GetFrameTime() const
	{
		return m_frameTime;
	}

	//+--------------------------------\--------------------------------------
	//|		    AnimationDef		   |
	//\--------------------------------/--------------------------------------
	AnimationDef::AnimationDef()
	{
		m_frameList.clear();
	}
	AnimationDef::AnimationDef(const AnimationFrame& frame)
		: m_type{ AnimationType::STATIC },
		m_firstFrame{ 0u }
	{
		m_frameList.push_back(frame);
	}
	AnimationDef::AnimationDef(const std::vector<AnimationFrame>& frameList,
		AnimationType type, unsigned firstFrame, bool startForward)
		: m_frameList{ frameList },
		m_type{ type },
		m_firstFrame{ firstFrame },
		m_startForward{ startForward }
	{
		d2Assert(m_firstFrame < m_frameList.size());
	}

	//+--------------------------------\--------------------------------------
	//|		      Animation	    	   |
	//\--------------------------------/--------------------------------------
	void Animation::Init(const AnimationDef& animationDef,
		const b2Vec2& relativeSize, const b2Vec2& relativePosition,
		float relativeAngle, const Color& tintColor)
	{
		m_def = animationDef;
		m_enabled = true;
		m_currentFrame = m_def.m_firstFrame;
		m_forward = m_def.m_startForward;
		m_frameTimeAccumulator = 0.0f;
		m_flipX = false;
		m_flipY = false;
		m_relativeSize = relativeSize;
		m_relativePosition = relativePosition;
		m_relativeAngle = relativeAngle;
		m_tintColor = tintColor;
	}
	void Animation::FlipX()
	{
		m_flipX = !m_flipX;
	}
	void Animation::FlipY()
	{
		m_flipY = !m_flipY;
	}
	void Animation::Update(float dt)
	{
		if (IsEnabled())
		{
			if(m_def.m_type == AnimationType::STATIC)
				return;

			m_frameTimeAccumulator += dt;
			if(m_frameTimeAccumulator >= GetCurrentFrame().GetFrameTime())
			{
				// Go to next frame
				m_frameTimeAccumulator -= GetCurrentFrame().GetFrameTime();
				m_currentFrame += m_forward ? 1 : -1;
				bool reachedEnd = m_forward && (m_currentFrame == m_def.m_frameList.size()) ||
								  !m_forward && (m_currentFrame == -1);

				// Adjust based on animation type
				switch(m_def.m_type)
				{
				default:
				case AnimationType::SINGLE_PASS:
					if(reachedEnd)
					{
						Restart();
						m_enabled = false;
						return;
					}
					break;
				case AnimationType::LOOP:
					if(reachedEnd)
						m_currentFrame = m_forward ? 0 : m_def.m_frameList.size() - 1;
					break;
				case AnimationType::PENDULUM:
					if(reachedEnd)
						m_currentFrame += m_forward ? -2 : 2;
					break;
				}
			}
		}
	}
	void Animation::Draw(const b2Vec2& entitySize) const
	{
		if (IsEnabled())
		{
			Window::PushMatrix();
			Window::Translate(m_relativePosition);
			Window::Rotate(m_relativeAngle);
			GetCurrentFrame().Draw(entitySize * m_relativeSize, m_tintColor);
			Window::PopMatrix();
		}
	}
	bool Animation::IsEnabled() const
	{
		return m_enabled;
	}
	bool Animation::IsAnimated() const
	{
		return m_def.m_type != AnimationType::STATIC;
	}
	void Animation::Restart()
	{
		m_enabled = true;
		m_currentFrame = m_def.m_firstFrame;
		m_forward = m_def.m_startForward;
		m_frameTimeAccumulator = 0.0f;
	}
	void Animation::SetTint(const Color& newTintColor)
	{
		m_tintColor = newTintColor;
	}
	const AnimationFrame& Animation::GetCurrentFrame() const
	{
		return m_def.m_frameList[m_currentFrame];
	}
}
