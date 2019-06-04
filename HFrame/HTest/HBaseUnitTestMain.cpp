#include <HBase.h>

class ConsoleLogger : public Logger
{
	void logMessage(const String& message) override
	{
		std::cout << message << std::endl;

#if HWINDOWS
		Logger::outputDebugString(message);
#endif
	}
};

//==============================================================================
class ConsoleUnitTestRunner : public UnitTestRunner
{
	void logMessage(const String& message) override
	{
		Logger::writeToLog(message);
	}
};


//==============================================================================
int main(int argc, char** argv)
{
	ConsoleLogger logger;
	Logger::setCurrentLogger(&logger);

	ConsoleUnitTestRunner runner;

	ConsoleApplication app;

	int64 seed = Random::getSystemRandom().nextInt64();

	app.addHelpCommand("--help|-h", "Usage: " + String(argv[0]) + "[--help|-h] [--list-categories] [--category category] [--seed seed]", true);
	app.addVersionCommand("--version|-v", "MyApp version 1.2.3");

	app.addCommand({ "--list-categories", "--list-categories name", "Performs a categories operation on the given name", "", [&](const auto& args)
	{
		for (auto& category : UnitTest::getAllCategories())
		std::cout << category << std::endl;
	}});

	app.addCommand({ "--seed", "--seed name", "Performs a categories operation on the given name", "", [&](const auto& args)
	{
		seed = args.getValueForOption("--seed").getLargeIntValue();
	} });

	app.addCommand({ "--category", "--category name", "Performs a categories operation on the given name", "", [&](const auto& args)
	{
		runner.runTestsInCategory(args.getValueForOption("--category"), seed);
	} });

	app.addCommand({ "--all", "--all name", "Performs a categories operation on the given name", "", [&](const auto& args)
	{
		runner.runAllTests(seed);
	} });


	// read file line by line
	std::istringstream input;
	input.str("1\n2\n3\n4\n5\n6\n7\n");
	int sum = 0;
	for (std::string line; std::getline(input, line); ) {

		String lines(line);
		lines.to
		app.findAndRunCommand(argc, argv);
	}

	ConsoleLogger.setCurrentLogger(nullptr);
	return 0;
}