/*
  ==============================================================================

    DSP4GuitarApp.h
    Conceptual outline for a JUCE-based VST host application.

    This is a simplified example to demonstrate:
    - ASIO device setup via JUCE's AudioDeviceManager.
    - VST plugin scanning and loading.
    - Basic audio processing through a loaded plugin.
    - A minimal UI for interaction.

    To build a full application, you would need to:
    - Set up a JUCE project using the Projucer or CMake.
    - Expand UI for plugin selection, parameter control, FX chain management.
    - Implement robust error handling and resource management.
    - Add features like preset saving/loading, MIDI processing, etc.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

// ==============================================================================
// Main Application Class (Entry Point)
// ==============================================================================
class DSP4GuitarApplication : public juce::JUCEApplication
{
public:
    DSP4GuitarApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code
        mainWindow.reset(new MainHostWindow(getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here.
        mainWindow = nullptr; // (deletes our window)
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    // ==============================================================================
    // Main Window Class
    // ==============================================================================
    class MainHostWindow : public juce::DocumentWindow
    {
    public:
        MainHostWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            // The MainAudioComponent will be the heart of our app
            mainAudioComponent = std::make_unique<MainAudioComponent>();
            setContentOwned(mainAudioComponent.get(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
           #else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
           #endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        std::unique_ptr<class MainAudioComponent> mainAudioComponent;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainHostWindow)
    };

private:
    std::unique_ptr<MainHostWindow> mainWindow;
};


// ==============================================================================
// Main Audio Component (Handles Audio, Plugins, and Basic UI)
// ==============================================================================
class MainAudioComponent : public juce::AudioAppComponent,
                           public juce::Button::Listener,
                           public juce::ComboBox::Listener,
                           private juce::Timer
{
public:
    MainAudioComponent()
        : AudioAppComponent(audioDeviceManager) // Pass the device manager to the base class
    {
        // Initialize AudioDeviceManager and request ASIO if available
        // The AudioAppComponent constructor will call setAudioChannels based on device capabilities.
        // We can further refine this in prepareToPlay.
        juce::String audioError = audioDeviceManager.initialiseWithDefaultDevices(2, 2); // 2 ins, 2 outs
        if (audioError.isNotEmpty())
        {
            juce::Logger::writeToLog("Audio Device Init Error: " + audioError);
            // Handle error appropriately, e.g., show a message to the user
        }
        
        // On Windows, try to set up ASIO by default if a device is available
        #if JUCE_WINDOWS
        juce::OwnedArray<juce::AudioIODeviceType> availableTypes;
        audioDeviceManager.createAudioDeviceTypes(availableTypes);
        for (auto* type : availableTypes)
        {
            if (type->getTypeName() == "ASIO")
            {
                audioDeviceManager.setCurrentAudioDeviceType("ASIO", true);
                break;
            }
        }
        #endif

        // Setup UI elements
        addAndMakeVisible(openAudioSettingsButton);
        openAudioSettingsButton.setButtonText("Audio Settings...");
        openAudioSettingsButton.addListener(this);

        addAndMakeVisible(scanPluginsButton);
        scanPluginsButton.setButtonText("Scan VST Plugins");
        scanPluginsButton.addListener(this);

        addAndMakeVisible(pluginSelectionComboBox);
        pluginSelectionComboBox.setTextWhenNoChoicesAvailable("No plugins found");
        pluginSelectionComboBox.addListener(this);
        pluginSelectionComboBox.setTooltip("Select a VST plugin to load");

        addAndMakeVisible(statusLabel);
        statusLabel.setText("Welcome to DSP4Guitar Host!", juce::dontSendNotification);
        statusLabel.setJustificationType(juce::Justification::centred);

        // Plugin Format Manager - for VST2 and VST3
        // You can add other formats like AU if targeting macOS
        pluginFormatManager.addDefaultFormats();

        // Start a timer to periodically check for new plugins if scanning in background
        // For this example, we'll do a blocking scan.
        // startTimer(5000); // Check every 5 seconds

        // Set component size
        setSize(600, 400);

        // Tell AudioAppComponent we want 2 input and 2 output channels
        setAudioChannels(2, 2); 
    }

    ~MainAudioComponent() override
    {
        shutdownAudio(); // This will call releaseResources()
        loadedPluginInstance = nullptr; // Release the plugin
    }

    //================ Audio Callbacks =========================================
    void prepareToPlay(int samplesPerBlockExpected, double newSampleRate) override
    {
        currentSampleRate = newSampleRate;
        currentBlockSize = samplesPerBlockExpected;

        if (loadedPluginInstance)
        {
            loadedPluginInstance->setRateAndBufferSizeDetails(newSampleRate, samplesPerBlockExpected);
            loadedPluginInstance->prepareToPlay(newSampleRate, samplesPerBlockExpected);
        }
        
        // You might want to prepare other things here, like internal buffers.
        statusLabel.setText("Audio prepared. Rate: " + juce::String(newSampleRate, 1) + " Hz, BlockSize: " + juce::String(samplesPerBlockExpected), juce::dontSendNotification);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        // This is the main audio processing callback.
        // Audio comes in bufferToFill.buffer, and should be processed in place
        // or written to bufferToFill.buffer.

        // Mute output if no plugin is loaded or if processing fails
        // bufferToFill.clearActiveBufferRegion(); // Good practice to clear first

        if (loadedPluginInstance && loadedPluginInstance->isSuspended() == false)
        {
            // JUCE's AudioBuffer is what plugins expect
            // The AudioSourceChannelInfo gives us a direct pointer to the audio data.
            // We need to wrap it in an AudioBuffer for the plugin.
            // Note: This is a simplified example. For complex routing or if the plugin
            // has a different channel configuration, more sophisticated buffer management is needed.
            
            // Assuming plugin matches host channel config (stereo in, stereo out)
            // If your plugin has a different layout, you'll need to adapt.
            juce::AudioBuffer<float> pluginBuffer(bufferToFill.buffer->getArrayOfWritePointers(),
                                                  bufferToFill.buffer->getNumChannels(),
                                                  bufferToFill.startSample,
                                                  bufferToFill.numSamples);


            // Process the audio through the plugin
            loadedPluginInstance->processBlock(pluginBuffer, juce::MidiBuffer()); // Pass an empty MIDI buffer if not handling MIDI
        }
        else
        {
            // If no plugin, or plugin suspended, pass audio through (or silence)
            // For pass-through, do nothing if bufferToFill.buffer already contains input.
            // For silence: bufferToFill.clearActiveBufferRegion(); (already done above)
        }
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when samplesPerBlockExpected/sampleRate change.
        if (loadedPluginInstance)
        {
            loadedPluginInstance->releaseResources();
        }
        currentSampleRate = 0;
        currentBlockSize = 0;
        statusLabel.setText("Audio resources released.", juce::dontSendNotification);
    }

    //================ UI Callbacks ============================================
    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(10);

        juce::Rectangle<int> topRow = area.removeFromTop(40);
        openAudioSettingsButton.setBounds(topRow.removeFromLeft(150));
        topRow.removeFromLeft(10); // spacer
        scanPluginsButton.setBounds(topRow.removeFromLeft(150));
        
        area.removeFromTop(10);
        pluginSelectionComboBox.setBounds(area.removeFromTop(30));
        area.removeFromTop(10);
        statusLabel.setBounds(area.removeFromTop(30));
    }

    void buttonClicked(juce::Button* button) override
    {
        if (button == &openAudioSettingsButton)
        {
            // JUCE provides a handy component for audio settings
            juce::AudioDeviceSelectorComponent settingsComp(audioDeviceManager,
                0, 256,     // min/max input channels
                0, 256,     // min/max output channels
                true,       // showMidiInputOptions
                true,       // showMidiOutputSelector
                true,       // showChannelsAsStereoPairs
                true);      // showChannelNames
            settingsComp.setSize(500, 450);

            juce::DialogWindow::LaunchOptions o;
            o.content.setOwned(&settingsComp);
            o.content.setSize(500, 450);
            o.dialogTitle = "Audio Settings";
            o.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
            o.escapeKeyTriggersCloseButton = true;
            o.useNativeTitleBar = false;
            o.launchAsync();
        }
        else if (button == &scanPluginsButton)
        {
            scanForPlugins();
        }
    }

    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == &pluginSelectionComboBox)
        {
            loadSelectedPlugin();
        }
    }
    
    //================ Plugin Management =======================================
    void scanForPlugins()
    {
        statusLabel.setText("Scanning for VST plugins...", juce::dontSendNotification);
        knownPluginList.clear(); // Clear previous scan results

        // You can specify paths to scan, or use default VST paths.
        // For simplicity, using default paths.
        // juce::FileSearchPath searchPath;
        // searchPath.addDefaultPluginSearchPaths();

        // For VST3, JUCE typically finds them automatically.
        // For VST (legacy), you might need to specify paths.
        // Example: searchPath.add(juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory).getChildFile("VST"));
        
        pluginSelectionComboBox.clear(juce::dontSendNotification);
        pluginDescriptions.clear();

        for (int i = pluginFormatManager.getNumFormats(); --i >= 0;)
        {
            juce::AudioPluginFormat* format = pluginFormatManager.getFormat(i);
            // We are interested in VST and VST3
            if (format->getName() == "VST" || format->getName() == "VST3")
            {
                // The true flag means it's a blocking scan.
                // For background scanning, you'd use a PluginDirectoryScanner.
                // This can take some time.
                juce::PluginDirectoryScanner scanner (knownPluginList, *format,
                                                      format->getDefaultLocationsToSearch(),
                                                      true, /*recursive*/
                                                      juce::File(), /*deadMansPedal*/
                                                      true /*allowPluginsWhichRequireRestart*/);
                
                juce::String pluginPathBeingScanned;
                while(scanner.scanNextFile(true, pluginPathBeingScanned)) {
                    // Optionally update UI with pluginPathBeingScanned
                }
            }
        }

        // Populate the ComboBox
        int itemId = 1;
        for (const juce::PluginDescription* desc : knownPluginList.getTypes())
        {
            // Only add plugins that are VST or VST3 and can be used as effects
            // (i.e., has audio inputs and outputs)
            if ((desc->pluginFormatName == "VST" || desc->pluginFormatName == "VST3") &&
                desc->hasAudioInput() && desc->hasAudioOutput() && !desc->isInstrument())
            {
                pluginSelectionComboBox.addItem(desc->name + " (" + desc->pluginFormatName + ")", itemId++);
                pluginDescriptions.add(*desc); // Store the description for later loading
            }
        }

        if (pluginSelectionComboBox.getNumItems() > 0)
        {
            pluginSelectionComboBox.setSelectedId(1, juce::sendNotification); // Select first plugin by default
            statusLabel.setText("Plugin scan complete. Select a plugin.", juce::dontSendNotification);
        }
        else
        {
            statusLabel.setText("No suitable VST effect plugins found.", juce::dontSendNotification);
        }
        pluginSelectionComboBox.setEnabled(pluginSelectionComboBox.getNumItems() > 0);
    }

    void loadSelectedPlugin()
    {
        int selectedId = pluginSelectionComboBox.getSelectedId();
        if (selectedId == 0 || selectedId > pluginDescriptions.size()) // 0 means no selection
        {
            statusLabel.setText("Invalid plugin selection.", juce::dontSendNotification);
            return;
        }

        const juce::PluginDescription& desc = pluginDescriptions[selectedId - 1];

        // Release previous plugin if any
        if (loadedPluginInstance)
        {
            if (currentSampleRate > 0 && currentBlockSize > 0)
                loadedPluginInstance->releaseResources();
            loadedPluginInstance = nullptr;
        }
        
        juce::String errorMessage;
        statusLabel.setText("Loading plugin: " + desc.name, juce::dontSendNotification);

        // Create instance of the plugin
        // This can also take some time and potentially crash if the plugin is faulty.
        // Consider doing this on a background thread or with error boundaries in a real app.
        loadedPluginInstance = pluginFormatManager.createPluginInstance(desc, currentSampleRate, currentBlockSize, errorMessage);

        if (!loadedPluginInstance)
        {
            statusLabel.setText("Failed to load plugin: " + desc.name + ". Error: " + errorMessage, juce::dontSendNotification);
            juce::Logger::writeToLog("Plugin Load Error: " + errorMessage);
            return;
        }

        // Prepare the plugin for playback
        if (currentSampleRate > 0 && currentBlockSize > 0) // Check if audio device is ready
        {
            loadedPluginInstance->setRateAndBufferSizeDetails(currentSampleRate, currentBlockSize);
            loadedPluginInstance->prepareToPlay(currentSampleRate, currentBlockSize);
        }
        
        // Enable processing
        loadedPluginInstance->enableAllBuses(); // Ensure buses are enabled

        statusLabel.setText("Loaded plugin: " + desc.name, juce::dontSendNotification);
        juce::Logger::writeToLog("Loaded plugin: " + desc.name + " (" + desc.fileOrIdentifier + ")");

        // Optional: Show plugin editor
        // auto* editor = loadedPluginInstance->createEditor();
        // if (editor)
        // {
        //     // Add editor to your UI. This requires more setup.
        //     // For simplicity, not shown here.
        // }
    }


private:
    //================ Member Variables ========================================
    // Audio Device Manager - handles device selection (ASIO, CoreAudio, etc.)
    // This is already a member of AudioAppComponent, but we keep a reference for clarity if needed.
    // juce::AudioDeviceManager& audioDeviceManager; (inherited)

    // Plugin Management
    juce::AudioPluginFormatManager pluginFormatManager;
    juce::KnownPluginList knownPluginList;
    juce::Array<juce::PluginDescription> pluginDescriptions; // To map ComboBox IDs to descriptions
    std::unique_ptr<juce::AudioPluginInstance> loadedPluginInstance;

    // UI Elements
    juce::TextButton openAudioSettingsButton;
    juce::TextButton scanPluginsButton;
    juce::ComboBox pluginSelectionComboBox;
    juce::Label statusLabel;

    // Audio state
    double currentSampleRate = 0.0;
    int currentBlockSize = 0;

    // Timer callback (not used in this simplified blocking scan example)
    void timerCallback() override {
        // Example: could trigger a non-blocking scan for new plugins
        // knownPluginList.scanAndAddDragAndDroppedFiles();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};


//==============================================================================
// This macro generates the main() function that launches the app.
// You need a Main.cpp file in your JUCE project that contains ONLY this:
//
// #include "DSP4GuitarApp.h" /* (or whatever your main app header is called) */
// START_JUCE_APPLICATION (DSP4GuitarApplication)
//
// For this example, I'm putting a simplified version here to make it runnable
// in a conceptual way, but in a real JUCE project, this would be in its own file.
//==============================================================================

/*
// In your Main.cpp or equivalent:
#include "AppHeader.h" // Assuming the classes above are in AppHeader.h

// This macro creates the application's main() function.
START_JUCE_APPLICATION (DSP4GuitarApplication)
*/
