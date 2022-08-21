#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void initializeValueTree();
    void addToValueTree();
    void removeFromValueTree();

    //==============================================================================
    juce::ValueTree vtComponentBuilderRoot;
    std::unique_ptr<juce::ComponentBuilder> componentBuilder;
    juce::Component* builderRootComponent;
    
    std::unique_ptr<juce::TextButton> addButton;
    std::unique_ptr<juce::TextButton> removeButton;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
