#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

class Results
{
private:
public:
	size_t pattern_len;
	size_t sample_len;
	size_t n_groups;
	std::string type;
	std::string library;
	std::vector<std::string> results;
	std::map<int, std::string> idx_to_group;

	/* Constructor and destructor */
	Results(std::string, size_t, size_t, std::string);
	~Results();

	/* Output data */
	void print_results();
	void reset();
};
#endif
