这里介绍一些项目会用到的设计模式和代码。

# 单例

[wiki](https://en.wikipedia.org/wiki/Singleton_pattern)

我们项目中相当一部分类本质上都是一个命名空间，只不过因为命名空间不好管理变量所以才写成了类。为了简便，这些类都是单例模式。

这些单例的唯一类变量：

1. 不可能通过任何方式直接构造、复制构造。
2. 必须被显式初始化，可以有多种初始化方式。
3. 可以被显式析构，但是在析构之后再需要使用就需要再次显式初始化。
4. 会在程序结束的时候自动析构。
5. 都可以通过类静态函数获得。

单例模式的代码如下：

```c++
class Single{
private:
    std::vector<int>datavi;
    int * datapi;
    static std::unique_ptr<Single>single; 			// 	唯一的Single实例。注意需要在.cpp文件初始化它为nullptr
    Single(){										//	默认构造函数private（也可以delete）
        datapi=new int(0);
    };
    Single(int i){
        datapi=new int(i);
    }
public:
	Single(const Single &)=delete;					//	拷贝构造函数显式删除
    Single & operator = (const Single &)=delete;	//	赋值函数显式删除
    ~Single(){
        delete datapi;								//	自动析构类就会自动释放指针，防止内存泄漏
    }
    static void init(){								//	第一种初始化方式
        single=std::unique_ptr<Single>(new Single());
        //	这里不能用std::make_unique，因为构造函数都是私有的
    }
    static void init(int i){						//	第二种初始化方式
        single=std::unique_ptr<Single>(new Single(i));
    }
    static Single & get(){
        if(!single) com::ThrowSingleNotInited();	//	判断是否初始化过。这里建议throw异常。
        return *single;
    }
    static void del(){
        single=nullptr;								//	因为使用了unique_ptr，所以可以自动析构
    }
}
```



这和广为流传的[另一种单例写法](https://stackoverflow.com/questions/1008019/c-singleton-design-pattern)相比，可以自定义构造方式，可以使用过程中析构已有单例然后新构造一个，更灵活一些。（关于[thread-safe](https://stackoverflow.com/a/449823/52074)，因为我不熟悉这部分内容，我们项目又不可能用多线程，所以就不讨论了）