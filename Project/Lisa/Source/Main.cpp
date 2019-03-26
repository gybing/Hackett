#include "../JuceLibraryCode/JuceHeader.h"

class Database
{
public:
    Database()
    {

    }
    ~Database()
    {
        
    }

};

class Memory
{
public:
    juce_DeclareSingleton(Memory, false)

private:
    ScopedPointer<Database> database;
};

class Query
{
public:
    
};

class Output
{
public:
    juce_DeclareSingleton(Output, false)

};

class Input
{
public:
    juce_DeclareSingleton(Input, false)
};

juce_ImplementSingleton(Memory);
juce_ImplementSingleton(Output);
juce_ImplementSingleton(Input);

//==============================================================================
int main (int argc, char* argv[])
{
    // start
    // check input and output
    // load memory
    // - if empty then create lisa
    // - else run

    return 0;
}
