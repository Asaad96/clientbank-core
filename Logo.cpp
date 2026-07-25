#include <iostream>

using namespace std;

// تعريف ألوان التيرمينال
#define RESET       "\033[0m"
#define WHITE_TEXT  "\033[37m"
#define RED_TEXT    "\033[31m"
#define GREEN_TEXT  "\033[32m"
#define BG_BLACK    "\033[40m"


void PrintColoredLogo() {
    cout << "\n";
    cout << WHITE_TEXT;
    cout << "                                                                       \n" ; 
    cout << "                         ⬜⬜⬜                                                       "  << GREEN_TEXT <<"    |                ▀█▀▀▄              ▀█         ▄▀▀▀▄            █               " << endl;
    cout << "                         ⬜⬜⬜                                                       "  <<"    |                 █▄▄▀  ▀▀▀▄  █▄▀▀▄  █ ▄▀      ▀▄▄▄  █  █ ▄▀▀▀ ▀█▀▀ ▄▀▀▀▄ █▀▄▀▄ " << endl;
    cout << "                         ⬜⬜⬜                                                       "  <<"    |                 █  █  ▄▀▀█  █   █  █▀▄       ▄   █ ▀▄▄█  ▀▀▄  █ ▄ █▀▀▀▀ █ █ █ " << endl;
    cout << "                  ⬜⬜⬜       ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜                  " <<"           |                 ▀▀▀▀   ▀▀ ▀ ▀   ▀ ▀▀  ▀       ▀▀▀     █ ▀▀▀    ▀   ▀▀▀  ▀ ▀ ▀ " << endl;
    cout << "                  ⬜⬜⬜       ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜                 " <<"            |                                                     ▀▀▀                        " << endl;
    cout << "                  ⬜⬜⬜       ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜                 " <<"            |              -------------------------------------------------------------------" << endl;
    cout << "                  ⬜⬜⬜  1  0 1  0  0   ⬜⬜⬜⬜ 1  0     ⬜⬜⬜               "<<"          |                -Next-Generation Secure Core Banking Solution " << endl ;
    cout << "                  ⬜⬜⬜     0 1  0      " << GREEN_TEXT << "DDDDDDDD" << WHITE_TEXT << "    0 1   ⬜⬜⬜     " << GREEN_TEXT"                    |                -[INFO] Initializing secure cryptographic modules...\n";
    cout << "                  ⬜⬜⬜                 " << GREEN_TEXT << "DD    DD" << WHITE_TEXT << "             ⬜⬜⬜⬜     "<<GREEN_TEXT"               |                -[INFO] Establishing encrypted connection with core database... \n"; 
    cout << "                  ⬜⬜⬜  1    0  1    0 " << GREEN_TEXT << "DD    DD" << WHITE_TEXT << "    1  1 1 0 ⬜⬜⬜⬜     "<<GREEN_TEXT"               |                -[STATUS] System operational. Please authenticate to proceed..\n";
    cout << "                  ⬜⬜⬜  1  0 1  0  0 1 " << GREEN_TEXT << "DDDDDDDD" << WHITE_TEXT << " 0 1   1 0 1 ⬜⬜⬜⬜     "<<GREEN_TEXT"               |              ##################################################################### \n" ;
    cout << "                  ⬜⬜⬜                 " << GREEN_TEXT << "EEEEEEEE" << WHITE_TEXT << "             ⬜⬜⬜⬜     "<<GREEN_TEXT"               |              ##################################################################### \n"  ;
    cout << "                  ⬜⬜⬜⬜ " << RED_TEXT << "▼▼▼▼▼▼▼▼▼" << WHITE_TEXT << "⬜⬜" << GREEN_TEXT << " EE      " << WHITE_TEXT << "  0  1  0  1 ⬜⬜⬜⬜"<<"                    |                * Quality & Security: Fully secured architecture protecting transactions.\n";  
    cout << "                  ⬜⬜⬜⬜█" <<  RED_TEXT << "▼▼▼▼▼▼▼" << WHITE_TEXT << "██⬜⬜" << GREEN_TEXT << " EEEEEEEE" << WHITE_TEXT << "             ⬜⬜⬜⬜"<<"                    |                * Strategic Goals: Delivering fast, precise, and reliable digital banking. \n";
    cout << "                  ⬜⬜⬜⬜███" << RED_TEXT << "▼▼▼▼▼" << WHITE_TEXT << "██⬜⬜" << GREEN_TEXT << " EE      " << WHITE_TEXT << "  0 10  0 1  ⬜⬜⬜⬜"<<"                    |                * Vision: Building your financial future with supreme trust & innovation. \n";
    cout << "                  ⬜⬜⬜⬜████" << RED_TEXT << "▼▼▼" << WHITE_TEXT << "███⬜⬜" << GREEN_TEXT << " EEEEEEEE" << WHITE_TEXT << "  0 10  0 1  ⬜⬜⬜⬜"<<"                    |              //////////////////////////////////////////////////\n";
    cout << "                  ⬜⬜⬜⬜⬜███" << RED_TEXT << "▼" << WHITE_TEXT << "████⬜⬜" << GREEN_TEXT << " VVV  VVV" << WHITE_TEXT << "  0  1  0 1  ⬜⬜⬜⬜"<<"                    |                                         //////////////////////////////////////////////////\n";
    cout << "                  ⬜⬜⬜      1 0       " << GREEN_TEXT << "  vv  vv" << WHITE_TEXT << "            1 ⬜⬜⬜⬜       "<<"             |                                  CopyRight © Designed by Asaad Baz\n";
    cout << "                  ⬜⬜⬜                " << GREEN_TEXT << "  vv  vv" << WHITE_TEXT << "              ⬜⬜⬜⬜     "<<"               | \n";
    cout << "                  ⬜⬜⬜          0   0 " << GREEN_TEXT << "  vv  vv " << WHITE_TEXT << " 1 01      ⬜⬜⬜    "<<"                    | \n";
    cout << "                  ⬜⬜⬜  1  0 1  0  0  " << GREEN_TEXT << "    vv  " << WHITE_TEXT << "  0         ⬜⬜⬜    "<<"                    | \n";
    cout << "                  ⬜⬜⬜  1        ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜   0                 "<<"        | \n";
    cout << "                  ⬜⬜⬜  1  0 10  ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜                   " <<"          | \n";
    cout << "                  ⬜⬜⬜           ⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜                     " <<"        | \n";
    
    cout << RESET << "\n";
}



