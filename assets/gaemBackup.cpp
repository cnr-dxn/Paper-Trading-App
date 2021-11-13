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

struct shortStock {
	string name;
	string url;
	string type;
	float currVal;
	float currLoaned;
	float priceAtLoan;
};

bool validTime()
{
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
	for (int i = 0; i < 40; i++)
	{
		cout << endl;
	}
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

void incrimentShortBank(float addition)
{
	float currBankVal = getShortBank();
	ofstream out;
	out.open("shortBank.txt");
	if (out.is_open())
	{
		currBankVal = currBankVal += addition;
		out << currBankVal << endl;
		out.close();
	}
	else
	{
		cerr << "failed on opening shortBank file in incrimentation" << endl;
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

void fillVectsFileShort(vector<shortStock*> &stocks, string filename)
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
					shortStock* newStock = new shortStock;
					newStock->name = tmp[0];
					newStock->url = tmp[1];
					newStock->type = tmp[2];
					newStock->currVal = pullCurrVal(newStock->url);
					newStock->currLoaned = stof(tmp[4]);
					newStock->priceAtLoan = stof(tmp[5]);
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

float getNetComponent(vector<shortStock*> shorts)
{
	float netComponent = 0.00;
	for (int i = 0; i < shorts.size(); i++)
	{
		shortStock* currStock = shorts[i];
		netComponent += (currStock->priceAtLoan * currStock->currLoaned) - (currStock->currVal * currStock->currLoaned);
	}
	return netComponent;
}

void displayStocks(vector<Stock*> stocks, vector<shortStock*> shortStocks)
{
	string daily = "dailyNetWorth.txt";
	string name = "netWorth.txt";
	string url;
	string type;
	float currVal;
	float currStockHeld;
	float remainingCash;
	cout << endl;
	cout << "--------------- LONG OVERVIEW ---------------" << endl;
	for (int i = 0; i < stocks.size(); i++)
	{
		cout << " - "; 
		Stock* currStock = stocks[i];
		cout <<  currStock->name << " (" << uppercase(extractTicker(currStock->url)) << "): " << endl;
		cout << fixed << setprecision(2) << "   - Current Value: " << currStock->currVal << endl;
		cout << fixed << setprecision(2) << "   - Shares Held: " << currStock->currStockHeld << endl;
		float totalWorth = (currStock->currVal * currStock->currStockHeld) + currStock->remainingCash;
		cout << fixed << setprecision(2) << "   - Total Worth of Investment: " << "$" << totalWorth << endl;
		cout << fixed << setprecision(2) << "   - Remaining Cash: " << "$" << currStock->remainingCash << endl;
		if (i == stocks.size() - 1)
		{
			cout << "----------------------------------------------" << endl;
			break;
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;
	cout << endl;
	cout << "--------------- SHORT OVERVIEW ---------------" << endl;
	float grossComponent = 0.00;
	float netComponent = 0.00;
	if (shortStocks.size() == 0)
	{
		cout << "No short positions available for report" << endl;
		cout << "----------------------------------------------" << endl;
	}
	for (int i = 0; i < shortStocks.size(); i++)
	{
		cout << " - "; 
		shortStock* currStock = shortStocks[i];
		cout <<  currStock->name << ": " << endl;
		cout << fixed << setprecision(2) << "   - Current Value: " << currStock->currVal << endl;
		cout << fixed << setprecision(2) << "   - Shares Loaned: " << currStock->currLoaned << endl;
		cout << fixed << setprecision(2) << "   - Price At Loan: " << "$" << currStock->priceAtLoan << endl;
		grossComponent += (currStock->priceAtLoan * currStock->currLoaned);
		float totalProfit = (currStock->priceAtLoan * currStock->currLoaned) - (currStock->currVal * currStock->currLoaned);
		cout << fixed << setprecision(2) << "   - Total Profit If Returned Now: " << "$" << totalProfit << endl;
		if (i == shortStocks.size() - 1)
		{
			cout << "----------------------------------------------" << endl;
			break;
		}
		cout << endl;
		cout << endl;
	}
	cout << endl;
	cout << endl;
	cout << endl;
	cout << "--------------- WORTH OVERVIEW ---------------" << endl;
	cout << " - Total Gross Worth: $" << calcNetWorth(stocks) + grossComponent + getShortBank() << endl;
	cout << " - Total Net Worth: $" << calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank();
	float difference = (calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()) - getNetWorth(name);
	if ((calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()) > getNetWorth(name))
	{
		cout << ", a $" << difference << " increase from last upload" << endl;
	}
	else if ((calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()) < getNetWorth(name))
	{
		cout << ", a $" << -1*difference << " decrease from last upload" << endl;
	}
	else
	{
		cout << ", no change from last upload" << endl;
	}
	writeNetWorth((calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()), name);
	cout << " - Net Worth has ";
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
	float overallProfit = getNetWorth(name) - 100000;
	if (overallProfit  > 0)
	{
		cout << "and increased by $" << overallProfit << " (+" << (overallProfit/100000)*100 << "\%) overall" << endl;
	}
	else
	{
		cout << "and decreased by $" << -1*overallProfit << " (-" << (-1*overallProfit/100000)*100 << "\%) overall" << endl;
	}
	cout << "----------------------------------------------" << endl;
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
		cout << "unfortunately that stock does not exist, please try again: " << endl;
		cin >> input;
	}
}

shortStock* findItShort(string input, vector<shortStock*> stocks)
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
		cout << "unfortunately that stock does not exist, please try again: " << endl;
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

bool doesItExistShort(string input, vector<shortStock*> stocks)
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

Stock* findStock(vector<Stock*> &stocks, bool determiner)
{
	string stock;
	Stock* returner = nullptr;
	cout << endl;
	if (!validTime())
	{
		Stock* newStock = new Stock;
		string name; 
		cout << "Enter the stock's name: " << endl; 
		getline(cin >> ws, name);
		newStock->name = name;
		string ticker; cout << "Enter the stock's symbol: " << endl; cin >> ticker; newStock->url = buildUrl(ticker);
		string type; cout << "Enter the stock's type: " << endl; cin >> type; newStock->type = type;
		newStock->currVal = pullCurrVal(newStock->url);
		newStock->currStockHeld = 0;
		newStock->remainingCash = 0;
		stocks.push_back(newStock);
		return newStock;
	}
	cout << "Enter the name of a stock to examine, or \"new\" to buy a new stock: " << endl;
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
		cout << "unfortunately that stock was not found, please try again: " << endl;
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
		Stock* newStock = new Stock;
		string name; 
		cout << "Enter the stock's name: " << endl; 
		getline(cin >> ws, name);
		newStock->name = name;
		string ticker; cout << "Enter the stock's symbol: " << endl; cin >> ticker; newStock->url = buildUrl(ticker);
		string type; cout << "Enter the stock's type: " << endl; cin >> type; newStock->type = type;
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

shortStock* findShortStock(vector<shortStock*> &shortStocks)
{
	string stock;
	cout << endl;
	cout << "Enter the name of a stock to remove it's short position: " << endl;
	for (int i = 0; i < shortStocks.size(); i++)
	{
		cout << "  - " << shortStocks[i]->name << endl;
	}
        getline(cin >> ws, stock);
	while (!doesItExistShort(stock, shortStocks))
	{
		cout << "unfortunately that stock was not found, please try again: " << endl;
		cin >> stock;	
	}
	return findItShort(stock, shortStocks);
}

void shorting(vector<shortStock*> &shortStocks)
{
	// shortStock* shorter = findShortStock(shortStocks);
	string modeChoice;
	cout << "add or remove short positions?" << endl;
	cin >> modeChoice;
	modeChoice = lowercase(modeChoice);
	while (modeChoice != "add" && modeChoice != "remove")
	{
		cout << "try again" << endl;
		cin >> modeChoice;
		modeChoice = lowercase(modeChoice);
	}
	if (modeChoice == "add")
	{
		shortStock* newStock = new shortStock;
		string name; 
		cout << "Enter the stock's name: " << endl; 
		getline(cin >> ws, name);
		newStock->name = name;
		// string url; cout << "Enter the stock's url: " << endl; cin >> url; newStock->url = url;
		string ticker; cout << "Enter the stock's symbol: " << endl; cin >> ticker; newStock->url = buildUrl(ticker);
		string type; cout << "Enter the stock's type: " << endl; cin >> type; newStock->type = type;
		newStock->currVal = pullCurrVal(newStock->url);
		float currLoaned; cout << "Enter the amount to be shorted: " << endl; cin >> currLoaned; newStock->currLoaned = currLoaned;
		newStock->priceAtLoan = newStock->currVal;
		shortStocks.push_back(newStock);
		incrimentTradeCounter();
	}
	else
	{
		// removeShorts(shortStocks);
		string stock;
		cout << "Enter the name of a stock to remove it's short position: " << endl;
       		getline(cin >> ws, stock);
		while (!doesItExistShort(stock, shortStocks))
		{
			cout << "unfortunately that stock was not found, please try again: " << endl;
			cin >> stock;	
		}
		shortStock* examined = findItShort(stock, shortStocks);
		float profit = 0.00;
		incrimentShortBank((examined->priceAtLoan * examined->currLoaned) - (examined->currVal * examined->currLoaned));
		examined->currLoaned = 0;
		examined->priceAtLoan = 0;
		if (examined->currLoaned == 0 && examined->priceAtLoan == 0)
		{
			cout << "returned " << examined->name << "'s short position" << endl;
		}
		else
		{
			cerr << "wtf" << endl;
			exit(15);
		}
		incrimentTradeCounter();
	}
	ofstream filetool;
	filetool.open("shortPortfolio.csv");
        for (int i = 0; i < shortStocks.size(); i++)
        {
                filetool << fixed << setprecision(5) << shortStocks[i]->name << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->url << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->type << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->currVal << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->currLoaned << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->priceAtLoan << endl;
        }
        filetool.close();
}

void transferFunc(vector<Stock*> stocks)
{
	Stock* withdrawnStock;
	Stock* depositedStock;
	string withdrawnStockName;
	string depositedStockName;
	float transferedAmount;
	cout << "enter the stock to be withdrawn from: " << endl;
        getline(cin >> ws, withdrawnStockName);
	withdrawnStock = findIt(withdrawnStockName, stocks);
	cout << "enter the stock to be deposited in to: " << endl;
        getline(cin >> ws, depositedStockName);
	depositedStock = findIt(depositedStockName, stocks);
	cout << "enter the amount to be transfered: " << endl;
	cin >> transferedAmount;
	while (transferedAmount > withdrawnStock->remainingCash)
	{
		cout << "unfortunately that amount is too large and not possessed by the withdrawled stock, please enter a valid amount: " << endl;
		getline(cin >> ws, depositedStockName);
	}
	withdrawnStock->remainingCash = withdrawnStock->remainingCash - transferedAmount;
	depositedStock->remainingCash = depositedStock->remainingCash + transferedAmount;
	cout << "transfered $" << transferedAmount << " from " << withdrawnStock->name << " to " << depositedStock->name << endl;
}

void buyFunc(vector<Stock*> &stocks, bool determiner)
{
	Stock* examined = findStock(stocks, determiner);		
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
	cout << fixed << setprecision(2) << "enter the amount of shares to be bought (can be up to " << examined->remainingCash/examined->currVal << "): " << endl;
	cin >> toBeBought;
	while (toBeBought > examined->remainingCash/examined->currVal && toBeBought <= 0)
	{
		cout << "amount not valid. please try again: " << endl;
		cin >> toBeBought;
	}
	examined->remainingCash = examined->remainingCash - (examined->currVal * toBeBought);
	examined->currStockHeld = examined->currStockHeld + toBeBought;	
	incrimentTradeCounter();
}

void sellFunc(vector<Stock*> &stocks, bool determiner)
{
	Stock* examined = findStock(stocks, determiner);	
	float toBeSold;
	cout << fixed << setprecision(2) << "enter the amount of shares to be sold (can be up to " << examined->currStockHeld << "): " << endl;
	cin >> toBeSold;
	while (toBeSold > examined->currStockHeld && toBeSold < 0)
	{
		cout << "amount not valid. please try again: " << endl;
		cin >> toBeSold;
	}
	examined->remainingCash = examined->remainingCash + (examined->currVal * toBeSold);
	examined->currStockHeld = examined->currStockHeld - toBeSold;	
	incrimentTradeCounter();
}

void menu(vector<Stock*> &stocks, vector<shortStock*> &shortStocks)
{
	string writingName = "dailyNetWorth.txt";
	string modeChoice;
	bool determiner;
	if (validTime())
	{
		cout << "examine, buy, sell, transfer, short or log?" << endl;
	}
	else
	{
		cout << "examine, prepare, transfer or log?" << endl;
	}
	cin >> modeChoice;
	modeChoice = lowercase(modeChoice);
	while (modeChoice != "examine" && modeChoice != "buy" && modeChoice != "sell" && modeChoice != "transfer" && modeChoice != "short" && modeChoice != "log" && modeChoice != "prepare")
	{
		cout << "try again" << endl;
		cin >> modeChoice;
		modeChoice = lowercase(modeChoice);
	}
	if (modeChoice == "examine")
	{
		displayStocks(stocks, shortStocks);
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
	else if (modeChoice == "short")
	{
		if (validTime())
		{
			shorting(shortStocks);
		}
		else
		{
			cout << "unfortunately the markets are closed at this time" << endl;
		}
	}
	else
	{
		writeNetWorth((calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()), writingName);
		cout << fixed << setprecision(2) << "logged $" << (calcNetWorth(stocks) + getNetComponent(shortStocks) + getShortBank()) << " as the new benchmark net worth" << endl;
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
	filetool.open("shortPortfolio.csv");
        for (int i = 0; i < shortStocks.size(); i++)
        {
                filetool << fixed << setprecision(5) << shortStocks[i]->name << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->url << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->type << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->currVal << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->currLoaned << ",";
                filetool << fixed << setprecision(5) << shortStocks[i]->priceAtLoan << endl;
        }
        filetool.close();
}

int main(int argc, const char* argv[])
{	
	vector<Stock*> masterStocks;
	vector<shortStock*> masterShortStocks;
	string filename = "portfolio.csv";
	string shortFilename = "shortPortfolio.csv";
	float shortBank = getShortBank();
	fillVectsFile(masterStocks, filename);
	fillVectsFileShort(masterShortStocks, shortFilename);
	header();
	menu(masterStocks, masterShortStocks);
	char choice;
	cout << "run another transaction? (y/n): " << endl;
	cin >> choice;
	while (choice != 'n')
	{
		menu(masterStocks, masterShortStocks);
		cout << "run another transaction? (y/n): " << endl;
		cin >> choice;
	}
	cout << "Save? (y/n)" << endl;
	cin >> choice;
	while (choice != 'y' && choice != 'n')
	{
		cout << "try again" << endl;
	}
	if (choice == 'y')
	{
		system("cp portfolio.csv assets/portfolioBackup.csv");
		system("cp shortPortfolio.csv assets/shortPortfolioBackup.csv");
		system("cp game.cpp assets/gaemBackup.cpp");
	}
}






