#include <iostream>
#include <string>
#include <quote.hpp>

int main()
{
	Quote* snp500 = new Quote("^GSPC");
	snp500->printSpots();
}
