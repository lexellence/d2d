/**************************************************************************************\
** File: d2Range.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for Range template
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	template <typename T>
	class Range
	{
	public:
		Range() : m_min{}, m_max{} {}
		Range(const T& newMin, const T& newMax)
		{
			Set(newMin, newMax);
		}
		void Set(const T& newMin, const T& newMax)
		{
			m_min = newMin;
			m_max = newMax;
			Sort();
		}
		const T& GetMin() const
		{
			return m_min;
		}
		const T& GetMax() const
		{
			return m_max;
		}
		T GetSize() const
		{
			return m_max - m_min;
		}
		bool Contains(const T& value) const
		{
			return (value >= m_min && value <= m_max);
		}
		bool Contains(const Range<T>& range) const
		{
			return Contains(range.m_min) && Contains(range.m_max);
		}
	private:
		void Sort()
		{
			if(m_min > m_max)
				std::swap(m_min, m_max);
		}
		T m_min;
		T m_max;
	};
	inline const Range<float> VALID_PERCENT_RANGE{ 0.0f, 1.0f };
	inline const Range<float> VALID_PERCENT_RANGE_SIGNED{ -1.0f, 1.0f };
}
