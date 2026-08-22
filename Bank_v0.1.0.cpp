#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include "Log.h"
#include "picosha2.h"
#include <random>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

int ReadPermissionsToSet();
const string ClientsFileName = "Clients.txt";
const string UsersFileName   = "user.txt";

enum Menue { Show = 1, Add = 2, Delete = 3, Update = 4, Find = 5, Transactions = 6, Manage = 7, LogOut = 8, Exit = 9 };
enum enTransactionsMenue { Deposit = 1, Withdraw = 2, Total = 3, Main = 4 };
enum enManiMenuManageMainOptions { eListUsers = 1, eAddUser = 2, eDeleteUser = 3, eUpdateUser = 4, eFindUser = 5, eMainMenu = 6 };
enum enMainMenuePermissions { eAll = -1, pListClients = 1, pAddNewClient = 2, pDeleteClient = 4, pUpdateClient = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

void startMainMenu();
void LogIn();

struct sClient
{
   string AccountNumber;
   string PinCode; 
   string Name;
   string Phone;
   double AccountBalance;
   bool MarkForDelete = false;
};

struct sUsers
{
  string Username;
  string Password;
  string Salt;
  int Permissions;
  bool MarkForDelete = false;
};

sUsers CurrentUser;

string TrimLeft(string S1)
{
    for (size_t i = 0; i < S1.length(); i++)
    {
        if (S1[i] != ' ' && S1[i] != '\t' && S1[i] != '\r' && S1[i] != '\n')
        {
            return S1.substr(i, S1.length() - i);
        }
    }
    return "";
}

string TrimRight(string S1)
{
    for (int i = S1.length() - 1; i >= 0; i--)
    {
        if (S1[i] != ' ' && S1[i] != '\t' && S1[i] != '\r' && S1[i] != '\n')
        {
            return S1.substr(0, i + 1);
        }
    }
    return "";
}

string Trim(string S1)
{
    return TrimLeft(TrimRight(S1));
}

vector<string> SplitString(string S1, string Delim)
{
    vector<string> vString;
    std::string::size_type pos = 0;
    string sWord; 

    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos);

        if (sWord != "")
        {
            vString.push_back(sWord);
        }
        S1.erase(0, pos + Delim.length());
    }

    if (S1 != "")
    {    
        vString.push_back(S1);
    }
    return vString;
}


string GenerateSalt(int Length = 16)
{
    const string Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    random_device rd; 
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, (int)Chars.size() - 1);

    string Salt;
    for (int i = 0; i < Length; i++)
    {
        Salt += Chars[dist(gen)];
    }
    return Salt;
}

string HashPassword(const string& Password, const string& Salt)
{
    string Salted = Salt + Password;
    return picosha2::hash256_hex_string(Salted);
}

bool VerifyPassword(const string& EnteredPassword, const string& Salt, const string& StoredHash)
{

    return HashPassword(EnteredPassword, Salt) == StoredHash;

}

bool LooksLikeSh256Hash(const string& S)
{
    if (S.length() != 64) return false;
    for (char c: S)
    {
        if (!isxdigit((unsigned char)c)) return false;
    }
    return true;
}

double ReadPositiveNumber(string Message)
{
    double Number = 0;
    do
    {
        cout << Message;
        cin >> Number;

        while (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Number, Enter a valid one: ";
            cin >> Number;
        }

    } while (Number <= 0);

    return Number;
}

sClient ReadNewClient()
{
    sClient Client;
    
    cout << "Enter Account Number: ";
    getline(cin >> ws, Client.AccountNumber);
    cout << "Enter PinCode: ";
    getline(cin, Client.PinCode);
    cout << "Enter Name: ";
    getline(cin, Client.Name);
    cout << "Enter Phone: ";
    getline(cin, Client.Phone);
    
    Client.AccountBalance = ReadPositiveNumber("Enter Account Balance: ");

    return Client;
}

sClient ConverLineToRecord(string line, string Seperator = "#//#")
{
    sClient Client;
    vector<string> vClientData = SplitString(line, Seperator);

    if (vClientData.size() >= 5) 
    {
       Client.AccountNumber = Trim(vClientData[0]);
       Client.PinCode       = Trim(vClientData[1]);
       Client.Name          = Trim(vClientData[2]);
       Client.Phone         = Trim(vClientData[3]);
       
       if (!vClientData[4].empty()) {
            Client.AccountBalance = stod(Trim(vClientData[4]));
       } else {
            Client.AccountBalance = 0;
       }
    }
    return Client;
}

sUsers ConverLineToRecordUser(string line, string Seperator = "#//#")
{
    sUsers User;
    vector<string> vUserDate = SplitString(line, Seperator);

    if (vUserDate.size() >= 4)
    {
        User.Username    = Trim(vUserDate[0]);
        User.Password    = Trim(vUserDate[1]);
        User.Salt        = Trim(vUserDate[2]); 
        User.Permissions = stoi(Trim(vUserDate[3]));

    }
    else if (vUserDate.size() == 3)
    {   
        User.Username    = Trim(vUserDate[0]);
        User.Password    = Trim(vUserDate[1]);
        User.Salt        = ""; 
        User.Permissions = stoi(Trim(vUserDate[2])); 

    }
    return User;
}

string ConvertUserRecordToLine(sUsers User, string Seperator = "#//#")
{
    string stUserRecord = "";
    stUserRecord += User.Username + Seperator;
    stUserRecord += User.Password + Seperator;
    stUserRecord += User.Salt + Seperator;
    stUserRecord += to_string(User.Permissions);
    return stUserRecord;
}

vector<sClient> LoadClientDataFromFile(string FileName)
{
    vector<sClient> vClients;
    fstream MyFile;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string line;
        sClient Client;

        while (getline(MyFile, line))
        {
            if (line != "")
            {
                Client = ConverLineToRecord(line);
                vClients.push_back(Client);
            }
        }
        MyFile.close();
    }
    return vClients; 
}

vector<sUsers> SaveUsersDataToFile(string FileName, vector<sUsers> vUsers);

vector<sUsers> LoadUsersDataFromFile(string FileName)
{
    vector<sUsers> vUsers;
    fstream MyFile;

    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string line;
        sUsers Users;

        while (getline(MyFile, line))
        {
            if (line != "")
            {
                Users = ConverLineToRecordUser(line);
                vUsers.push_back(Users);
            }
        }
        MyFile.close();
    }

     bool NeedsMigration = false;
    for (sUsers& U : vUsers)
    {
        if (U.Salt.empty())
        {
            string PlainPassword = U.Password;  
            U.Salt     = GenerateSalt();
            U.Password = HashPassword(PlainPassword, U.Salt);
            NeedsMigration = true;
        }
    }
    if (NeedsMigration)
    {
        SaveUsersDataToFile(FileName, vUsers);
    }
    return vUsers; 
}

string ConvertRecordToLine(sClient Client, string Seperator = "#//#")
{
    string stClientRecord = "";

    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance) + Seperator;

    return stClientRecord;
}

void AddDataLineToFile(string FileName, string stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    { 
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}

void AddNewClient(vector<sClient> &vClients)
{
    sClient Client = ReadNewClient();
    AddDataLineToFile(ClientsFileName, ConvertRecordToLine(Client));
    vClients = LoadClientDataFromFile(ClientsFileName);
}

void ClearScreen()
{
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}

void AddClients(vector<sClient> &vClients)
{
    if (CurrentUser.Permissions != enMainMenuePermissions::pAddNewClient && CurrentUser.Permissions != enMainMenuePermissions::eAll)
    {
        cout << "Access Denied !! \nUser Does NOT have Permission\n";
    }
    else
    {    
      char AddMore = 'Y';
      do 
      {
        ClearScreen();
        cout << "Adding New Client:\n\n";
        AddNewClient(vClients);
        cout << "\nClient Added Successfully, Do You Want to add More clients ? Y/N? \n";
        cin >> AddMore;
       } while (toupper(AddMore) == 'Y');
    }
}

void PrintClientRecord(sClient Client)
{
    cout << fixed << setprecision(2);
    cout << "|  " << setw(15) << left << Client.AccountNumber;
    cout << "|  " << setw(10) << left << Client.PinCode;
    cout << "|  " << setw(40) << left << Client.Name;
    cout << "|  " << setw(12) << left << Client.Phone;
    cout << "|  " << setw(12) << left << Client.AccountBalance;
    cout << endl;
}

void PrintUserCard(sUsers User)
{
    cout << "\nThe following are the user details:\n";
    cout << "-----------------------------------";
    cout << "\nUsername : " << User.Username;
    cout << "\nPassword : " << User.Password;
    cout << "\nPermissions : " << User.Permissions;
    cout << "\n-----------------------------------\n";
}

void PrintClientRecordDeposit(sClient Client)
{
    cout << fixed << setprecision(2);
    cout << "\nThe Following are the Client details:\n";
    cout << "------------------------------------------\n";
    cout << "AccountNumber   : " << Client.AccountNumber;
    cout << "\nPinCode         : " << Client.PinCode;
    cout << "\nName Client     : " << Client.Name;
    cout << "\nPhone           : " << Client.Phone;
    cout << "\nAccount Balance : " << setw(12) << left << Client.AccountBalance;
    cout << endl;
    cout << "------------------------------------------\n";
}

void PrintClientRecordBalanceList(sClient Client)
{
    cout << fixed << setprecision(2);
    cout << "|  " << setw(15) << left << Client.AccountNumber;
    cout << "|  " << setw(40) << left << Client.Name;
    cout << "|  " << setw(12) << left << Client.AccountBalance;
    cout << endl;
}

void PrintBalancesList()
{   
    vector<sClient> vClients = LoadClientDataFromFile(ClientsFileName);
    double Total = 0;
    cout << "\n\t\t\t\t\t\t\t Balances List (" << vClients.size() << ") Clients   \n";
    cout << "=======================================================================================================================================" << endl;
    cout << "|  " << left << setw(15) << "Account Number";
    cout << "|  " << left << setw(40) << "Client's Name";
    cout << "|  " << left << setw(12) << "Account Balance";
    cout << "\n--------------------------------------------------------------------------------------------";
    cout << "-------------------------------------------\n" << endl;

    for (sClient Client : vClients)
    {
        PrintClientRecordBalanceList(Client);
        Total += Client.AccountBalance; 
    }
    cout << "\n--------------------------------------------------------------------------------------------";
    cout << "-------------------------------------------\n" << endl;
    cout << "\n\t\t\t\t\t\t\t Total Balances = " << Total;
}

void PrintLongInScreen()
{
    PrintColoredLogo();
    cout << "\n--------------------------------------------------\n";
    cout << "                   Login Screen                     \n";
    cout << "\n--------------------------------------------------\n";
}

sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;
    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? \n"; 
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? \n";
    getline(cin, Client.Name);

    cout << "Enter Phone? \n";
    getline(cin, Client.Phone);

    Client.AccountBalance = ReadPositiveNumber("Enter AccountBalance? \n");

    return Client;
}

sUsers ChangeUserRecord(string Username)
{
    sUsers User;
    User.Username = Username;
    cout << "\n\nEnter Password? ";
    getline(cin >> ws, User.Password);
    User.Permissions = ReadPermissionsToSet();
    return User;
}

void DepositClientRecord(sClient &Client, double Deposit)
{
    Client.AccountBalance += Deposit; 
}

void WithdrawCleintRecord(sClient &Client, double Withdraw)
{
    Client.AccountBalance -= Withdraw;
}

void PrintIntroFace()
{
    cout << "======================================================================" << endl;
    cout << "                            Main Menu Screen                       \n" << endl;
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

void PrintUserRecordLine(sUsers User)
{
   cout << "| " << setw(15) << left << User.Username;
   cout << "| " << setw(10) << left << User.Password;
   cout << "| " << setw(104) << left << User.Permissions;
}

void ShowAllUsersScreen()
{
    vector<sUsers> vUsers = LoadUsersDataFromFile(UsersFileName);
    cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ")  User(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "| " << left << setw(15) << "User Name";
    cout << "| " << left << setw(10) << "Password";
    cout << "| " << left << setw(104) << "Permissions";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    
    if (vUsers.size() == 0)
        cout << "\t\t\t\tNo Users Available In the System!";
    else
        for (sUsers User : vUsers)
        {
            PrintUserRecordLine(User);
            cout << endl;
        }
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}

void ShowAccessDeniedMessage()
{
    cout << "\n------------------------------------\n";
    cout << "Access Denied, \nYou dont Have Permission To Do this,\nPlease Contact Your Admin.";
    cout << "\n------------------------------------\n";
}

vector<sClient> SaveClientFile(string FileName, vector<sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);

    string DataLine;
    if (MyFile.is_open())
    {
        for (sClient C : vClients)
        {
             if (!C.MarkForDelete)
             {
                 DataLine = ConvertRecordToLine(C);
                 MyFile << DataLine << endl;
             }
        }
        MyFile.close();
    }
    return vClients;
}

vector<sUsers> SaveUsersDataToFile(string FileName, vector<sUsers> vUsers)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);
    string DataLine;
    if (MyFile.is_open())
    {
        for (sUsers U : vUsers)
        {
              if (!U.MarkForDelete)
              {
                 DataLine = ConvertUserRecordToLine(U);
                 MyFile << DataLine << endl;
              }
        }
        MyFile.close();
    }
    return vUsers;
}

bool FindExitsByUsername(string Username, string FileName)
{
    fstream MyFile;
    MyFile.open(FileName, ios::in);
    if (MyFile.is_open())
    {
        string Line;
        sUsers Users;
        while (getline(MyFile, Line))
        {
            Users = ConverLineToRecordUser(Line);
            if (Users.Username == Username)
            {
                MyFile.close();
                return true;
            }
        }
        MyFile.close();
    }
    return false;
}

int ReadPermissionsToSet()
{
  int Permissions = 0;
  char Answer = 'n';
  cout << "\nDo you want to give full access? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y')
  {
     return -1;
  }
  
  cout << "\nDo you want to give access to : \n";
  cout << "\nShow Client List? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pListClients;

  cout << "\nAdd New Client? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pAddNewClient;

  cout << "\nDelete Client? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pDeleteClient;

  cout << "\nUpdate Client? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pUpdateClient;

  cout << "\nFind Client? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pFindClient;

  cout << "\nTransactions? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pTransactions;

  cout << "\nManage Users? y/n? ";
  cin >> Answer;
  if (toupper(Answer) == 'Y') Permissions += enMainMenuePermissions::pManageUsers;

  return Permissions;
}

bool CheckAccessPermission(enMainMenuePermissions Permission)
{
    if (CurrentUser.Permissions == enMainMenuePermissions::eAll)
         return true;
    if ((Permission & CurrentUser.Permissions) == Permission)
         return true;
    else
        return false;
}

sUsers ReadNewUser()
{
    sUsers User;
    cout << "Enter Username? ";
    getline(cin >> ws, User.Username);
    
    while (FindExitsByUsername(User.Username, UsersFileName))
    {
        cout << "\nUser with [" << User.Username << "] already exists, Enter another Username? ";
        getline(cin >> ws, User.Username);
    }
    
    cout << "Enter Password? ";
    getline(cin, User.Password);
    User.Permissions = ReadPermissionsToSet();
    return User;
}

void AddNewUser(vector<sUsers> &vUsers)
{
    sUsers User = ReadNewUser();
    User.Salt = GenerateSalt();
    User.Password = HashPassword(User.Password, User.Salt);
    AddDataLineToFile(UsersFileName, ConvertUserRecordToLine(User));
    vUsers = LoadUsersDataFromFile(UsersFileName);
}

void AddUsers(vector<sUsers> &vUsers)
{
    char AddMore = 'Y';
    do 
    {
        ClearScreen();
        cout << "Adding New User:\n\n";
        AddNewUser(vUsers);
        cout << "\nUser Added Successfully, Do You Want to add More Users ? Y/N? \n";
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');
}

bool FindClientByAccountNumber(string& AccountNumber, vector<sClient>& vClients, sClient& Client)
{
    for (sClient C : vClients)
    {   
        if (C.AccountNumber == AccountNumber)
        {
            Client = C; 
            return true;
        }
    }
    return false; 
}

bool FindUser(string& Username, vector<sUsers>& vUsers, sUsers& Users)
{
    for (sUsers U : vUsers)
    {
        if (U.Username == Username) 
        {
           Users = U;
           return true;
        }
    } 
    return false;
}

bool FindUserByUsernameAndPassword(string Username, string Password, sUsers& Users)
{
    vector<sUsers> vUsers = LoadUsersDataFromFile(UsersFileName);
    for (sUsers U : vUsers) 
    {
        if (U.Username == Username && VerifyPassword(Password, U.Salt, U.Password)) 
        { 
            Users = U; 
            return true;
        }
    }
    return false;
}

void ShowFoundUserbyUserName(string Username, vector<sUsers>& vUsers)
{
    sUsers User;

    if (FindUser(Username, vUsers, User))
    {
        PrintUserCard(User); 
    }  
    else 
    {
        cout << "\nUser With Username: " << Username << " Is Not Found\n";
    }        
}

bool MarkClientFileDeleteByAccountNumber(string AccountNumber, vector<sClient>& vClients)
{
    for (sClient& C: vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return true;
        }
    }
    return false;
}

bool MarkUserForDeleteByUsername(string Username, vector<sUsers>& vUsers)
{
    for (sUsers& U : vUsers)
    {
        if (U.Username == Username)
        {
            U.MarkForDelete = true;
            return true;
        }
    }
    return false;
}

void ShowCleintsList()
{
    if (!CheckAccessPermission(enMainMenuePermissions::pListClients))
    {
        ShowAccessDeniedMessage();
        return;
    }

    vector<sClient> vClients = LoadClientDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\t\t\t Clients List (" << vClients.size() << ") Client(s)";
    cout << "\n--------------------------------------------------------------------------------------------";
    cout << "-------------------------------------------------------------------------------------------\n" << endl;
    cout << "|  " << left << setw(15) << "Account Number";
    cout << "|  " << left << setw(10) << "Pin Code ";
    cout << "|  " << left << setw(40) << "Client's Name";
    cout << "|  " << left << setw(12) << "Phone ";
    cout << "|  " << left << setw(12) << "Account Balance ";
    cout << "\n--------------------------------------------------------------------------------------------";
    cout << "-------------------------------------------------------------------------------------------\n" << endl;

    for (sClient Client : vClients)
    {
        PrintClientRecord(Client);
        cout << endl;
    }
}

void PrintTransactionsMenuScreen()
{
    cout << "======================================================================" << endl;
    cout << "                         Transactions Menu Screen                       \n" << endl;
    cout << "======================================================================" << endl;
    cout << "      [1] Deposit.     \n";
    cout << "      [2] Withdraw.       \n";
    cout << "      [3] Total Balances.        \n";
    cout << "      [4] Main Menu.   \n";
    cout << "======================================================================" << endl;
    cout << "Choose What do you want to do ? [1 to 4]? \n";
}

void PrintMangeUserScreen()
{
    cout << "======================================================================" << endl;
    cout << "                          Manage Users Menu Screen                       \n" << endl;
    cout << "======================================================================" << endl;
    cout << "      [1] Show Users List.     \n";
    cout << "      [2] Add User.       \n";
    cout << "      [3] Delete User.        \n";
    cout << "      [4] Update User Permissions. \n";       
    cout << "      [5] Find User.           \n";
    cout << "      [6] Main Menu.   \n";
    cout << "======================================================================" << endl;
    cout << "Choose What do you want to do ? [1 to 6]? \n";
}

void PrintDepositScreen()
{
    cout << "======================================================================" << endl;
    cout << "                             Deposit Screen                         \n" << endl;
    cout << "======================================================================" << endl;
}

void PrintWithdrawScreen()
{
    cout << "======================================================================" << endl;
    cout << "                             Withdraw Screen                         \n" << endl;
    cout << "======================================================================" << endl;
}

bool DeleteClientByAccountNumber(string AccountNumber, vector<sClient>& vClients)
{
    if (!CheckAccessPermission(enMainMenuePermissions::pDeleteClient))
    {
       ShowAccessDeniedMessage(); 
       return false;
    }

    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientRecord(Client);  
        cout << "\n\nAre you sure that you want to delete the client? (Y/N)!\n";

        cin >> Answer;
        if (toupper(Answer) == 'Y')
        {
            MarkClientFileDeleteByAccountNumber(AccountNumber, vClients); 
            SaveClientFile(ClientsFileName, vClients);
            vClients = LoadClientDataFromFile(ClientsFileName);
        
            cout << "\nClient Deleted Successfully.\n";
            return true;
        }
        return false;
    }
    else 
    {
        cout << "\nClient With Account Number " << AccountNumber << " Is Not Found\n";
        return false;
    }
}

void ShowFoundClientbyAccountNumber(string AccountNumber, vector<sClient>& vClients)
{
    if (!CheckAccessPermission(enMainMenuePermissions::pFindClient))
    {
        ShowAccessDeniedMessage();
        return;
    }

    sClient Client;
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientRecord(Client); 
    }  
    else 
    {
        cout << "\nClient With Account Number " << AccountNumber << " Is Not Found\n";
    } 
}

bool UpdateClientByAccountNumber(string AccountNumber, vector<sClient> &vClients)
{
    if (!CheckAccessPermission(enMainMenuePermissions::pUpdateClient))
    {
        ShowAccessDeniedMessage();
        return false;
    }

    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientRecord(Client);
        cout << "\n\nAre you sure you want update this client? y/n? ";
        cin >> Answer;

        if (toupper(Answer) == 'Y')
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
            vClients = LoadClientDataFromFile(ClientsFileName);
            cout << "\n\nClient Updated Successfully.\n";
            return true;
        }
    }
    else
    {
        cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
    }
    return false;
}

bool AddDepositAmount(string AccountNumber, vector<sClient> &vClients)
{
    sClient Client;
    char Answer = 'n';

    cout << "\nPlease Enter AccountNumber: ";
    cin >> AccountNumber;

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientRecordDeposit(Client);
        
        double Deposit = ReadPositiveNumber("\nPlease Enter Deposit Amount: ");

        cout << "\n\nAre you sure you want perform this Transaction? y/n? ";
        cin >> Answer;

        if (toupper(Answer) == 'Y')
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
            vClients = LoadClientDataFromFile(ClientsFileName);
            cout << "\n\nClient Updated Successfully.\n";
            return true;
        }
        ClearScreen();
        PrintTransactionsMenuScreen();
    }
    else
    {
        cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
    }
    return false;
}

bool WithdrawAmount(string AccountNumber, vector<sClient> &vClients)
{
    sClient Client;
    char Answer = 'n';

    cout << "\nPlease Enter AccountNumber: ";
    cin >> AccountNumber;

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientRecordDeposit(Client);
        
        double Withdraw = 0;
        do
        { 
            Withdraw = ReadPositiveNumber("\nPlease Enter Withdraw Amount: ");
            
            if (Withdraw > Client.AccountBalance)
            {
                cout << "\nNot Enough, insufficient funds! Your current balance is: " << Client.AccountBalance << "\n";
            }

        } while (Withdraw > Client.AccountBalance);
        
        cout << "\n\nAre you sure you want Withdraw " << Withdraw << " ? y/n? ";
        cin >> Answer;

        if (toupper(Answer) == 'Y')
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
            vClients = LoadClientDataFromFile(ClientsFileName);
            cout << "\n\nClient Updated Successfully.\n";
            return true;
        }
        ClearScreen();
        PrintTransactionsMenuScreen();
    }
    else
    {
        cout << "\nClient with Account Number ( " << AccountNumber << " ) is Not Found!\n";
    }
    return false;
}

bool DeleteUserByUsername(string Username, vector<sUsers>& vUsers)
{
    if (Username == "Admin") 
    {
        cout << "\n\nYou cannot delete the Admin User!!\n";
        return false;
    }

    sUsers User;
    char Answer = 'n';

    if (FindUser(Username, vUsers, User))
    {
        PrintUserCard(User);
        cout << "\n\nAre you sure you want to delete this user? (Y/N)? ";
        cin >> Answer;

        if (toupper(Answer) == 'Y')
        {
            MarkUserForDeleteByUsername(Username, vUsers);
            SaveUsersDataToFile(UsersFileName, vUsers);
            vUsers = LoadUsersDataFromFile(UsersFileName);

            cout << "\nUser Deleted Successfully.\n";
            return true;
        }
    }
    else
    {
        cout << "\nUser with Username [" << Username << "] is NOT found!\n";
    }
    return false;
}

bool UpdateUserByUsername(string Username, vector<sUsers>& vUsers)
{
    sUsers User;
    char Answer = 'n';

    if (FindUser(Username, vUsers, User))
    {
        PrintUserCard(User);
        cout << "\n\nAre you sure you want to update permissions for this user? (Y/N)? ";
        cin >> Answer;

        if (toupper(Answer) == 'Y')
        {
            for (sUsers& U : vUsers)
            {
                if (U.Username == Username)
                {
                    U = ChangeUserRecord(Username);
                    break;
                }
            }

            SaveUsersDataToFile(UsersFileName, vUsers);
            vUsers = LoadUsersDataFromFile(UsersFileName);
            cout << "\nUser Permissions Updated Successfully.\n";
            return true;
        }
    }
    else
    {
        cout << "\nUser with Username [" << Username << "] is NOT found!\n";
    }
    return false;
}

void GoToShowMainMenu()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    ClearScreen();
}

enTransactionsMenue Transtart()
{
    short Choose = 0;

    if (!CheckAccessPermission(enMainMenuePermissions::pTransactions))
    {
        ShowAccessDeniedMessage();
        GoToShowMainMenu(); 
        return enTransactionsMenue::Main; 
    }

    do {
        PrintColoredLogo();
        PrintTransactionsMenuScreen();
        cin >> Choose;

        while (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid Number, Enter a valid one" << endl;
            cin >> Choose;
        }
    } while (Choose < 1 || Choose > 4);
    
    return (enTransactionsMenue)Choose;     
}

void PreformTransMenuOption(enTransactionsMenue Choice, vector<sClient>& vClients, string& AccountNumber)
{
    switch (Choice)
    {
        case Deposit:
            {   
                ClearScreen();
                PrintColoredLogo();
                PrintDepositScreen();
                AddDepositAmount(AccountNumber, vClients);
                break;
            }

        case Withdraw: 
            { 
                ClearScreen();
                PrintColoredLogo();
                PrintWithdrawScreen(); 
                WithdrawAmount(AccountNumber, vClients);
                break;
            }

        case Total:
            {   
                ClearScreen();
                PrintColoredLogo();
                PrintBalancesList();
                cout << "\n\nPress any key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case Main:
            {   
                ClearScreen();
                PrintColoredLogo();
                break;
            }
    } 
}

enManiMenuManageMainOptions nManage()
{
    short Choose = 0;

    if (!CheckAccessPermission(enMainMenuePermissions::pManageUsers))
    {
        ShowAccessDeniedMessage();
        GoToShowMainMenu(); 
        return enManiMenuManageMainOptions::eMainMenu;  
    }

    do {
        PrintColoredLogo();
        PrintMangeUserScreen();
        cin >> Choose;

        while (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid Number, Enter a valid one" << endl;
            cin >> Choose;
        }
    } while (Choose < 1 || Choose > 6);

    return (enManiMenuManageMainOptions)Choose;
}

void PreformMenuManager(enManiMenuManageMainOptions Choice, vector<sUsers>& vUsers, string& Username)
{
    switch (Choice)
    {
        case eListUsers:
            {
                ClearScreen();
                PrintColoredLogo();
                ShowAllUsersScreen();
                cout << "\n\nPress any key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case eAddUser:
            {
                ClearScreen();
                PrintColoredLogo();
                AddUsers(vUsers);
                break;
            }
        case eDeleteUser:
            {
                ClearScreen();
                PrintColoredLogo();
                cout << "\nEnter Username to Delete: ";
                cin >> Username;
                DeleteUserByUsername(Username, vUsers);
                cout << "\n\nPress any Key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case eUpdateUser:
            {
                ClearScreen();
                PrintColoredLogo();
                cout << "\nEnter Username to Update: ";
                cin >> Username;
                UpdateUserByUsername(Username, vUsers);
                cout << "\n\nPress any Key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case eFindUser:
            {
                ClearScreen();
                PrintColoredLogo();
                cout << "\nEnter the Username: ";
                cin >> Username;
                ShowFoundUserbyUserName(Username, vUsers);
                cout << "\n\nPress any key to continue...";
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
                break;
            }
        case eMainMenu:
            {
                ClearScreen();
                PrintColoredLogo();
                break;
            }
    }
}

void PreformMenuOption(Menue Choice, vector<sClient>& vClients, vector<sUsers>& vUsers, string& AccountNumber, string& Username) 
{
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
                break;
            }
        case Delete:
            {
                ClearScreen();
                PrintColoredLogo();
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
                enManiMenuManageMainOptions ManageChoice;
                do
                {
                    ClearScreen();
                    ManageChoice = nManage();
                    PreformMenuManager(ManageChoice, vUsers, Username);
                } while (ManageChoice != eMainMenu);
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
            ClearScreen();
            cout << "\nThank you for using our system!\n";
             exit(0);
            break;
    }

    if (Choice != LogOut && Choice != Transactions && Choice != Exit && Choice != Manage)
    {   
        GoToShowMainMenu();
    }
}

Menue start()
{
    short Choose;
    do {
        PrintColoredLogo();
        PrintIntroFace();
        cin >> Choose;

        while (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid Number, Enter a valid one" << endl;
            cin >> Choose;
        }
    } while (Choose < 1 || Choose > 9);

    return (Menue)Choose;
}

void startMainMenu()
{
    vector<sClient> vClients = LoadClientDataFromFile(ClientsFileName);
    vector<sUsers> vUsers = LoadUsersDataFromFile(UsersFileName);
    Menue Choice;

    do 
    {   
        string AccountNumber = "";
        string Username = "";
        ClearScreen();
        Choice = start();
        PreformMenuOption(Choice, vClients, vUsers, AccountNumber, Username);

    } while (Choice != Exit); 
}

string ReadPassword() 
{
    string password = "";
    char ch;

#ifndef _WIN32
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON); 
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
            cout << "\nInvalid Username/Password!\n"; 
        }

        cout << "Username: ";
        getline(cin >> ws, Username);

        cout << "Password: ";
        Password = ReadPassword();


        Username = Trim(Username);
        Password = Trim(Password);

        LogInFaild = !FindUserByUsernameAndPassword(Username, Password, CurrentUser);

    } while (LogInFaild);

    startMainMenu();
}

int main()
{
    LogIn();
    return 0; 
}
