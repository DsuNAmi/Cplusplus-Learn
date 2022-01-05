/* 

a simple calculator

Author:Hujingrui
Time:2022/01/05~
Contact:DsuNAmi@163.com

*/

#include <iostream>
#include <vector>
#include <map>
#include <stack>


#define ON true

std::initializer_list<char> BUTTONS = {'1','2','3','4','5','6','7','8','9','0','+','-','x','/','='};
// #define BUTTONS {'1','2','3','4','5','6','7','8','9','0','+','-','x','/'}

const std::string ZEROERROR = "Division by zero !!!";


/**
 * @brief 
 * 
 */
class Calculate
{
    public:
        template<class T>
        static T& plus(T& a, T& b);

        template<class T>
        static T& div(T& a, T& b);

        template<class T>
        static T& minus(T& a, T& b);

        template<class T>
        static T& mul(T& a, T&  b);
};

template<class T>
T& Calculate::plus(T& a, T& b)
{
    return a + b;
}

template<class T>
T& Calculate::minus(T& a, T& b)
{
    return a - b;
}


template<class T>
T& Calculate::div(T& a, T& b)
{
    if(!b)
    {
        throw ZEROERROR;
    }
    
    return a / b;
}

template <class T>
T& Calculate::mul(T& a, T& b)
{
    try
    {
        return a * b;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

//end Calculate

/**
 * @brief 
 * 
 */
class UI
{
    public:
        UI(std::initializer_list<char>& buttons);
        UI(const char * buttons, int size);
        ~UI() {}
        
        void PrintButtons();
        void OnIT();
        void Output(int select);
        void FlushScreen();
        int Input();

    private:
        std::vector<char> _UIButtons;
        std::stack<int> _integerStack;
        std::stack<char> _charStack;
        std::string displayString;

        


};

UI::UI(std::initializer_list<char>& buttons)
{
    for(auto button : buttons)
    {
        this->_UIButtons.push_back(button);
    }
}

UI::UI(const char * buttons, int size)
{
    for(int i = 0; i < size; ++i)
    {
        this->_UIButtons.push_back(buttons[i]);
    }
}


void UI::PrintButtons()
{
    int count = 1;
    for(auto button : this->_UIButtons)
    {
        std::cout << count++ << "::::" << button << std::endl;  
    }
}

void UI::FlushScreen()
{
    system("cls");
    this->PrintButtons();
    std::cout << this->displayString << std::endl;

}

int UI::Input()
{
    try
    {
        int select;
        std::cin >> select;
        // std::cout << std::endl;
        return select;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void UI::Output(int select)
{
    char selected = this->_UIButtons[select - 1];
    if('=' == selected)
    {  
        
    }
    else
    {
        this->displayString += selected;
        if('0' <= selected && selected <= '9')
        {
            this->_integerStack.push(selected - '0');
        }
        else
        {
            this->_charStack.push(selected);
        }
    }

}



void UI::OnIT()
{
    this->PrintButtons();
    while(ON)
    {
        /* wait for the inputing */
        this->Output(this->Input());
    }
}

//end UI


int main()
{
    UI userInterface = UI(BUTTONS);
    // userInterface.PrintButtons();
    userInterface.OnIT();

    return 0;
}