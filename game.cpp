#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <limits>
#include <iomanip>

#define STARTING_CASH 100000
#define TRADE_COST 5
#define ARRAY_ELEMENTS 8
#define MARGIN .02
#define ITERATIONS_FOR_CURRVAL_TRACKER 0

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

// cat example.txt | ssmtp 9702195822@vtext.com

struct Stock {
	string name;
	string url;
	string type;
	float currVal;
	float currStockHeld;
	float remainingCash;
};

bool validTime()
{
	return true;
	time_t s, val = 1;
        struct tm* current_time;
        s = time(NULL);
        current_time = localtime(&s);
        int hourTime = (current_time->tm_hour);
	int minTime = (current_time->tm_min);
	if (hourTime >= 7 && hourTime < 14)
	{
		return true;
	}
	return false;
}

void removeEntry(vector<Stock*> &stocks, string name)
{
        vector<Stock*> tmp;
        for (int i = 0; i < stocks.size(); i++)
        {
                if (stocks[i]->name != name)
                {
                        tmp.push_back(stocks[i]);
                }
        }
        stocks = tmp;
}

int getHour()
{
	time_t s, val = 1;
        struct tm* current_time;
        s = time(NULL);
        current_time = localtime(&s);
        int hourTime = (current_time->tm_hour);
	return hourTime;
}

string extractTicker(string input)
{
	int counter = 43;
	string finalString;
	while (input[counter] != '-')
	{
		finalString += input[counter];
		counter++;
	}
	return finalString;
}

void header()
{
	cout << endl;
	ifstream header;
	header.open("header.txt");
	if (header.is_open())
        {
                string line = "";
                while (getline(header, line))
                {
			cout << line << endl;
                }
        }
        else
        {
                cerr << "failed on opening header file in header" << endl;
                // system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
                exit(22);
        }
	header.close();
	cout << endl;
}

string lowercase(string input)
{
	string finalString = "";
	for (int i = 0; i < input.length(); i++)
	{
		finalString += tolower(input[i]);
	}
	return finalString;
}

string uppercase(string input)
{
	string finalString = "";
	for (int i = 0; i < input.length(); i++)
	{
		finalString += toupper(input[i]);
	}
	return finalString;
}

bool isANumber(char input)
{
	string compare = "1234567890.";
	for (int i = 0; i < compare.size(); i++)
	{
		if (compare[i] == input)
		{
			return true;
		}
	}
	return false;
}

string buildUrl(string ticker)
{
	string finalstring = "https://markets.businessinsider.com/stocks/" + ticker + "-stock";
	return finalstring;
}

float getNetWorth(string filename)
{
	ifstream in;
	in.open(filename);
	if (in.is_open())
	{
                string line = "";
                while (getline(in, line))
                {
			return stof(line);
		}
	}
	else
	{
		cerr << "failed on opening netWorth file in getnetWorth ??" << endl;
		// system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

void writeNetWorth(float addition, string filename)
{
	ofstream out;
	out.open(filename);
	if (out.is_open())
	{
		out << fixed << setprecision(2) << addition << endl;
		out.close();
	}
	else
	{
		cerr << "failed on opening netWorth file in incrimentation" << endl;
		// system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

float getShortBank()
{
	ifstream in;
	in.open("shortBank.txt");
	if (in.is_open())
	{
                string line = "";
                while (getline(in, line))
                {
			return stof(line);
		}
	}
	else
	{
		cerr << "failed on opening shortBank file in getShortBank" << endl;
		// system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

float getTradeCounter()
{
	ifstream in;
	in.open("tradeCounter.txt");
	if (in.is_open())
	{
                string line = "";
                while (getline(in, line))
                {
			return stof(line);
		}
	}
	else
	{
		cerr << "failed on opening tradeCounter file in getTradeCounter" << endl;
		// system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

void incrimentTradeCounter()
{
	float currentTradeCounter = getTradeCounter();
	ofstream out;
	out.open("tradeCounter.txt");
	if (out.is_open())
	{
		currentTradeCounter++;
		out << currentTradeCounter << endl;
		out.close();
	}
	else
	{
		cerr << "failed on opening tradeCounter file in incrimentation" << endl;
		// system ("echo failed on opening tradeCounter file | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

float calcNetWorth(vector<Stock*> stocks)
{
	float netWorth = 0.000;
	for (int i = 0; i < stocks.size(); i++)
	{
		Stock* currStock = stocks[i];
		float totalWorth = (currStock->currVal * currStock->currStockHeld) + currStock->remainingCash;		
		netWorth += totalWorth;
	}	
	return (netWorth - (getTradeCounter() * TRADE_COST));
}

float scrapeForCurrVal(string filename)
{
	ifstream filetool;
        filetool.open(filename);
	int finderCounter = 0;
        if (filetool.is_open())
        {
                string line = "";
                while (getline(filetool, line))
                {
                        if (line.length() >= 7)
                        {
                                for (int i = 0; i < line.length(); i++)
                                {
                                        if (line[i] == 'c' && line[i+1] == 'e' && line[i+2] == '"' && line[i+3] == ':' && line[i+4] == ' ' && line[i+5] == '"')
                                        {
						// cout << "found" << endl;
						finderCounter++;
						if (finderCounter > ITERATIONS_FOR_CURRVAL_TRACKER)
						{			
                                                	string finalString = "";
                                                	int counter = i + 6;
						// 	cout << "line[i+6]: " << line[i+6] << endl;
						// 	exit(0);
                                                	while (isANumber(line[counter]) == true)
                                                	{
                                                	        finalString += line[counter];
                                                	        counter++;
                                                	}
                                                	return stof(finalString);
						}
                                        }
                                }
                        }
                }
        }
        else
        {
                cerr << "Failed at go" << endl;
		// system ("echo failed at scraping for currVal | ssmtp 9702195822@vtext.com");
                exit(22);
        }
        filetool.close();
}

string runCommand(const char* cmd)
{
	array<char, 128> buffer;
	string result;
	unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe)
	{
		throw runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
       		result += buffer.data();
	}
	return result;
}

float pullCurrVal(string input)
{
	string curlCommand = "curl " + input + " > output.txt";
	const char* cmd = curlCommand.c_str();
	string returner = runCommand(cmd);
	float returnedVal = scrapeForCurrVal("output.txt");
	return returnedVal;
}

void fillVectsFile(vector<Stock*> &stocks, string filename)
{
	ifstream filetool;
	filetool.open(filename);
	if (filetool.is_open())
	{
		string line = "";
		bool bypass = true;
		while (getline(filetool, line))
		{
			if (bypass)
			{
				string tmp[ARRAY_ELEMENTS];
				istringstream ss(line);
        			string maker;
        			int counterForStringStream = 0;
        			while(getline(ss,maker,','))
       		 		{
       		 		        tmp[counterForStringStream] = maker;
       		 		        counterForStringStream++;
       		 		}	
				if (!isANumber(line[0]))
				{
					Stock* newStock = new Stock;
					newStock->name = tmp[0];
					newStock->url = tmp[1];
					newStock->type = tmp[2];
					if (validTime())
					{
						newStock->currVal = pullCurrVal(newStock->url);
					}
					else
					{
						newStock->currVal = stof(tmp[3]);
					}
					newStock->currStockHeld = stof(tmp[4]);
					newStock->remainingCash = stof(tmp[5]);
					stocks.push_back(newStock);
				}
			}
		}		
	}
	else
	{
		cerr << "Failed at locating sheet" << endl;
		// system ("echo failed at locating sheet | ssmtp 9702195822@vtext.com");
		exit(22);
	}
}

float percentageDifference(float initial, float latter)
{
	float difference = latter - initial;
	return difference/initial;
}

void readPrevValues(vector<float> &longValues)
{
	ifstream in;
	in.open("longPrevValues.csv");
	if (in.is_open())
	{
                string line = "";
                while (getline(in, line))
                {
			longValues.push_back(stof(line));	
                }
	}
	else
	{
		cerr << "Failed on opening long prev values in readPrevValues" << endl;
		exit(15);
	}
	in.close();
}

void writePrevValues(vector<Stock*> stocks)
{
	ofstream out;
	out.open("longPrevValues.csv");
	if (out.is_open())
	{
		for (int i = 0; i < stocks.size(); i++)
		{
			out << fixed << setprecision(5) << (stocks[i]->currVal * stocks[i]->currStockHeld) + stocks[i]->remainingCash << endl;		
		}
	}
	else
	{
		cerr << "Failed on opening long prev values in writePrevValues" << endl;
		exit(15);
	}
	out.close();
}

void displayStocks(vector<Stock*> stocks, vector<float> &longPrevVals)
{
	readPrevValues(longPrevVals);
	string daily = "dailyNetWorth.txt";
	string name = "netWorth.txt";
	string url;
	string type;
	float currVal;
	float currStockHeld;
	float remainingCash;
	bool writerDeterminer = false;
	cout << endl;
	cout << "-------------------------------- LONG OVERVIEW ------------------------------" << endl;
	if (stocks.size() == 0)
	{
		cout << "No long positions available for report" << endl;
		cout << "----------------------------------------------------------------------------" << endl;
		
	}
	for (int i = 0; i < stocks.size(); i++)
	{
		writerDeterminer = true;
		cout << " - "; 
		Stock* currStock = stocks[i];
		cout << fixed << setprecision(2) << currStock->name << " (" << uppercase(extractTicker(currStock->url)) << "): " << endl;
		cout << fixed << setprecision(2) << "   - Current Value: " << currStock->currVal << endl;
		cout << fixed << setprecision(2) << "   - Shares Held: " << currStock->currStockHeld << endl;
		float totalWorth = (currStock->currVal * currStock->currStockHeld) + currStock->remainingCash;
		cout << fixed << setprecision(2) << "   - Total Worth of Investment: " << "$" << totalWorth << " (";
		if ((((currStock->currVal * currStock->currStockHeld) + currStock->remainingCash) - longPrevVals[i]) >= 0.000000000)
		{
			// cout << "+$" << (longPrevVals[i] - ((currStock->currVal * currStock->currStockHeld) + currStock->remainingCash)) << ")" << endl;
			cout << "+$" << (((currStock->currVal * currStock->currStockHeld) + currStock->remainingCash) - longPrevVals[i]) << ")" << endl;
		}
		else
		{
			// cout << "-$" << -1*(longPrevVals[i] - ((currStock->currVal * currStock->currStockHeld) + currStock->remainingCash)) << ")" << endl;
			cout << "-$" << -1*(((currStock->currVal * currStock->currStockHeld) + currStock->remainingCash) - longPrevVals[i]) << ")" << endl;
		}
		cout << fixed << setprecision(2) << "   - Remaining Cash: $" << currStock->remainingCash << endl;
		if (i == stocks.size() - 1)
		{
			cout << endl;
			bool tinyDeterminer = false;
			cout << " - - - - - - STOCKS CURRENTLY HELD - - - - - - " << endl;
			for (int i = 0; i < stocks.size(); i++)
			{
				if (stocks[i]->currStockHeld >= .5)
				{
					tinyDeterminer = true;
					cout << "   - " << stocks[i]->name << " ($" << stocks[i]->currStockHeld * stocks[i]->currVal << " investment)" << endl;
				}
			}
			if (tinyDeterminer == false)
			{
				cout << " No stocks' investments large enough to report" << endl;
			}
			cout << " - - - - - - - - - - - - - - - - - - - - - - - " << endl;
			cout << "----------------------------------------------------------------------------" << endl;
			break;
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;
	cout << endl;
	float netPocket = 0.00;
	for (int i = 0; i < stocks.size(); i++)
	{
		netPocket += stocks[i]->remainingCash;
	}
	cout << "------------------------------ WORTH OVERVIEW ------------------------------" << endl;
	cout << " - Pocket Values: " << endl;
	cout << "   - Total Net Worth In Pocket: $" << netPocket << endl;
	cout << " - Gross/Net Values: " << endl;
	cout << "   - Total Net Worth: $" << calcNetWorth(stocks);
	float difference = calcNetWorth(stocks) - getNetWorth(name);
	if (calcNetWorth(stocks) > getNetWorth(name))
	{
		cout << ", a $" << difference << " increase from last upload" << endl;
	}
	else if (calcNetWorth(stocks) < getNetWorth(name))
	{
		cout << ", a $" << -1*difference << " decrease from last upload" << endl;
	}
	else
	{
		cout << ", no change from last upload" << endl;
	}
	writeNetWorth(calcNetWorth(stocks), name);
	cout << " - Historic Overview: " << endl;
	cout << "   - Net Worth has ";
	if (getNetWorth(name) > getNetWorth(daily))
	{
		cout << "increased by $" << getNetWorth(name) - getNetWorth(daily) << " since last log, ";
	}
	else if (getNetWorth(name) < getNetWorth(daily))
	{
		cout << "decreased by $" << (-1) * (getNetWorth(name) - getNetWorth(daily)) << " since last log, ";
	}
	else
	{
		cout << "no reported change since last log, ";
	}
	float overallProfit = getNetWorth(name) - STARTING_CASH;
	if (overallProfit  > 0)
	{
		cout << "and increased by $" << overallProfit << " (+" << (overallProfit/100000)*100 << "\%) overall" << endl;
	}
	else
	{
		cout << "and decreased by $" << -1*overallProfit << " (-" << (-1*overallProfit/100000)*100 << "\%) overall" << endl;
	}
	cout << "----------------------------------------------------------------------------" << endl;
	if (writerDeterminer == true)
	{
		writePrevValues(stocks);
	}
}

Stock* findIt(string input, vector<Stock*> stocks)
{
	while (1 > 0)
	{
		for (int i = 0; i < stocks.size(); i++)
		{
			if (lowercase(stocks[i]->name) == lowercase(input))
			{
				return stocks[i];
			}
		}
		cout << "unfortunately that stock does not exist, please try again: ";
		cin >> input;
	}
}

bool doesItExist(string input, vector<Stock*> stocks)
{
	for (int i = 0; i < stocks.size(); i++)
	{
		if (lowercase(stocks[i]->name) == lowercase(input))
		{
			return true;
		}
	}
	return false;
}

Stock* findStock(vector<Stock*> &stocks, bool determiner, bool &checker)
{
	string stock;
	Stock* returner = nullptr;
	cout << endl;
	if (!validTime())
	{
		checker = false;
		Stock* newStock = new Stock;
		string name; 
		cout << "Enter the stock's name: "; 
		getline(cin >> ws, name);
		newStock->name = name;
		string ticker; cout << "Enter the stock's symbol: "; cin >> ticker; newStock->url = buildUrl(ticker);
		string type; cout << "Enter the stock's type: "; cin >> type; newStock->type = type;
		newStock->currVal = pullCurrVal(newStock->url);
		newStock->currStockHeld = 0;
		newStock->remainingCash = 0;
		stocks.push_back(newStock);
		return newStock;
	}
	cout << "Enter the name of a stock to examine, or \"new\" to buy a new stock: " << endl;;
	if (determiner == true)
	{
		for (int i = 0; i < stocks.size(); i++)
		{
			cout << "  - " << stocks[i]->name << " ($" << stocks[i]->remainingCash << ")" << endl;
		}
	}
	else
	{
		for (int i = 0; i < stocks.size(); i++)
		{
			cout << "  - " << stocks[i]->name << " (" << stocks[i]->currStockHeld << ")" << endl;
		}
	}
        getline(cin >> ws, stock);
	while (!doesItExist(stock, stocks) && stock != "new")
	{
		cout << "unfortunately that stock was not found, please try again: ";
		cin >> stock;	
	}
	if (stock != "new")
	{
		for (int i = 0; i < stocks.size(); i++)
		{
			if (lowercase(stocks[i]->name) == lowercase(stock))
			{
				return stocks[i];	
			}
		}
	}
	else if (stock == "new" && determiner == true)
	{
		checker = false;
		Stock* newStock = new Stock;
		string name; 
		cout << "Enter the stock's name: "; 
		getline(cin >> ws, name);
		newStock->name = name;
		string ticker; cout << "Enter the stock's symbol: "; cin >> ticker; newStock->url = buildUrl(ticker);
		string type; cout << "Enter the stock's type: "; cin >> type; newStock->type = type;
		newStock->currVal = pullCurrVal(newStock->url);
		newStock->currStockHeld = 0;
		newStock->remainingCash = 0;
		stocks.push_back(newStock);
		return newStock;
	}
	else
	{
		cout << "unfortunately you cannot buy what you cannot have" << endl;
	}
}

bool transferChecker(vector<Stock*> stocks, string input)
{
	for (int i = 0; i < stocks.size(); i++)
	{
		if (stocks[i]->name == input)
		{
			return true;
		}
	}
	return false;
}

void transferFunc(vector<Stock*> stocks)
{
	Stock* withdrawnStock;
	Stock* depositedStock;
	string withdrawnStockName;
	string depositedStockName;
	float transferedAmount;
	cout << "enter the stock to be withdrawn from: " << endl;
	for (int i = 0; i < stocks.size(); i++)
	{
		cout << fixed << setprecision(2) << "  - " << stocks[i]->name << " ($" << stocks[i]->remainingCash << ")" << endl;
	}
        getline(cin >> ws, withdrawnStockName);
	while (!transferChecker(stocks, withdrawnStockName))
	{
		cout << "unfortunately \"" << withdrawnStockName << "\" was not found. please try again: " << endl;
		getline(cin >> ws, withdrawnStockName);
	}
	withdrawnStock = findIt(withdrawnStockName, stocks);
	cout << "enter the stock to be deposited in to: " << endl;
	for (int i = 0; i < stocks.size(); i++)
	{
		cout << fixed << setprecision(2) << "  - " << stocks[i]->name << " ($" << stocks[i]->remainingCash << ")" << endl;
	}
        getline(cin >> ws, depositedStockName);
	while (!transferChecker(stocks, depositedStockName))
	{
		cout << "unfortunately \"" << depositedStockName << "\" was not found. please try again: " << endl;
		getline(cin >> ws, depositedStockName);
	}
	depositedStock = findIt(depositedStockName, stocks);
	cout << fixed << setprecision(2) << "enter the amount to be transfered (can be up to $" << withdrawnStock->remainingCash << "): ";
	cin >> transferedAmount;
	while (transferedAmount > withdrawnStock->remainingCash)
	{
		cout << "unfortunately that amount is too large and not possessed by the withdrawled stock, please enter a valid amount: ";
		cin >> transferedAmount;
	}
	withdrawnStock->remainingCash = withdrawnStock->remainingCash - transferedAmount;
	depositedStock->remainingCash = depositedStock->remainingCash + transferedAmount;
	cout << fixed << setprecision(2) << "Transfered $" << transferedAmount << " from " << withdrawnStock->name << " to " << depositedStock->name << endl;
}

void buyFunc(vector<Stock*> &stocks, bool determiner)
{
	bool lilDeterminer = true;
	Stock* examined = findStock(stocks, determiner, lilDeterminer);		
	float toBeBought;
	if (validTime() == false)
	{
		cout << "automatically purchasing zero shares due to the markets not being open" << endl;
		toBeBought = 0;
		examined->remainingCash = examined->remainingCash - (examined->currVal * toBeBought);
		examined->currStockHeld = examined->currStockHeld + toBeBought;	
		incrimentTradeCounter();
		return;
	}
	if (lilDeterminer)
	{
		cout << fixed << setprecision(2) << "enter the amount of shares to be bought (can be up to " << examined->remainingCash/examined->currVal << "): ";
		cin >> toBeBought;
		while (toBeBought >= examined->remainingCash/examined->currVal && toBeBought < 0)
		{
			cout << "amount not valid. please try again: ";
			cin >> toBeBought;
		}
		examined->remainingCash = examined->remainingCash - (examined->currVal * toBeBought);
		examined->currStockHeld = examined->currStockHeld + toBeBought;	
		incrimentTradeCounter();
	}
	cout << "Purhcased $" << examined->currStockHeld * examined->currVal << " worth of " << examined->name << endl;
}

void sellFunc(vector<Stock*> &stocks, bool determiner)
{
	bool lilDeterminer = true;
	Stock* examined = findStock(stocks, determiner, lilDeterminer);	
	float toBeSold;
	float prevStockHeld = examined->currStockHeld;
	if (lilDeterminer)
	{
		cout << fixed << setprecision(2) << "enter the amount of shares to be sold (can be up to " << examined->currStockHeld << "): ";
		cin >> toBeSold;
		while (toBeSold >= examined->currStockHeld && toBeSold < 0)
		{
			cout << "amount not valid. please try again: " << endl;
			cin >> toBeSold;
		}
		examined->remainingCash = examined->remainingCash + (examined->currVal * toBeSold);
		examined->currStockHeld = examined->currStockHeld - toBeSold;	
		incrimentTradeCounter();
	}
	cout << "Sold $" << toBeSold * examined->currVal << " worth of " << examined->name << endl;
}

void printMenu()
{
	if (validTime())
	{
		cout << "------ MENU ------ " << endl;
		cout << " - Examine" << endl;
		cout << " - Buy" << endl;
		cout << " - Sell" << endl;
		cout << " - Transfer" << endl;
		cout << " - Log" << endl;
		cout << " - Update" << endl;
	}
	else
	{
		cout << "------ MENU ------ " << endl;
		cout << " - Examine" << endl;
		cout << " - Prepare" << endl;
		cout << " - Transfer" << endl;
		cout << " - Log" << endl;
	}
	cout << "Enter your choice here: ";
}

void menu(vector<Stock*> &stocks, vector<float> longPrevVals)
{
	string writingName = "dailyNetWorth.txt";
	string modeChoice;
	bool determiner;
	printMenu();
	cin >> modeChoice;
	modeChoice = lowercase(modeChoice);
	while (modeChoice != "examine" && modeChoice != "buy" && modeChoice != "sell" && modeChoice != "transfer" && modeChoice != "short" && modeChoice != "log" && modeChoice != "prepare" && modeChoice != "update")
	{
		cout << "try again" << endl;
		cin >> modeChoice;
		modeChoice = lowercase(modeChoice);
	}
	if (modeChoice == "examine")
	{
		displayStocks(stocks, longPrevVals);
	}
	else if (modeChoice == "buy" || modeChoice == "prepare")
	{
		determiner = true;
		buyFunc(stocks, determiner);
	}
	else if (modeChoice == "sell")
	{
		if (validTime())
		{
			determiner = false;
			sellFunc(stocks, determiner);
		}
		else
		{
			cout << "unfortunately the markets are closed at this time" << endl;
		}
	}
	else if (modeChoice == "transfer")
	{
		transferFunc(stocks);
	}
	else if (modeChoice == "log")
	{
		writeNetWorth(calcNetWorth(stocks), writingName);
		cout << fixed << setprecision(2) << "logged $" << calcNetWorth(stocks) << " as the new benchmark net worth" << endl;
	}
	else
	{
		if (!validTime())
		{
			cout << "unfortunately the portfolio cannot be updated due to the markets being closed at this time" << endl;
		}
		else
		{
			for (int i = 0; i < stocks.size(); i++)
			{
				Stock* currStock = stocks[i];
				currStock->currVal = pullCurrVal(currStock->url);
			}
			displayStocks(stocks, longPrevVals);
		}
	}
	ofstream filetool;
	filetool.open("portfolio.csv");
        for (int i = 0; i < stocks.size(); i++)
        {
                filetool << fixed << setprecision(5) << stocks[i]->name << ",";
                filetool << fixed << setprecision(5) << stocks[i]->url << ",";
                filetool << fixed << setprecision(5) << stocks[i]->type << ",";
                filetool << fixed << setprecision(5) << stocks[i]->currVal << ",";
                filetool << fixed << setprecision(5) << stocks[i]->currStockHeld << ",";
                filetool << fixed << setprecision(5) << stocks[i]->remainingCash << endl;
        }
        filetool.close();
}

int main(int argc, const char* argv[])
{	
	vector<Stock*> masterStocks;
	vector<float> longPrevVals;
	string filename = "portfolio.csv";
	fillVectsFile(masterStocks, filename);
	readPrevValues(longPrevVals);
	header();
	menu(masterStocks, longPrevVals);
	char choice;
	cout << "run another transaction? (y/n): ";
	cin >> choice;
	while (choice != 'n')
	{
		menu(masterStocks, longPrevVals);
		cout << "run another transaction? (y/n): ";
		cin >> choice;
	}
	cout << "Save? (y/n): ";
	cin >> choice;
	while (choice != 'y' && choice != 'n')
	{
		cout << "try again: ";
	}
	if (choice == 'y')
	{
		system("cp portfolio.csv assets/portfolioBackup.csv");
		system("cp shortBank.txt assets/shortBankBackup.txt");
		system("cp netWorth.txt assets/netWorthBackup.txt");
		system("cp tradeCounter.txt assets/tradeCounterBackup.txt");
		system("cp longPrevValues.csv assets/longPrevValuesBackup.csv");
		system("cp game.cpp assets/gameBackup.cpp");
		system("scp -r ../../trading/ pi@172.16.0.250:."); 
	}
}






