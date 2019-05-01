/*************************************************************************************\
** File: d2Timer.cpp
** Project: Space
** Author: David Leksen
** Date: 4/1/16
** 
** Source code file for the Timer class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Timer.h"
namespace d2d
{
	Timer::Timer()
	{
		Start();
	}
	//+-------------------------\---------------------------------------------------------- 
	//| Name: Start				| 
	//\-------------------------/
	// PostCondition: Initializes the timer
	//-------------------------------------------------------------------------------------
	void Timer::Start()
	{
		m_dt = 0.0;
		m_lastTicks = SDL_GetPerformanceCounter();
	}
	//+-------------------------\---------------------------------------------------------- 
	//| Name: Getdt	| 
	//\-------------------------/
	// PostCondition: Returns the number of seconds between ticks.
	//			If this is the first call, returns the seconds since start.
	//-------------------------------------------------------------------------------------
	float Timer::Getdt() const
	{
		return m_dt;
	}
	void Timer::Update()
	{
		// Get the new time and tick-rate
		const Uint64 ticksPerSecond = SDL_GetPerformanceFrequency();
		const Uint64 currentTicks = SDL_GetPerformanceCounter();

		// Calculate dt
		const Uint64 deltaTicks = currentTicks - m_lastTicks;
		m_lastTicks = currentTicks;
		m_dt = (float)(deltaTicks) / (float)(ticksPerSecond);
	}
}