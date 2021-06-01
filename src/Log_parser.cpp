#include "Log_parser.hpp"

Log_parser::Log_parser(std::string name, std::string parser,
    std::vector<std::string> fields)
{
	this->name = name;
	this->parser = parser;
	this->fields = fields;
}

Log_parser::~Log_parser()
{
	return;
}

std::string Log_parser::get_name()
{
	return name;
}

std::string Log_parser::get_parser_str()
{
	return parser;
}

const_iterator Log_parser::fields_begin() const
{
	return fields.begin();
}

const_iterator Log_parser::fields_end() const
{
	return fields.end();
}
