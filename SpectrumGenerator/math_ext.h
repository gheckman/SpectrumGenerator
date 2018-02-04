#pragma once

#include <cassert>

//! Mathematical type functions
namespace math_ext
{
	constexpr double pi = 3.141592653589792; //!< normal old pi
	constexpr double inv_sqrt_tau = 0.3989422804014327; //!< 1 / sqrt(2 * pi)

	template<typename T, typename Compare>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
	{
		return assert(!comp(hi, lo)),
			comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	template<typename T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi)
	{
		return clamp(v, lo, hi, std::less<>());
	}

	//! Guassian distribution function
	/*! Outputs (0, 1] Maxmimum when value = mean */
	template<typename T>
	T guass(T val, T mean, T sigma)
	{
		auto diff = val - mean;
		diff *= diff;
		auto var = 2 * sigma * sigma;
		return ::exp(-diff / var);
	}

	//! Guassian distribution function
	/*! Normalized so that the integral of the function over (-Inf, Inf) = 1 */
	template<typename T>
	T guass_normal(T val, T mean, T sigma)
	{
		return inv_sqrt_tau * sigma * guass(val, mean, sigma);
	}
}