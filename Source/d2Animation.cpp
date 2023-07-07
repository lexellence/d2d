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
	//|		      Animation	    	   |
	//\--------------------------------/--------------------------------------
	void Animation::Init(const AnimationDef& animationDef,
		const b2Vec2& relativeSize, const b2Vec2& relativePosition,
		float relativeAngle, const Color& tintColor)
	{
		d2Assert(animationDef.firstFrame < animationDef.frameList.size());
		m_def = animationDef;
		m_enabled = true;
		m_currentFrame = m_def.firstFrame;
		m_forward = m_def.startForward;
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
			if(m_def.type == AnimationType::STATIC)
				return;

			m_frameTimeAccumulator += dt;
			if(m_frameTimeAccumulator >= GetCurrentFrame().frameTime)
			{
				// Go to next frame
				m_frameTimeAccumulator -= GetCurrentFrame().frameTime;
				m_currentFrame += m_forward ? 1 : -1;
				bool reachedEnd = m_forward && (m_currentFrame == m_def.frameList.size()) ||
								  !m_forward && (m_currentFrame == -1);

				// Adjust based on animation type
				switch(m_def.type)
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
						m_currentFrame = m_forward ? 0 : m_def.frameList.size() - 1;
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
			const AnimationFrame& frame = GetCurrentFrame();
			if(frame.texturePtr)
			{
				Window::SetColor(m_tintColor);

				Window::PushMatrix();
				Window::Translate(m_relativePosition);
				Window::Rotate(m_relativeAngle);
				{
					Window::PushMatrix();
					Window::Translate(frame.relativePosition);
					Window::Rotate(frame.relativeAngle);
					b2Vec2 drawSize = entitySize * m_relativeSize * frame.relativeSize;
					Window::DrawTexture(*frame.texturePtr, drawSize);
					Window::PopMatrix();
				}
				Window::PopMatrix();
			}
		}
	}
	bool Animation::IsEnabled() const
	{
		return m_enabled;
	}
	bool Animation::IsAnimated() const
	{
		return m_def.type != AnimationType::STATIC;
	}
	void Animation::Restart()
	{
		m_enabled = true;
		m_currentFrame = m_def.firstFrame;
		m_forward = m_def.startForward;
		m_frameTimeAccumulator = 0.0f;
	}
	void Animation::SetTint(const Color& newTintColor)
	{
		m_tintColor = newTintColor;
	}
	const AnimationFrame& Animation::GetCurrentFrame() const
	{
		return m_def.frameList[m_currentFrame];
	}
}
