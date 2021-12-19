#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

using std::cout; using std::cin; using std::ofstream; using std::endl; using std::string; using std::ifstream; using std::vector;
class UserDatabase{

public:
    UserDatabase() = default;

    //Log the user into the system
    void logIn(string username, string password)
    {
        bool status = verifyLogIn(username, password); // checks if login information is correct
        int counter = 0;                               // login counter
   
        while(!status)
        {
            counter++;
            cout << "Your login information is incorrect. Please try again...  (Hint: Username and Password are case-sensitive)" << endl;
            cout << "Type stop to exit.\n";

            cout << "Enter username: ";
            cin >> username;
            if(username == "stop") return;
         
            cout << "Enter password: ";
            cin >> password;
            
            status = verifyLogIn(username, password);

            if(counter >= 3)
            {
                cout << "Failed to login successfully." << endl;
                return;
            }
        }

        if (status)
        {
            cout << "Successfully logged in!" << endl;
            counter = 0; // reset login counter
            
            for(size_t i = 0; i < users_.size(); i++)
                if(users_[i].username_ == username)
                    logged_in_users_.push_back(users_[i]);
    
            system("PAUSE");
        }
    }

    //Checks if a user is logged in
    bool isLoggedIn(string username)
    {
        for(size_t i = 0; i < logged_in_users_.size(); i++)
            if(logged_in_users_[i].username_ == username)
                return true;
            
        return false;
    }

    //Asks the user to log in and then checks if the login information is correct
    bool verifyLogIn(string username, string password)
    {
        for(size_t i = 0; i < users_.size(); i++)
            if(users_[i].username_ == username && users_[i].password_ == password) return true; //user found
        
        return false;
    }

    //Register the user into the system
    void registerUser(string& username, string& password)
    {
        User* user = new User();
        user->username_ = username;
        user->password_ = password;
        users_.push_back(*user); 

        string filename("database.txt");
        ofstream file;
        file.open(filename, std::ios_base::app); //append data instead of overwrite
        file << "\nusername: " << username << "    " << "password: " << password; 
        file.close();

    }

    //Parse the user login information from the file into the array
    void parseUserBase(const string& filename)
    {  
        string header, filter_text;
        ifstream file;
        file.open(filename);
        
        for(int i = 0; i < 3; i++)
            getline(file, header); //skip the header of the file

        while(file)
        {
            string username, password;
            getline(file, filter_text, ':');
            file >>  username >> filter_text >> password;
            file >> std::ws;
            if(!file) break;

            //cout << "'" << username << "'" << " '" << password << "'" << endl; //debug parsing

            User* user = new User();
            user->username_ = username;
            user->password_ = password;
            users_.push_back(*user);
        }
        
        file.close();
    }

    //Checks to see if the username already exists. Usernames should be unique
    bool isUser(string username) const
    {
        for(size_t i = 0; i < users_.size(); i++)
            if(users_[i].username_ == username) return true;
        
        return false;
    }

    //Changes the user's password
    void changePassword(string& username, string password, string password2, string& new_password)
    {
        /* --------------- Security Check ---------------------------- */
        if(password != password2) {cout << "Password do not match." << endl; return;}
        bool user = isUser(username);
        if(!user) {cout << "User does not exist." << endl; return;}
        /* ----------------------------------------------------------- */
        for(size_t i = 0; i < users_.size(); i++)
        {
            if(users_[i].username_ == username && users_[i].password_ == password)
                users_[i].password_ = new_password;
        }

        string filename("database.txt");
        string line;
        ifstream read_file;
        ofstream write_file;
        
        read_file.open(filename);
        write_file.open("new.txt");
        while(getline(read_file, line))
        {
            size_t found =  line.find("username: " + username);
            if(found != std::string::npos)
                write_file << "username: " << username << "    " << "password: " << new_password << endl;
            else
                write_file << line << endl;
        }
        
        write_file.close();
        read_file.close();
        remove("database.txt");
        rename("new.txt", "database.txt");
        cout << "Password change successful." << endl;
    }

    //Clear the database
    void clearUserNetwork()
    {
        remove("database.txt");
    }

private: 
    //User class
    struct User{
        string username_, password_;
    };
    
    vector<User> users_; //array of users
    vector<User> logged_in_users_;
};

int menu()
{
    string menu_option; //user option from the menu

    cout << "\n1: Register\n2: Login\n3: Change Password\n4: Exit\n\nEnter a number to choose an option: ";
    cin >> menu_option;

    for(char c : menu_option)
        if(!isdigit(c)) return -1;
     if(stoi(menu_option) > 4 || stoi(menu_option) < 1) return -1;
    
    return stoi(menu_option);
    
}

//Check to see if the database file exists
inline bool fileExists (const string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}
    

int main()
{
    UserDatabase network;
    string filename = "database.txt";
    if(!fileExists(filename))
    {
        ofstream file(filename);
        file << "User Database Information Below: ";
        for(int i = 0; i < 2; i++)
            file << endl;
        file.close();
    }

    network.parseUserBase(filename);

    bool run = true;
    int menu_option = menu();
    while(run)
    {
        if(menu_option == -1) //Invalid option
        {
            cout << "\nNot a valid input. Please enter a correct option." << endl;
            menu_option = menu();
        }
        if(menu_option == 1) //Register
        {
            string username, password;
            bool valid = false;

            do
            {
                cout << "Select a username (Case-Sensitive): ";
                cin >> username;

                if(!network.isUser(username))
                    valid = true; //break the loop
                else 
                    cout << "This user already exists, please select another username.\n";
                
            } while (valid == false);
         
            cout << "Select a password (Case-Sensitive): ";
            cin >> password;

            network.registerUser(username, password);
            if(network.isUser(username)) cout << "Registration successful!\n";
            menu_option = menu();
        }

        else if(menu_option == 2) //Login
        {
            string username, password;

            cout << "Enter username: ";
            cin >> username;

            cout << "Enter password: ";
            cin >> password;
            
            if(!network.isLoggedIn(username))
                network.logIn(username, password);
            else
                cout << "You are already logged in." << endl;

            menu_option = menu();
        }

        else if(menu_option == 3) //Change password
        {
            string username, password, password2, new_password;
            bool valid = false;
            do
            {
                cout << "Enter username: ";
                cin >> username;

                if(network.isLoggedIn(username))
                    valid = true; //break the loop
                else if (username == "stop")
                { valid = true;}
                else 
                {
                    cout << "User is not logged in.\n";
                    cout << "Type stop to exit.\n";
                }
                    
                
            } while (valid == false);
            if( username != "stop")
            {
                cout << "Enter password: ";
                cin >> password;

                cout << "Verify password: ";
                cin >> password2;

                cout << "Enter a new password: ";
                cin >> new_password;

                network.changePassword(username, password, password2, new_password);
            }
            
            menu_option = menu();
        }
        //Exit
        else if(menu_option == 4) run = false; //all users will be logged out

    }
    return 0;
}