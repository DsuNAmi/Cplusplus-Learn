#include <iostream>
#include <memory>

using namespace std;


shared_ptr<int> myfunc(shared_ptr<int> & ptmp)
{
    return ptmp;
}


void myDeleter(int * p)
{
    delete p;       //如果不写这句话，还是用自定义删除器，那么就会出现内存泄漏
}

// class CT
// {
//     public:
//         shared_ptr<CT> getself()
//         {
//             return shared_ptr<CT>(this);    //这行代码很危险，类似返回get的指针
//         }
// };


class CT : public enable_shared_from_this<CT>   //这是一个类模板
{
    public:
        shared_ptr<CT> getself()
        {
            return shared_from_this(); //这个会返回shared_ptr智能指针
        }
};


auto myfunc()
{
    return unique_ptr<string>(new string("I Love China!")); //临时对象都是右值
}

unique_ptr<string> tuniqp()
{
    unique_ptr<string> pr(new string("I Love China!"));
    return pr;
    // return unique_ptr<string>(new string("I Love China!"));
}

void UniqueDeleter(string * del)
{
    delete del;
    del = nullptr;
}


int main()
{

    using fp = void(*)(string *);
    unique_ptr<string, fp> ps1(new string("I Love China!"), UniqueDeleter);

    unique_ptr<string, decltype(UniqueDeleter)*> ps2(new string("I Love China1"),UniqueDeleter);
    auto delFunction = [](string * del) {delete del; del = nullptr;};
    unique_ptr<string, decltype(delFunction)> ps3(new string("I Love China3"), delFunction);



    // auto ps2 = tuniqp();

    // shared_ptr<string> pss1 = myfunc();     //引用计数加1


    // shared_ptr<int> p2 = make_shared<int>(100);         //int * pi = new int(100);
    // shared_ptr<string> p3 = make_shared<string>(5,'a');     //5 ge 'a' 字符


    // shared_ptr<int> p4 = make_shared<int>();        //int 0;

    // p4 = make_shared<int>(50);          //重新挂到新的对象，释放之前的对象

    // auto p6 = make_shared<int>(100);

    // auto p7(p6);

    // auto p8 = myfunc(p7);


    // cout << "fda" << endl;


    // p8 = make_shared<int>(100);         //p8 指向新对象，计数为1，p6,p7恢复为2
    // p7 = make_shared<int>(200);         //p7 指向新对象，计数为1，p6恢复为1



    // shared_ptr<int> p(new int(12345), myDeleter);
    // shared_ptr<int> p2(p);              
    // p2.reset();
    // p.reset();          //

    // shared_ptr<int> p9(new int(12343), [](int * p){
    //     delete p;
    // });

    // p.reset();          //同样会调用


    // auto p1 = make_shared<int>(100);
    // weak_ptr<int> pw;
    // pw = p1;        //一强一弱

    // if(!pw.expired())
    // {
    //     //没过期
    //     auto p2 = pw.lock();            //2强一弱
    //     if(p2 != nullptr)
    //     {
    //         cout << "所指对象存在" << endl;
    //     }
    //     //注意，离开这个范围，p2会被释放，强引用计数减一
    // }
    // else {cout << "过期了" << endl;}

    // p1.reset();

    // if(pw.expired())
    // {
    //     cout << "已经过期了" << endl;
    // }


    // {
    //     auto p1 = make_shared<int>(42);
    //     pw = p1;
    // }

    // if(pw.expired())
    // {
    //     cout << "已经过期了" << endl;
    // }

    // shared_ptr<int> myp(new int(100));
    // int * p = myp.get();
    // {
    //     shared_ptr<int> myp2(p);
    // }
    // *myp = 45;

    // shared_ptr<int> p1(new int(100));
    // cout << *p1 << endl;

    // shared_ptr<int> p2(move(p1));
    // cout << *p1 << endl;
    // cout << p1 << endl;
    

    // unique_ptr<int> ps1(new int(20));
    // unique_ptr<int> ps2 = make_unique<int>(100);
    // auto ps3 = make_unique<int>(100);

    // ps3 = move(ps1);

    // cout << *ps1 << endl;

    // cout << *ps3 << endl;

    // unique_ptr<int> ps1;

    // unique_ptr<int[]> ptrarray(new int[10]);

    // ptrarray[0] = 12;
    // ptrarray[1] = 24;
    // ptrarray[9] = 123;


    // cout << ptrarray[3];
    
    
    
    return 0;





}