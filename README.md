# Paper Trading Application

A terminal based paper trading application that allows for real-time buying, selling and shorting of stocks and securities in the stock market. Made in C++, this application was designed around efficiency and timing, rather than ease of programming, to allow for the most accurate transactions possible. 

**Object Oriented Programming**: This algorithm utilizes dynamically allocated object-oriented programming to increase efficiency and ensure each trade placed utilizes the most real time data. 

**Web Scraping**: In order to allow for authentic, real time trading, a web-scraping algorithm had to be built in. This algorithm consists of a series of cURL manipulations and data manipulations to take values from the scraped website to then be stored in a series of data structures. 

**User Friendly Interface**: The UI of this application allows for accessibility without sacrificing utility, allowing for both investing and shorting all from the main interface. 

## How to run this application:

This is run as a C++ application, and thus to allow correct access to existing credientials to the necessary executables, run the following code:
```
g++ -o game game.cpp
```
This will compile the necessary code into an executable titled main, so to run simply execute it:
```
./game
```
Then just simply follow the directions output to the screen! 

## File Structure

**game.cpp** is where all the main functionality is written.

**portfolio.csv** is where the user's current portfolio is stored

**netWorth.txt** is where the netWorth is stored

## Notes

The website used for this application has since changed their website architecture, and thus the scraping feature is currently offline and is being fixed.

