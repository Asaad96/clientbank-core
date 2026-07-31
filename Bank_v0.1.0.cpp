#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include "Log.h"


#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif



using namespace std;

const string ClientsFileName = "/Users/asaadmbaz/Downloads/Cprojects/Project_Bank/clientbank-core/Clients.text";
const string UsersFileName   = "/Users/asaadmbaz/Downloads/Cprojects/Project_Bank/clientbank-core/user.text";


enum Menue  {  Show = 1 , Add = 2  ,  Delete = 3 , Update = 4 , Find = 5 , Transactions = 6 , Manage = 7 , LogOut = 8 , Exit = 9 };

enum enTransactionsMenue { Deposit = 1 , Withdraw = 2 , Total  = 3 , Main  = 4 };


enum enManiMenuManageMainOptions { eListUsers = 1 , eAddUser = 2, eDeleteUser = 3, eUpdateUser = 4, eFindUser = 5, eMainMenu = 6 };

enum enMainMenuPermisions { eAll = -1, pListClients = 1 , pAddNewClient = 2, pDeletClient = 4 , pUpdateClient = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

void startMainMenu();
void LogIn();

struct sClient
{
   string AccountNumber;
   string PinCode; 
   string Name ;
   string Phone;
   double AccountBalance;
   bool MarkForDelete = false;
 };


struct sUsers
{
  string Username;
  string Password;
  short value;

};

sUsers CurrentUser;
vector <string> SplitString ( string S1 , string Delim)
{
    vector<string> vString;
   // short pos = 0;
    std::string::size_type pos = 0;
    string sWord ; 

    while ( (pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0,pos);

        if ( sWord != "")
        {
            vString.push_back(sWord);
        }
        S1.erase(0, pos + Delim.length());
    }

    if(S1 != "")

    {    
        vString.push_back(S1);
    }
    return vString ;
}



sClient ReadNewClient()
{
    sClient Client;
    
    cout << "Enter Account Number: ";
    getline(cin>> ws , Client.AccountNumber);
    cout << "Enter PinCode: ";
    getline(cin, Client.PinCode );
    cout<< "Enter Name: ";
    getline(cin, Client.Name);
    cout << "Enter Phone: ";
    getline(cin, Client.Phone);
    cout << "Enter Account Balance: ";
    cin >> Client.AccountBalance;

    return Client;
}




sClient ConverLineToRecord (string line , string Seperator = "#//#")
{
    sClient Client ;
    vector <string> vClientData;

    vClientData = SplitString(line, Seperator);
    if (vClientData.size() >= 5) 
    {
       Client.AccountNumber = vClientData[0];
       Client.PinCode =  vClientData[1];
       Client.Name = vClientData[2];
       Client.Phone = vClientData[3];
      // Client.AccountBalance = stod(vClientData[4]);
   if (!vClientData[4].empty()) {
            Client.AccountBalance = stod(vClientData[4]);
        } else {
            Client.AccountBalance = 0; // قيمة افتراضية في حال الخطأ
               }
    }
    return Client;
}

sUsers ConverLineToRecordUser (string line, string Seperator ="#//#")
{
    sUsers User;
    vector <string> vUserDate;

    vUserDate = SplitString(line, Seperator);
    if(vUserDate.size() >= 3)
    {
        User.Username = vUserDate[0];
        User.Password =vUserDate[1];
        User.value = stoi(vUserDate[2]);
    }
    return User;
      
}



vector <sClient> LoadClientDataFromFile (string FileName)
{

    vector <sClient> vClients;
    fstream MyFile;

    MyFile.open(FileName, ios::in);

        if(MyFile.is_open())
        {

            string line;
            sClient Client;


            while(getline(MyFile, line))
            {
                Client = ConverLineToRecord(line);
                    vClients.push_back(Client);
            }

            MyFile.close();
        }
    return vClients; 
}

vector <sUsers> LoadUsersDataFromFile (string FileName)
{

    vector <sUsers> vUsers;
    fstream MyFile;

    MyFile.open(FileName, ios::in);

        if(MyFile.is_open())
        {

            string line;
            sUsers Users;


            while(getline(MyFile, line))
            {
                Users = ConverLineToRecordUser(line);
                    vUsers.push_back(Users);
            }

            MyFile.close();
        }
       // cout << "DEBUG: Loaded " << vUsers.size() << " users.\n";
    return vUsers; 
}





string ConvertRecordToLine(sClient  Client , string Seperator = "#//#")
{
    string stClientRecord = "";

    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator ;
    stClientRecord += to_string(Client.AccountBalance) + Seperator;

    return stClientRecord;
}


void AddDataLineToFile(string FileName, string stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if(MyFile.is_open())
    { MyFile << stDataLine << endl;
        MyFile.close();
    }
}


void AddNewClient (  vector<sClient> &vClients)
{
    sClient Client;
    Client = ReadNewClient();
    AddDataLineToFile(ClientsFileName,ConvertRecordToLine(Client));
    vClients.push_back(Client); 
}



void ClearScreen()
{
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}


void AddClients( vector<sClient> &vClients)
{
    char AddMore = 'Y';
    do 
    {
        ClearScreen();
        cout << "Adding New Client:\n\n";
        AddNewClient(vClients);
        cout << "\nClient Added Successfuly, Do You Want to add More clients ? Y/N? \n";
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');
}


void  PrintClientRecord ( sClient Client)
{
    cout << fixed << setprecision(2);

    cout << "|  " <<  setw(15) << left << Client.AccountNumber;
    cout << "|  " <<  setw(10) << left << Client.PinCode;
    cout << "|  " <<  setw(40) << left << Client.Name ;
    cout << "|  " <<  setw(12) << left << Client.Phone;
    cout << "|  " <<  setw(12) << left << Client.AccountBalance ;
    cout << endl;

}

void  PrintClientRecordDeposit ( sClient Client)
{
    cout << fixed << setprecision(2);
    cout << "\nThe Following are the Client details:\n";
    cout << "------------------------------------------\n";
    cout << "AccountNumber   : "<< Client.AccountNumber;
    cout << "\nPinCode         : " <<Client.PinCode;
    cout << "\nName Client     : " <<Client.Name ;
    cout << "\nPhone           : " <<Client.Phone;
    cout << "\nAccount Balance : " <<  setw(12) << left << Client.AccountBalance ;
    cout << endl;
    cout << "------------------------------------------\n";
    //cout << "Please Enter Deposit Amount?\n";
    
  
}

void  PrintClientRecordBalanceList ( sClient Client)
{
    cout << fixed << setprecision(2);

    cout << "|  " <<  setw(15) << left << Client.AccountNumber;
  //  cout << "|  " <<  setw(10) << left << Client.PinCode;
    cout << "|  " <<  setw(40) << left << Client.Name ;
   // cout << "|  " <<  setw(12) << left << Client.Phone;
    cout << "|  " <<  setw(12) << left << Client.AccountBalance ;
    cout << endl;

}

void PrintBalancesList ()
{   

    
    vector<sClient> vClients = LoadClientDataFromFile(ClientsFileName);
    double Total = 0;
    cout << "\n\t\t\t\t\t\t\t Balances List (" << vClients.size() << ") Clients   \n";
    cout << "=======================================================================================================================================" << endl;
    // cout << "| Account Number    |    Client Name               |     Balance       \n"; 
    // cout << "=======================================================================================================================================" << endl;

    cout << "|  " << left << setw(15) << "Account Number";
    //cout << "|  " << left << setw(10) << "Pin Code " ;
    cout << "|  " << left << setw(40) << "Client's Name" ;
    //cout << "|  " << left << setw(12) << "Phone " ;
    cout << "|  " << left << setw(12) << "Account Balance " ;
    cout <<"\n--------------------------------------------------------------------------------------------";
    cout <<"-------------------------------------------\n" << endl;

    for (sClient Client : vClients)
   {
        
        PrintClientRecordBalanceList(Client);
        Total = Total +Client.AccountBalance; 
        cout << endl;
    }
    cout <<"\n--------------------------------------------------------------------------------------------";
    cout <<"-------------------------------------------\n" << endl;
    cout << "\n\t\t\t\t\t\t\t Total Balances = "<< Total ;

}

void PrintLongInScreen()
{

PrintColoredLogo();
cout <<"\n--------------------------------------------------\n";
cout <<"                   Login Screen                     \n";
cout <<"\n--------------------------------------------------\n";
//cout <<"Username: ";
}


sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;
    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? \n"; 
    getline(cin >> ws, Client.PinCode );

    cout << "Enter Name? \n";
    getline(cin , Client.Name);

    cout << "Enter Phone? \n";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? \n";
    cin >> Client.AccountBalance;

    return Client;
}


void DepositClientRecord (sClient &Client, double Deposit)
{
    Client.AccountBalance = Client.AccountBalance + Deposit; 

}

void WithdrawCleintRecord(sClient &Client, double Withdraw)
{
    Client.AccountBalance = Client.AccountBalance - Withdraw;

}

void PrintIntroFace ()
{
    cout << "======================================================================" << endl;
    cout << "                            Main Menu Screen                       \n" << endl  ;
    cout << "======================================================================" << endl;
    cout << "      [1] Show Client List.     \n";
    cout << "      [2] Add New Client.       \n";
    cout << "      [3] Delete Client.        \n";
    cout << "      [4] Update Client Info.   \n";
    cout << "      [5] Find Client.          \n";
    cout << "      [6] Transactions          \n"; 
    cout << "      [7] Manage Users          \n";
    cout << "      [8] Log Out               \n";
    cout << "      [9] Exit                  \n";   

    cout << "======================================================================" << endl;
    cout << "Choose What do you want to do ? [1 to 9]? \n";

}


vector <sClient> SaveClientFile(string FileName , vector<sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName , ios::out);

    string DataLine;
    if ( MyFile.is_open())
    {
        for ( sClient C : vClients )
        {
             if (!C.MarkForDelete)
             {
                 DataLine = ConvertRecordToLine(C);
                 MyFile  << DataLine << endl;
             }
        }
       // 
       //  MyFile.flush();
       //  MyFile.seekg(0);
        MyFile.close();
    }
    return vClients;
}

bool FindClientByAccountNumber (string& AccountNumber, vector <sClient>& vClients, sClient& Client)
{
    
    for (sClient C : vClients )
    {   
   //     SaveClientFile(ClientsFileName, vClients);
        if(C.AccountNumber == AccountNumber)
        {
        Client = C; 
        return true;
        }
    }
    return false; 
    cout << "\nClient With Account Number "<< AccountNumber <<" Is Not Found\n";

}


bool FindUser (string& Username, vector <sUsers>& vUsers, sUsers& Users)
{
    for (sUsers U : vUsers )
    {
        if((U.Username == Username)) 
        {
           Users = U;
            return true;
        }
         
    } 
    return false;
}

bool FindUserByUsernameAndPassword(string Username, string Password, sUsers& Users)
{
  vector <sUsers> vUsers = LoadUsersDataFromFile(UsersFileName);
    for(sUsers U : vUsers) 
    {
        if(U.Username == Username && U.Password == Password)
        { 
            Users = U ; 
            return true;
        }
    }
    return false;
   
}

bool MarkClientFileDeleteByAccountNumber (string AccountNumber , vector <sClient>& vClients)
{
    for (sClient& C: vClients )
    {
        if(C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return true;
        }
    }
    return false;
}


void ShowCleintsList ()
{
    vector<sClient> vClients = LoadClientDataFromFile(ClientsFileName);
  //  PrintAllClientData(vClients);


    cout << "\n\t\t\t\t\t\t\t Clients List ("<< vClients.size() <<  ") Client(s)";
    cout <<"\n--------------------------------------------------------------------------------------------";
    cout <<"-------------------------------------------------------------------------------------------\n" << endl;
    cout << "|  " << left << setw(15) << "Account Number";
    cout << "|  " << left << setw(10) << "Pin Code " ;
    cout << "|  " << left << setw(40) << "Client's Name" ;
    cout << "|  " << left << setw(12) << "Phone " ;
    cout << "|  " << left << setw(12) << "Account Balance " ;
    cout <<"\n--------------------------------------------------------------------------------------------";
    cout <<"-------------------------------------------------------------------------------------------\n" << endl;

    for (sClient Client : vClients)
    {
        
        PrintClientRecord(Client);
        cout << endl;
    }

}

void PrintTransactionsMenuScreen ()
{
    cout << "======================================================================" << endl;
    cout << "                         Transactions Menu Screen                       \n" << endl  ;
    cout << "======================================================================" << endl;
    cout << "      [1] Deposit.     \n";
    cout << "      [2] Withdraw.       \n";
    cout << "      [3] Total Balances.        \n";
    cout << "      [4] Main Menu.   \n";
    cout << "======================================================================" << endl;
    cout << "Choose What do you want to do ? [1 to 4]? \n";

}

void PrintDepositScreen ()
{
    cout << "======================================================================" << endl;
    cout << "                             Deposit Screen                         \n" << endl  ;
    cout << "======================================================================" << endl;


}


void PrintWithdrawScreen ()
{
    cout << "======================================================================" << endl;
    cout << "                             Withdraw Screen                         \n" << endl  ;
    cout << "======================================================================" << endl;


}

bool DeleteClientByAccountNumber  (string AccountNumber , vector<sClient>& vClients)
{

    sClient Client;
    char Answer = 'n';

   if(FindClientByAccountNumber(AccountNumber, vClients,  Client))
    {
        PrintClientRecord(Client);  
        cout << "\n\nAre you sure that you want to delete the client? (Y/N)!\n";

        cin >> Answer ;
        if (Answer == 'y' || Answer == 'Y' )
        {
             MarkClientFileDeleteByAccountNumber(AccountNumber, vClients); 
             SaveClientFile( ClientsFileName ,vClients);

        
        cout << "\nClient Deleted Successfuly.\n";
        return true;
        }
        return false;
    }

   else {
            cout << "\nClient With Account Number "<< AccountNumber <<" Is Not Found\n";
            return false;
        }
}


void ShowFoundClientbyAccountNumber (string AccountNumber , vector<sClient>& vClients)
{
    sClient Client;

    if(FindClientByAccountNumber(AccountNumber, vClients,  Client))
    {
        PrintClientRecord(Client); 
        
    }  
  
    else 
    {
            cout << "\nClient With Account Number "<< AccountNumber <<" Is Not Found\n";
    }        
        
}



bool UpdateClientByAccountNumber(string AccountNumber, vector<sClient> &vClients)
{
    sClient Client;
    char Answer = 'n';
        if (FindClientByAccountNumber(AccountNumber, vClients,Client))
    {
        PrintClientRecord(Client);
        cout << "\n\nAre you sure you want update this client? y/n? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
            {
                for (sClient& C : vClients)
                {
                    if (C.AccountNumber == AccountNumber)
                    {
                        C = ChangeClientRecord(AccountNumber);
                        break;
                    }
                }
        SaveClientFile(ClientsFileName, vClients);
        cout << "\n\nClient Updated Successfully.\n";
        return true;
            }
    }
        else
    {
        cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
        return false;
    }
  return false;
}

bool AddDepositAmount (string AccountNumber, vector<sClient> &vClients)
{


    sClient Client;
    double Deposit;
    char Answer = 'n';
    cout << "\nPlease Enter AccountNumber ?\n";
    cin >> AccountNumber;
        if (FindClientByAccountNumber(AccountNumber, vClients,Client))
    {
        PrintClientRecordDeposit(Client);
        cout << "\nPlease Enter Deposit Amount\n";
        cin >> Deposit;
       // DepositClientRecord(AccountNumber);
        cout << "\n\nAre you sure you want perform this Transaction? y/n? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
            {
                for (sClient& C : vClients)
                {
                    if (C.AccountNumber == AccountNumber)
                    {
                         DepositClientRecord(C, Deposit);
                        break;
                    }
                }
              SaveClientFile(ClientsFileName, vClients);
              cout << "\n\nClient Updated Successfully.\n";
              return true;
            }
        ClearScreen();
        PrintTransactionsMenuScreen();
    }
        else
    {
        cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
          return AddDepositAmount ( AccountNumber,  vClients);
        //return false;
    }
        return false;
 
}



bool WithdrawAmount (string AccountNumber, vector<sClient> &vClients)
{


    sClient Client;
    double Withdraw;
    char Answer = 'n';

    cout << "\nPlease Enter AccountNumber ?\n";
    cin >> AccountNumber;
        if (FindClientByAccountNumber(AccountNumber, vClients,Client))
    {
        PrintClientRecordDeposit(Client);
        
        do
        { 
            cout << "\nPlease Enter Withdraw Amount\n";
            cin >> Withdraw;
            
            if (Withdraw > Client.AccountBalance)
            {
                cout << "\nNot Enough, insufficient funds! Your current balance is: " << Client.AccountBalance << "\n";
            }

         } while (Withdraw > Client.AccountBalance);
        
               cout << "\n\nAre you sure you want  Withdraw "<<Withdraw <<" ? y/n? ";
               cin >> Answer;
               if (Answer == 'y' || Answer == 'Y')
                {
                   for (sClient& C : vClients)
                   {
                      if (C.AccountNumber == AccountNumber)
                     {
                         WithdrawCleintRecord(C, Withdraw);
                        break;
                     }
                   }
                   SaveClientFile(ClientsFileName, vClients);
                   cout << "\n\nClient Updated Successfully.\n";
                   return true;
                }
             ClearScreen();
             PrintTransactionsMenuScreen();
     }
        
         
       

        else
          {
               cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
               return WithdrawAmount ( AccountNumber,  vClients);
               //return false;
          }
    
        return false;
    } 




void GoToShowMainMenu ()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
     ClearScreen();
   // startMainMenu ();

}

enTransactionsMenue Transtart ()
{

 short Choose;
    do {
        PrintColoredLogo();
        PrintTransactionsMenuScreen();
        cin >> Choose ;

        while(cin.fail())
        {
            cin.clear();
           cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            cout << "Invalid Number, Enter a valid one" << endl;
            cin >> Choose;
         }
       }
       while(Choose < 1 || Choose > 4 );
    
   

    return (enTransactionsMenue)Choose;
}



//void PreformMenuOption( Menue Choice , vector <sClient>& vClients , string& AccountNumber,  sClient& Client); 

void PreformTransMenuOption ( enTransactionsMenue Choice , vector <sClient>& vClients , string& AccountNumber )
{
    switch (Choice)
    {
        case Deposit:
            {   ClearScreen();
                PrintColoredLogo();
                PrintDepositScreen();
                //cin >> AccountNumber;
                AddDepositAmount(AccountNumber, vClients);
                break;
                 
            }

        case Withdraw: 
            { ClearScreen();
              PrintColoredLogo();
              PrintWithdrawScreen(); 
              WithdrawAmount(AccountNumber, vClients);
                break;
            }

        case Total:
            {   ClearScreen();
                PrintColoredLogo();
                PrintBalancesList();
                cout << "\n\nPress any key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case Main:
            {   ClearScreen();
                PrintColoredLogo();
               // PreformMenuOption(Choice, vClients,AccountNumber, Client);
               //GoToShowMainMenu();
                break;
            }

    } 
}


void PreformMenuOption( Menue Choice , vector <sClient>& vClients , string& AccountNumber ) 
{
     // PrintColoredLogo();
      switch (Choice)
      {
        case Show:
            {
                  ClearScreen();
                  PrintColoredLogo();
                  ShowCleintsList(); 
                  break;
            }
       case Add:
            { 
           
                 ClearScreen();
                 PrintColoredLogo();
                 cout << "\n-----------------------------------------------------------\n";
                 AddClients(vClients);
                 //GoToShowMainMenu(); 
                 break;
            }

       case Delete:
            {
                ClearScreen();
                PrintColoredLogo();
               // string AccountNumber;
                cout << "Enter Account Number to delete: \n";
                cin >> AccountNumber;

                DeleteClientByAccountNumber(AccountNumber, vClients);
                break;
            }

    case Update:
            {
               ClearScreen();
               PrintColoredLogo();
               cout << "Enter Account Number to Update: \n";
               cin >> AccountNumber;
               UpdateClientByAccountNumber(AccountNumber, vClients);
               break;
           }

    case Find:
           {   
               ClearScreen();
               PrintColoredLogo();
               sClient Client ; 
               cout << "Enter Account Number to Find: \n";
               cin >> AccountNumber;
               ShowFoundClientbyAccountNumber(AccountNumber, vClients);
               break;
           }

    case Transactions:
           {
                    
               enTransactionsMenue TransChoice;
               do
              {
                ClearScreen();
                TransChoice = Transtart();
                PreformTransMenuOption(TransChoice, vClients, AccountNumber);
              } while (TransChoice != Main);
    
               break;
           }
        
    case Manage:
           {
            ClearScreen();
            break;
           }


    case LogOut:
           {
           ClearScreen();
           PrintColoredLogo();
            LogIn();

              break;
          }   
    case Exit: 
            
                break;
            
      }

    if (Choice != LogOut && Choice != Transactions && Choice != Exit )
    {   
        
        GoToShowMainMenu();
    }
 
}

Menue start ()
{
    short Choose ;
   
    do {
        PrintColoredLogo();
        PrintIntroFace();
        cin >> Choose ;

        while (cin.fail())
          {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            cout << "Invalid Number, Enter a valid one" << endl;
            cin >> Choose;
         }
       }
       while(Choose < 1 || Choose > 9 );
    
   

    return (Menue)Choose;
}





void startMainMenu ()
{
    
 vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
 Menue Choice ;

 
     
      do 
      {   
        sClient Client;
        string AccountNumber ; 
         ClearScreen();
         Choice =  start();
         PreformMenuOption(Choice, vClients, AccountNumber);

       } while (Choice != Exit ); 
    
     
  
   
}



bool LoadUserInfo(string Username, string Password)
{
   if (FindUserByUsernameAndPassword(Username, Password, CurrentUser))
           return true;
   else
           return false;
}



string ReadPassword() {
    string password = "";
    char ch;

#ifndef _WIN32
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO; // تم التصحيح هنا إضافة c_ قبل lflag
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    while (true) {
#ifdef _WIN32
        ch = _getch(); 
#else
        ch = cin.get(); 
#endif

        if (ch == '\n' || ch == '\r') {
            break;
        }
        
        #ifdef _WIN32
        if (ch == 8) { 
        #else
        if (ch == 127 || ch == 8) { 
        #endif
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; 
            }
        } 
        else if (ch >= 32 && ch <= 126) { 
            password += ch;
            cout << '*';
        }
    }

#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    cout << endl;
    return password;
}









void LogIn()
{

   bool LogInFaild = false;
   string Username;
   string Password;
 
    do 
    {
        ClearScreen();
        PrintLongInScreen();

        if (LogInFaild)
        {
            cout << "\nInvaild Username/Password!\n"; 
        }
        cout << "Username: ";
        cin >> Username;
        cout << "\nPassword: ";
       // cin >> Password;
       cin.ignore(numeric_limits<streamsize>::max(), '\n');
       Password = ReadPassword();
        LogInFaild = !LoadUserInfo(Username, Password);
    } while(LogInFaild);

  startMainMenu();
    
}





int main ()
{
 //startMainMenu();
LogIn();
return 0; 
}

