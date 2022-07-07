class A
{
    public:
        /** 
        * @brief 类中定义了的函数是隐式内联函数，声明想要成为内联函数，必须在实现处（定义处）加inline关键字
        * 
        * @param x 
        * @param y 
        */
       void Foo(int x, int y)//定义即隐式内联函数！
       {
       };

       void f1(int x);///<声明后，要想成为内联函数，必须加上inline关键字
};