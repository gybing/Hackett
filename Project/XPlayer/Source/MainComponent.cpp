/*
  ==============================================================================

  This file was auto-generated!

  ==============================================================================
  */

#include "MainComponent.h"
#include "memory"
#include <map>
#include <vector>

class A
{
public:
    template <typename Iterator>
    void bar(Iterator begin, Iterator end)
    {
        std::for_each(begin, end, [](int n) {std::cout << n << std::endl; });
        auto is_odd = [](int n) {return n % 2 == 1; };
        auto pos = std::find_if(begin, end, is_odd);
        if (pos != end)
            std::cout << *pos << std::endl;
    }

    template <typename C>
    void foo(C c)
    {
        bar(std::begin(c), std::end(c));
    }

    template <typename T, size_t N>
    void foo(T(&arr)[N])
    {
        bar(std::begin(arr), std::end(arr));
    }

    A()
    {
        int arr[] = { 1, 2, 3 };
        foo(arr);
    }
};

template <typename T1, typename T2>
auto add(T1 t1, T2 t2) -> decltype(t1)
{
    static_assert(std::is_integral<T2>::value, "Type T2 must be integral");
    return t1 + t2;
}




template<typename T>
class Buffer
{
    std::string _name;
    size_t _size;
    std::unique_ptr<T[]>_buffer;
public:

    //defaultconstructor
    Buffer()
        : _size(16), _buffer(new T[16])
    {
    }

    //constructor
    Buffer(const std::string&name, size_t size)
        :_name(name), _size(size), _buffer(new T[size])
    {
    }

    //copy constructor
    Buffer(const Buffer& copy)
        :_name(copy._name), _size(copy._size), _buffer(new T[copy._size])
    {
        T* source = copy._buffer.get();
        T* dest = _buffer.get();
        std::copy(source, source + copy._size, dest);
    }

    //copy assignment operator
    Buffer& operator = (const Buffer&copy)
    {
        if (this != copy)
        {
            _name = copy._name;
            if (_size != copy._size)
            {
                _buffer = nullptr;
                _size = copy._size;
                _buffer = _size > 0 ? new T[_size] : nullptr;
            }

            T* source = copy._buffer.get();
            T* dest = _buffer.get();
            std::copy(source, source + copy._size, dest);
        }
        return*this;
    }

    //move constructor
    Buffer(Buffer&& temp)
        :_name(std::move(temp._name)), _size(temp._size), _buffer(std::move(temp._buffer))
    {
        temp._buffer = nullptr;
        temp._size = 0;
    }

    //move assignment operator
    Buffer&operator = (Buffer&&temp)
    {
        jassert(this != &temp);

        //assert if this is not atemporary
        _buffer = nullptr;
        _size = temp._size;
        _buffer = std::move(temp._buffer);
        _name = std::move(temp._name);
        temp._buffer = nullptr;
        temp._size = 0;
        return *this;
    }
};

template<typename T>
Buffer<T> getBuffer(const std::string& name)
{
    Buffer<T> b(name, 128);
    return b;
}



//==============================================================================
MainContentComponent::MainContentComponent()
    : DirectShowComponent(DirectShowComponent::dshowEVR)
{

    Buffer<int> b1;
    Buffer<int> b2("buf2", 64);
    Buffer<int> b3 = b2;
    Buffer<int> b4 = getBuffer<int>("buf4");
    b1 = getBuffer<int>("buf5");





    std::vector<int> vec;
    int64 cur, t = juce::Time::currentTimeMillis();

    int sumTest = 0;

    for (unsigned int i = 0; i < 100000000; ++i)
    {
        vec.push_back(i*i);
    }

    cur = juce::Time::currentTimeMillis();
    int64 s1 = cur - t;
    t = cur;

    auto it = vec.begin();
    auto itEnd = vec.end();
    for (; it != itEnd; ++it)
    {
        sumTest += *it;
    }

    cur = juce::Time::currentTimeMillis();
    int64 s2 = cur - t;
    t = cur;

    std::for_each(vec.begin(), vec.end(), [&](const int& m)
    {
        sumTest += m;
    });

    cur = juce::Time::currentTimeMillis();
    int64 s3 = cur - t;
    t = cur;

    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        sumTest += *it;
    }

    cur = juce::Time::currentTimeMillis();
    int64 s4 = cur - t;
    t = cur;

    for each (auto m in vec)
    {
        sumTest += m;
    }

    cur = juce::Time::currentTimeMillis();
    int64 s5 = cur - t;
    t = cur;

    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        sumTest += vec[i];
    }

    cur = juce::Time::currentTimeMillis();
    int64 s6 = cur - t;
    t = cur;

    unsigned int c = 0;
    const unsigned int s = vec.size();
    for (; c < s; ++c)
    {
        sumTest += vec[c];
    }

    cur = juce::Time::currentTimeMillis();
    int64 s7 = cur - t;
    t = cur;

    cur = s1 + s2 + s3 + s4 + s5 + s6 + s7;
    /* debug        release
    s1:81        s1: 701
    s2:52        s2: 49
    s3:5         s3: 51
    s4:147       s4: 49
    s5:45        s5: 49
    s6:9         s6: 50
    s7:7         s7: 50
    */
    // 
    /* debug        release
    t1:3916      t1: 1132
    t2:118       t2: 18
    t3:77        t3: 18
    t4:248       t4: 17
    t5:94        t5: 18
    */
    
    
    */


    std::map<int, std::pair<int, int>> mapPair;
    t = juce::Time::currentTimeMillis();

    sumTest = 0;

    for (unsigned int i = 0; i < 1000000; ++i)
    {
        mapPair[i] = std::pair<int, int>(i+i, i*i);
    }

    cur = juce::Time::currentTimeMillis();
    int64 t1 = cur - t;
    t = cur;

    auto its = mapPair.begin();
    auto itEnds = mapPair.end();
    for (; its != itEnds; ++its)
    {
        sumTest += its->second.second;
    }

    cur = juce::Time::currentTimeMillis();
    int64 t2 = cur - t;
    t = cur;

    std::for_each(mapPair.begin(), mapPair.end(), [&](const std::pair<int, std::pair<int, int>>& m)
    {
        sumTest += m.second.second;
    });

    cur = juce::Time::currentTimeMillis();
    int64 t3 = cur - t;
    t = cur;

    for (auto it = mapPair.begin(); it != mapPair.end(); ++it)
    {
        sumTest += it->second.second;
    }

    cur = juce::Time::currentTimeMillis();
    int64 t4 = cur - t;
    t = cur;

    for each (auto m in mapPair)
    {
        sumTest += m.second.second;
    }

    cur = juce::Time::currentTimeMillis();
    int64 t5 = cur - t;
    t = cur;

    
    cur = t1 + t2 + t3 + t4 + t5;
    // std::for_each < foreach < for < for
    /* debug        release
    t1:3916      t1: 1132
    t2:118       t2: 18
    t3:77        t3: 18
    t4:248       t4: 17
    t5:94        t5: 18
    */

    juce::FileOutputStream fs(juce::File(L"D:\\testlogs.txt"));
    
    fs.writeString("t1:");
    fs.writeString(String(t1));
    fs.writeString(" ");
    fs.writeString("t2:");
    fs.writeString(String(t2));
    fs.writeString(" ");
    fs.writeString("t3:");
    fs.writeString(String(t3));
    fs.writeString(" ");
    fs.writeString("t4:");
    fs.writeString(String(t4));
    fs.writeString(" ");
    fs.writeString("t5:");
    fs.writeString(String(t5));
    fs.writeString(" ");

    fs.writeString("s1:");
    fs.writeString(String(s1));
    fs.writeString(" ");
    fs.writeString("s2:");
    fs.writeString(String(s2));
    fs.writeString(" ");
    fs.writeString("s3:");
    fs.writeString(String(s3));
    fs.writeString(" ");
    fs.writeString("s4:");
    fs.writeString(String(s4));
    fs.writeString(" ");
    fs.writeString("s5:");
    fs.writeString(String(s5));
    fs.writeString(" ");
    fs.writeString("s6:");
    fs.writeString(String(s6));
    fs.writeString(" ");
    fs.writeString("s7:");
    fs.writeString(String(s7));
    fs.writeString(" ");

    //add("a", "b");
    setSize(600, 400);

    TextButton* b = new TextButton();
    b->setButtonText(L"加载");
    b->setBounds(0, 0, 200, 50);
    b->addListener(this);
    addAndMakeVisible(b);

    TextButton* bb = new TextButton;
    std::shared_ptr<TextButton> sb(b);

    std::map<std::string, std::vector<int>> map;
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    map["one"] = v;

    for (const auto& kvp : map)
    {
        std::cout << kvp.first << std::endl;

        for (auto v : kvp.second)
        {
            std::cout << v << std::endl;
        }
    }

    enum class Options {
        None, One, All
    };

    Options o = Options::All;

    enum Suit { Diamonds = 5, Hearts = 5, Clubs = 4, Spades };
    if (Diamonds == Spades)
    {
        if (Diamonds == Hearts)
        {
            int a = 0;
        }
        int a = 0;
    }

        {
            std::vector<int> v;
            v.push_back(1);
            v.push_back(2);
            v.push_back(3);

            std::map<int, int> m;
            m[1] = 1;
            m[2] = 2;
            m[3] = 3;

            std::for_each(m.begin(), m.end(), [](std::pair<const int, int> n){
                int a = n.second;
            });
            std::for_each(v.begin(), v.end(), [](int n){
                int a = n;
            });
        }

        {
            std::function<int(int)> lfib = [&lfib](int n) {
                return n < 2 ? 1 : lfib(n - 1) + lfib(n - 2);
            };

            std::vector<int> f;
            for (unsigned int i = 0; i < 10; ++i)
            {
                f.push_back(lfib(i));
            }
            int a = lfib(10);
            int b = a;
        }

        {
            

            A a;
            // static_assert(0 > 0, "i got a static assert false!");
        }

        {
            
        }
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::visibilityChanged()
{
    DirectShowComponent::visibilityChanged();
}

void MainContentComponent::buttonClicked(Button*)
{
    loadMovie(L"http://static.zqgame.com/html/playvideo.html?name=http://lom.zqgame.com/v1/video/LOM_Promo~2.flv");
    play();
}
