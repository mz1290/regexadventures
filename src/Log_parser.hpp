#ifndef LOG_PARSER_HPP
#define LOG_PARSER_HPP

#include <vector>
#include <string>

typedef std::vector<std::string>::const_iterator const_iterator;

class Log_parser
{
private:
	std::string name;
	std::string parser;
	// some compile parser
	std::vector<std::string> fields;

public:
	/* Constructor and destructor */
	Log_parser(std::string, std::string, std::vector<std::string>);
	~Log_parser();

	/* Get interface */
	std::string get_name();
	std::string get_parser_str();
	const_iterator fields_begin() const;
	const_iterator fields_end() const;
};
#endif