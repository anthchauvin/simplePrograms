#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;		//This is assignment 1 for 340

int main()
{
	map<string, unsigned> hist;
	string s;

	while (cin >> s)
	{
		++hist[s];
	}

	auto const longest_entry_in_map = max_element(begin(hist), end(hist),[](auto const& a, auto const& b)
									{
										return a > b;
									});

	auto mode = (longest_entry_in_map == end(hist)) ? 0 : longest_entry_in_map -> first.size();
	
	cout << "\n\n\nAn unsorted frequency histogram of the input is:\n";

	for_each(begin(hist), end(hist),
			[mode] (auto const& i)
			{
				cout
				<< left
				<< setw(mode)
				<< setfill(' ')
				<< i.first
				<< " |"
				<< left
				<< setw(i.second)
				<< setfill('*')
				<< ""
				<< '\n'
				;
			}
		);
}
