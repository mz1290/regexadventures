#include "pcre2_mz.hpp"

void pcre2_destroy_mz(pcre2_code *re, pcre2_match_data *match_data)
{
		pcre2_match_data_free(match_data);
		pcre2_code_free(re);
}

bool pcre2_match_mz(const std::string &in_pattern, const std::string &in_sample,
    Results &pcre2_results)
{
	PCRE2_SPTR pattern;
	PCRE2_SPTR subject;
	PCRE2_SIZE subject_length;
	pcre2_code *re;
	int errornumber;
	PCRE2_SIZE erroroffset;
	pcre2_match_data *match_data;
	int rc;
	uint32_t namecount;
	PCRE2_SIZE *ovector;

	pattern = (PCRE2_SPTR)in_pattern.c_str();
	subject = (PCRE2_SPTR)in_sample.c_str();
	subject_length = (PCRE2_SIZE)in_sample.length();

	/* Compile pcre2 */
	re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber,
	    &erroroffset, NULL);

	/* Compilation failed: print the error message and exit.*/
	if (re == NULL) {
		PCRE2_UCHAR buffer[256];

		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		printf("PCRE2 compilation failed at offset %d: %s\n",
		    (int)erroroffset, buffer);
		return false;
	}

	/*
	* If the compilation succeeded, we call PCRE2 again, in order to do a
	* pattern match against the subject string. This does just ONE match. If
	* further matching is needed, it will be done below. Before running the
	* match we must set up a match_data block for holding the result. Using
	* pcre2_match_data_create_from_pattern() ensures that the block is
	* exactly the right size for the number of capturing parentheses in the
	* pattern. If you need to know the actual size of a match_data block as
	* a number of bytes, you can find it like this:
	*
	* PCRE2_SIZE match_data_size = pcre2_get_match_data_size(match_data);
	*/
	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	/* Run the match */
	rc = pcre2_match(re, subject, subject_length, 0, 0, match_data, NULL);

	/*
	 * See if there are any named substrings, and if so, show them by name.
	 * First we have to extract the count of named parentheses from the
	 * pattern.
	 */
	(void)pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &namecount);
	pcre2_results.n_groups = (size_t)rc;

	/* Matching failed: handle error cases */
	if (rc < 0) {
		switch(rc) {
		case PCRE2_ERROR_NOMATCH:
			printf("No match\n"); break;
		default:
			printf("Matching error %d\n", rc); break;
		}
		pcre2_destroy_mz(re, match_data);
		return false;
	}

	/*
	 * Match succeeded. Get a pointer to the output vector, where string 
	 * offsets are stored.
	 */
	ovector = pcre2_get_ovector_pointer(match_data);
	//printf("Match succeeded at offset %d\n", (int)ovector[0]);

	/*
	* We have found the first match within the subject string. If the output
	* vector wasn't big enough, say so. Then output any substrings that were
	* captured.
	*/

	/*
	 * The output vector wasn't big enough. This should not happen, because
	 * we used pcre2_match_data_create_from_pattern() above.
	 */
	if (rc == 0)
		printf("ovector was not big enough for all the captured "
		    "substrings\n");

	/*
	 * We must guard against patterns such as /(?=.\K)/ that use \K in an
	 * assertion to set the start of a match later than its end. In this
	 * demonstration program, we just detect this case and give up.
	 */
	if (ovector[0] > ovector[1]) {
		printf("\\K was used in an assertion to set the match start "
		    "after its end.\nFrom end to start the match was: %.*s\n",
		    (int)(ovector[0] - ovector[1]),
		    (char *)(subject + ovector[1]));
		printf("Run abandoned\n");
		pcre2_destroy_mz(re, match_data);
		return false;
	}

	/*
	 * Show substrings stored in the output vector by number. Obviously, in
	 * a real application you might want to do things other than print them.
	 */
	pcre2_results.results.resize(rc);
//	for (i = 0; i < rc; i++) {
//		PCRE2_SPTR substring_start = subject + ovector[2*i];
//		PCRE2_SIZE substring_length = ovector[2*i+1] - ovector[2*i];
//		char buffer[substring_length + 1];
//
//		strncpy(buffer, (char *)substring_start, substring_length);
//		buffer[substring_length] = '\n';
//		std::string str(buffer);
//		pcre2_results.results.push_back(str);
//	}

	/*
	 * See if there are any named substrings, and if so, show them by name.
	 * First we have to extract the count of named parentheses from the
	 * pattern.
	 */
	(void)pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &namecount);

	if (namecount == 0) {
		//printf("No named substrings\n");
		return true;
	} else {
		PCRE2_SPTR name_table;
		uint32_t name_entry_size;
		PCRE2_SPTR tabptr;
		uint32_t ui;

		//printf("Named substrings\n");

		/*
		 * Before we can access the substrings, we must extract the
		 * table for translating names to numbers, and the size of each
		 * entry in the table.
		 */
		(void)pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &name_table);
		(void)pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, 
		    &name_entry_size);

		/*
		 * Now we can scan the table and, for each entry, print the
		 * number, the name, and the substring itself. In the 8-bit
		 * library the number is held in two bytes, most significant
		 * first.
		 */

		// mymap.insert ( std::pair<char,int>('a',100) );

		tabptr = name_table;
		for (ui = 0; ui < namecount; ui++) {

			/* Insert group index and name */
			int n = (tabptr[0] << 8) | tabptr[1];
			char name_buffer[name_entry_size - 3 + 1];
			strncpy(name_buffer, (char *)tabptr + 2, name_entry_size -3);
			name_buffer[name_entry_size - 3] = '\0';
			std::string name_str(name_buffer);
			pcre2_results.idx_to_group.insert(std::pair<int, std::string>(n + 1, name_str));

			/* Insert extracted substring into vector */
			char *substring_start = (char*)subject + ovector[2 * n];
			int substring_length = ovector[2 * n + 1] - ovector[2 * n];
			char substring_buffer[substring_length + 1];
			strncpy(substring_buffer, substring_start, substring_length);
			substring_buffer[substring_length] = '\0';
			std::string substring_str(substring_buffer);
			auto itPos = pcre2_results.results.begin() + n;
			pcre2_results.results.insert(itPos, substring_str);
//			printf("(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
//			    (int)(ovector[2*n+1] - ovector[2*n]), subject + ovector[2*n]);
			tabptr += name_entry_size;
		}
	}

	pcre2_destroy_mz(re, match_data);
	return true;
}
