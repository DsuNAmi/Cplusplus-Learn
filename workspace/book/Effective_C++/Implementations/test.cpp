#include <iostream>
#include <string>
#include <memory>


using namespace std;

class Date {};
class Address {};
// class Date;
// class Address;


class Person
{
    public:
        virtual ~Person();
        virtual string name() const = 0;
        virtual string birthDate() const = 0;
        virtual string address() const = 0;
        static shared_ptr<Person> create(const string & name, const Date & birthday, const Address & addr);
};


class RealPerson : public Person
{
    public:
        RealPerson(const string &name, const Date& birthday, const Address & addr)
        : theName(name),theBirth(birthday),theAddr(addr) {}
        virtual ~RealPerson();
        string name() const;
        string birthDate() const;
        string address() const;
    private:
        string theName;
        Date theBirth;
        Address theAddr; 
};  

shared_ptr<Person> Person::create(const string & name, const Date & birthday, const Address & addr)
{
    return shared_ptr<Person>(new RealPerson(name,birthday,addr));
}



int main()
{
    string name;
    Date dateofBirth;
    Address address;


    //创建一个接口对象
    shared_ptr<Person> pp(Person::create(name,dateofBirth,address));
    cout << pp->name() << pp->birthDate() << pp->address();
    return 0;
}