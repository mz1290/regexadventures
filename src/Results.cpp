#include "Results.hpp"


Results::Results(std::string s_type, size_t patt_l, size_t samp_l,
    std::string lib)
{
	type = s_type;
	pattern_len = patt_l;
	sample_len = samp_l;
	library = lib;
}

Results::~Results() {}

void Results::print_results()
{
	for (auto const &pair: idx_to_group) {
		std::cout << "{ " << pair.first << " : " << pair.second << " : ";
		std::cout << results[pair.first - 1] <<  " }" << std::endl;
	}
}

void Results::reset()
{
	n_groups = 0;
	results.clear();
	idx_to_group.clear();
	library.clear();
}
