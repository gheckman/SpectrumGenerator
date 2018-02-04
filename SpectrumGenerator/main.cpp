// Standard headers
#include <iostream>
#include <iterator>
#include <vector>

// My headers
#include "math_ext.h"
#include "spectrum_generator.h"

// Third-party headers
#include "fft.h"

using std::cout;
using std::cin;
using std::endl;

using namespace math_ext;
using namespace spectrum_generation;

int main(int argc, char** argv)
{
	modulation_ops mod1;
	mod1.frequency = 5000;
	mod1.sigma = .5;
	mod1.sidebands = 1;
	mod1.sideband_offset = 8;
	mod1.snr = 1e9;

	modulation_ops mod2;
	mod2.frequency = 512;
	mod2.sigma = .5;
	mod2.sidebands = 0;
	mod2.snr = 2e8;

	spectrum_ops ops;
	ops.modulation.push_back(mod1);
	ops.modulation.push_back(mod2);

	ops.points = 4096;
	ops.rbw = 1;

	default_generator generator;
	auto spectrum = generator(ops);

	auto sampler = 0;

	std::vector<std::complex<double>> ifft(spectrum.size());
	std::transform(spectrum.begin(), spectrum.end(), ifft.begin(), [](double x) {return std::complex<double>(x, 0); });

	const char* error = nullptr;
	simple_fft::IFFT(ifft, ifft.size(), error);

	auto middle = ifft.begin() + std::distance(ifft.begin(), ifft.end()) / 2;
	std::rotate(ifft.begin(), middle, ifft.end());

	std::copy(spectrum.cbegin(), spectrum.cend(), std::ostream_iterator<double>(cout, "\n"));
	cout << "---" << endl;
	std::copy(ifft.cbegin(), ifft.cend(), std::ostream_iterator<std::complex<double>>(cout, "\n"));

	//char _;
	//cin >> _;

	return 0;
}
