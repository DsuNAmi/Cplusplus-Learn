#include <iostream>

using namespace std;


// template <typename T>
// class NamedObject
// {
//     public:
//         //以下构造函数不再接受一个const名称，因为nameValue
//         //如今是一个reference-to-const string.
//         NamedObject(std::string & name, const T & value);

//         //假如未声明operator=

//     private:
//         std::string& nameValue;
//         const T objectValue;
// };


// std::string newDog("persephone");
// std::string oldDog("Satch");
// NamedObject<int> p(newDog,2);
// NamedObject<int> s(oldDog,36);

// p =  s; //这段代码能否通过编译器检测？


// class Uncopyable
// {
//     protected:
//         Uncopyable() {}                 //允许ctor dtor
//         ~Uncopyable() {}
//     private:
//         Uncopyable(const Uncopyable &);  //阻止copy 
//         Uncopyable & operator=(const Uncopyable &);
// };


// class HomeForSale : private Uncopyable
// {

// };

// class DBConnection
// {
//     public:
//         static DBConnection create();

//         void close();
// };

// class DBConn
// {
//     public:
//         void close()
//         {
//             db.close();
//             closed = true;
//         }

//         ~DBConn()
//         {
//             if(!closed)
//             {
//                 try
//                 {
//                     db.close();
//                 }
//                 catch(exception)
//                 {
//                     //记录错误
//                     //终止程序或者是吞下异常
//                 }
//             }
//         }

//     private:
//         DBConnection db;
//         bool closed;

// };


// class Transaction{
//     public:
//         explicit Transaction(const std::string & logInfo);
//         void logTransaction(const std::string & logInfo) const;            //做出一份因类型不同而不同的日志记录    
// };

// Transaction::Transaction(const std::string & logInfo)
// {
//     //一系列操作
//     // logTransaction();//最后是记录这笔交易
//     logTransaction(logInfo);
// }


// class BuyTransaction : public Transaction
// {
//     public:
//         BuyTransaction() : Transaction(createLogString()) {}
//     private:
//         static std::string createLogString();
// };

// class SellTransaction : public Transaction
// {
//     public:
//         virtual void logTransaction() const;
// };

// class Widget
// {
//     public:
//         Widget & operator=(const Widget & rhs)
//         {
//             return *this;
//         }
// };

// //该协议不仅适用于以上的标准形式，也适用于所有的运算
// class Widget
// {
//     Widget & operator+=(const Widget & rhs)
//     {
//         return *this;
//     }

//     Widget & operator=(int rhs)
//     {
//         return *this;
//     }
// };

// void logCall(const std::string & funcName); //制造一个LOG ENTRY

// class Customer
// {
//     public:
//         Customer(const Customer & rhs);
//         Customer & operator=(const Customer & rhs);
//     private:
//         std::string name;
// };


// Customer::Customer(const Customer & rhs)
// :name(rhs.name)
// {
//     logCall("Customer copy constructor");
// }

// Customer & Customer::operator=(const Customer & rhs)
// {
//     logCall("Customer copy assignment operator");
//     name = rhs.name;
//     return * this;
// }

// class PriortyCustomer : public Customer
// {
//     public:
//         PriortyCustomer(const PriortyCustomer & rhs);
//         PriortyCustomer& operator=(const PriortyCustomer & rhs);
//     private:
//         int priority;
// };

// PriortyCustomer::PriortyCustomer(const PriortyCustomer & rhs)
// :Customer(rhs),priority(rhs.priority)
// {
//     logCall("PriorityCustomer copy constructor");
// }

// PriortyCustomer & PriortyCustomer::operator=(const PriortyCustomer & rhs)
// {
//     logCall("PriorityCustomer copy assignment operator");
//     Customer::operator=(rhs);
//     priority = rhs.priority;
//     return *this;
// }



int main()
{
    // int a = 1;
    // int b = 2;
    // int & ref = a;
    // std::cout << ref << std::endl;
    // ref = b;
    // std::cout << a << std::endl;
    // BuyTransaction b;

    return 0;
}