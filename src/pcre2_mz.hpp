#ifndef PCRE2_MZ_HPP
#define PCRE2_MZ_HPP

#define PCRE2_CODE_UNIT_WIDTH	8 /* required by pcre2.h */
#include <pcre2.h>
#include <iostream>
#include <string>
#include "Results.hpp"

void pcre2_destroy_mz(pcre2_code*, pcre2_match_data*);
bool pcre2_match_mz(const std::string&, const std::string&, Results&);
#endif
