#include <iostream>
#include <omp.h> /* For timing ONLY */
#include <string>
#include <vector>
#include <map>

#ifndef NUMTRIES
#define NUMTRIES		100
#endif

/* RE2 */
#include <re2/re2.h>

/* PCRE2 */
#include "src/pcre2_mz.hpp"

/* YAML parsing */
#include <yaml-cpp/yaml.h>

/* Testing parser class */
#include "src/Log_parser.hpp"
#include "src/Results.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void CHECK(int, string, Results, double, double);
bool re2_match(const string&, const string&, Results&);
bool test_regex_lib(string, bool (*fptr)(const string&, const string&, Results&), YAML::Node);

int main()
{
	YAML::Node benchmark_yaml, tests;
	std::size_t i;

	/* Parse yaml data file */
	benchmark_yaml = YAML::LoadFile("benchmark_tests.yaml");
	tests = benchmark_yaml["tests"];

	cout << "RESULT,LIB,TYPE,TEST,PLEN,SLEN,NGROUPS,AVG_TIME,";
	cout << "BEST_TIME(microseconds)" << endl;
	for (i = 0; i < 2; i++) {
		bool (*fptr)(const string&, const string&, Results&);
		string lib;
		if (i == 0) {
			lib = "RE";
			fptr = &re2_match;
		} else {
			lib = "PCRE2";
			fptr = &pcre2_match_mz;
		}
		test_regex_lib(lib, fptr, tests);
	}

	return (0);
}

bool test_regex_lib(string lib,
    bool (*fptr)(const string&, const string&, Results&), YAML::Node tests)
{
	std::size_t i;

	for (i = 0; i < tests.size(); i++) {
		size_t t;
		double max_time = 999999;
		double sum_time = 0;
		string name = tests[i]["name"].as<string>();
		string type = tests[i]["type"].as<string>();
		string sample = tests[i]["sample"].as<string>();
		string pattern = tests[i]["regex"].as<string>();
		bool res = false;
		Results results(type, pattern.length(), sample.length(), lib);

		for (t = 0; t < NUMTRIES; t++) {
			double start, time;

			start = omp_get_wtime();
			res = (*fptr)(pattern, sample, results);
			time = (omp_get_wtime() - start) * 1000000;

			sum_time += time;
			if (max_time > time)
				max_time = time;
		}
		CHECK(res, name, results, sum_time / NUMTRIES, max_time);
		//results.print_results();
	}

	return true;
}

void CHECK(int predicate, string message, Results r, double avg, double time)
{
	if (predicate)
		cout << "PASSED,";
	else
		cout << "FAILED,";
	cout << r.library << ",";
	cout << r.type << "," << message << "," << r.pattern_len << ",";
	cout << r.sample_len << "," << r.n_groups << "," << avg << ",";
	cout << time << endl;
}

// https://stackoverflow.com/questions/25889065/how-to-use-re2-library-when-match-arguments-are-unknown
bool re2_match(const string &pattern, const string &str,
    Results &re2_results)
{
	std::string wrapped_pattern = "(" + pattern + ")";
	RE2::Options opt;
	opt.set_log_errors(false);
	opt.set_case_sensitive(false);
	//opt.set_utf8(false);
	RE2 re2(wrapped_pattern, opt);
	if (!re2.ok()) {
		/// Failed to compile regular expression.
		return false;
	}

	/// Argument vector.
	std::vector<RE2::Arg> arguments;
	/// Vercor of pointers to arguments.
	std::vector<RE2::Arg *> arguments_ptrs;

	/// Get number of arguments.
	std::size_t args_count = re2.NumberOfCapturingGroups();
	re2_results.n_groups = args_count;
	re2_results.idx_to_group = re2.CapturingGroupNames();

	/// Adjust vectors sizes.
	arguments.resize(args_count);
	arguments_ptrs.resize(args_count);
	re2_results.results.resize(args_count);
	/// Capture pointers to stack objects and result object in vector..
	for (std::size_t i = 0; i < args_count; ++i) {
		/// Bind argument to string from vector.
		arguments[i] = &re2_results.results[i];
		/// Save pointer to argument.
		arguments_ptrs[i] = &arguments[i];
	}

	return RE2::PartialMatchN(re2::StringPiece(str), re2, arguments_ptrs.data(),
	    args_count);
}
