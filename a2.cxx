#include <string>
#include <locale>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

template <typename TrimPredicate>
auto string_range_trim(string& str, TrimPredicate p) -> pair<
	string::iterator,
	string::iterator>
{
	auto last_non_ws_iter = find_if_not(str.rbegin(), str.rend(), p).base();
	auto first_non_ws_iter = find_if_not(begin(str), last_non_ws_iter, p);
	return make_pair(first_non_ws_iter, last_non_ws_iter);
}
auto read_next_termcap_entry(istream& is) -> pair<bool, string>
{
	ostringstream buffer;
	auto loc = is.getloc();
	buffer.imbue(loc);

	bool data = false;
	bool continued_line = true;

	for(
		string line;
		(!data || continued_line) && getline(is,line);
	)
	{
		continued_line = false;

		if(line.empty())
			continue;

		auto trimmed_range = string_range_trim(
			line,
			[&loc](char c){ return isspace(c, loc); }
		);
		if(trimmed_range.first == trimmed_range.second)
			continue;
		if(*trimmed_range.first == '#')
			continue;

		auto last_char_iter = trimmed_range.second;
		--last_char_iter;

		if(*last_char_iter == '\\')
		{
			continued_line = true;
			if(trimmed_range.first == last_char_iter)
				continue;

			trimmed_range.second = last_char_iter;
		}
		bool const header_line = isalpha(*trimmed_range.first,loc);

		bool const capability_line = (find(line.begin(), trimmed_range.first, '\t') != trimmed_range.first);

		if(header_line || capability_line)
		{
			data = true;
			copy(trimmed_range.first, trimmed_range.second, ostreambuf_iterator<char>(buffer));
		}
	}

	return { data, buffer.str() };
}
int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cerr << "Usage: " << argv[0] << " <termcap_db_file>\n ";
		return 1;
	}
	ifstream in(argv[1]);
	if (!in)
	{
		cerr << "Unable to open file. Aborting.\n";
		return 2;
	}
	for(
		auto result = read_next_termcap_entry(in);
		result.first;
		result = read_next_termcap_entry(in))
	{
			cout << result.second << '\n';
	}
	
	string s = " Hello You!\n";
	auto iterators = string_range_trim(s, [](auto ch){return isspace(ch, cin.getloc());});
	for_each(iterators.first, iterators.second,[](auto const& c) {
		cout << c;
		});
}


