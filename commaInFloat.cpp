#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

string insertCommas(float input)
{
	cout << int(input) << endl;
	cout << input << endl;
	return to_string(input - int(input));
}

int main(int argc, const char* argv[])
{
	float num = stof(argv[1]);
	cout << fixed << setprecision<< insertCommas(num) << endl;
}
