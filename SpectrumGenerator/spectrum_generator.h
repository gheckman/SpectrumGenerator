#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <random>
#include <vector>

//! Provides tools for generating spectra
namespace spectrum_generation
{

//! Options for each modulation applied to the spectrum
struct modulation_ops
{
	double frequency = 1.0; //!< Frequency in Hz in frequency domain
	int sidebands = 0; //<! Amount of sidebands to modulate on
	double sideband_offset = 0.0; //<! Offset in Hz of the first order sidebands from the main peak
	double sigma = 0.0; //<! Sigma of the guassian distributed peak
	double snr = 1.0; //<! Signal to noise ratio
};

//! Options for spectrum generation
struct spectrum_ops
{
	std::vector<modulation_ops> modulation; //<! Vector of Modulation operations applied to the spectrum
	int points = 1; //<! Amount of points in the final spectrum
	double rbw = 1.0; //<! Resolution bandwidth (currently unused)
};

//! Generates the spectrum using the supplied options
class default_generator
{
public:
	default_generator() : default_generator(1) {}
	default_generator(double sigma) :
		engine_(std::random_device{}()),
		normal_(0, sigma),
		uniform_(0, 1)
	{}

	//! Generates the spectrum
	std::vector<double> operator()(const spectrum_ops& ops)
	{
		std::vector<double> spectrum(ops.points);
		for (const auto& mod : ops.modulation)
		{
			auto offset = 128; // mod.frequency * -uniform_(engine_);
			while (offset < spectrum.size() + mod.frequency)
			{
				for (int i = 0; i <= mod.sidebands; ++i)
				{
					auto scale = 1.0 / (i + 1.0);
					auto pow2 = 1 << i;
					auto inv_pow2 = 1.0 / pow2;

					auto power = mod.snr * inv_pow2;
					auto peak_width = mod.sigma * 3; // 3 should give a decent guass shape without getting too much tail
					auto local_offset = offset + mod.sideband_offset * (inv_pow2 - 1.0);

					for (int j = 0; j < pow2; ++j)
					{
						auto range = clamp_it(spectrum.begin(), spectrum.end(), local_offset - peak_width, local_offset + peak_width);
						auto sampler = (double)std::distance(spectrum.begin(), range.first);
						auto sample = [&](double x) { return x + math_ext::guass(sampler++, local_offset, mod.sigma) * power; };
						std::transform(range.first, range.second, range.first, sample);
						local_offset += mod.sideband_offset * inv_pow2 * 2;
					}
				}

				offset += mod.frequency;
			}
		}
		std::transform(spectrum.begin(), spectrum.end(), spectrum.begin(), [&](double x) { return x + normal_(engine_); });
		return spectrum;
	}
private:
	std::default_random_engine engine_;
	std::normal_distribution<> normal_;
	std::uniform_real_distribution<> uniform_;

	//! Clamps the incoming iterators to a specified offset
	template <typename Iter>
	static std::pair<Iter, Iter> clamp_it(Iter begin, Iter end, int offset_begin, int offset_end)
	{
		auto size = std::distance(begin, end);
		offset_begin = math_ext::clamp(offset_begin, 0, size);
		offset_end = math_ext::clamp(offset_end, offset_begin, size);
		end = begin;
		std::advance(begin, offset_begin);
		std::advance(end, offset_end);
		return std::make_pair(begin, end);
	}
};

}
