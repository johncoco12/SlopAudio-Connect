#include <JuceHeader.h>
#include "GUI/MainWindow.h"
#include "GUI/LogConsoleWindow.h"
#include "App/AppState.h"
#include "App/SessionController.h"

struct AppLogger : public juce::Logger
{

    std::shared_ptr<LogConsoleWindow> window;

    void logMessage(const juce::String& msg) override
    {
        const juce::String line =
            "[" + juce::Time::getCurrentTime().toString(false, true, true, true) + "] " + msg;

        std::fputs((line + "\n").toRawUTF8(), stderr);

        if (auto w = window)
        {
            juce::MessageManager::callAsync([w, line]
            {
                w->appendLine(line);
            });
        }
    }
};

class SlopAudioConnectApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override
    {
        return JUCE_APPLICATION_NAME_STRING;
    }
    const juce::String getApplicationVersion() override
    {
        return JUCE_APPLICATION_VERSION_STRING;
    }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        appLogger = std::make_unique<AppLogger>();
        juce::Logger::setCurrentLogger(appLogger.get());

        const bool verbose = commandLine.containsWholeWord("--verbose");

        if (verbose)
        {
            appLogger->window = std::make_shared<LogConsoleWindow>();
            juce::Logger::writeToLog("SlopAudio Connect starting — verbose mode");
        }

        appState          = std::make_unique<AppState>();
        sessionController = std::make_unique<SessionController>(*appState);
        mainWindow        = std::make_unique<MainWindow>(
            getApplicationName(), *appState, *sessionController);
    }

    void shutdown() override
    {
        sessionController->disconnect();

        mainWindow.reset();
        sessionController.reset();
        appState.reset();

        if (appLogger)
            appLogger->window.reset();

        juce::Logger::setCurrentLogger(nullptr);
        appLogger.reset();
    }

    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<AppLogger>         appLogger;
    std::unique_ptr<AppState>          appState;
    std::unique_ptr<SessionController> sessionController;
    std::unique_ptr<MainWindow>        mainWindow;
};

START_JUCE_APPLICATION(SlopAudioConnectApp)
