/*
  ==============================================================================

    EmptyComponent.h
    Created: 23 Apr 2025 8:13:29pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "SwappableComponent.h"
#include "../Processors/SwappableProcessor.h"

class EmptyComponent : public SwappableComponent {

public:
    /*
    EmptyComponent(EmptyProcessor* emptyProcessor) : SwappableComponent(emptyProcessor) {
        menu.addItem("EMPTY", 1);
        menu.addItem("GATE", 2);
        menu.addItem("DISTORTION", 3);
        menu.addItem("FLANGER", 4);
        menu.setSelectedId(1);
        addAndMakeVisible(menu);

        menu.onChange = [this]() {
            int selectedId = menu.getSelectedId();
            juce::String selectedText = menu.getText();
            //signal the UI to do something
            //int index = getIndexInComponentList();
            int index = 1;
            sendActionMessage("CREATECOMPONENT_" + String(index) + "_" + selectedText);
            //signal the processor to do something

            };
    }*/

    EmptyComponent(EmptyProcessor* emptyProcessor, int index);
    ~EmptyComponent() {}
    void resized() override {
        menu.setBounds(getLocalBounds().withTrimmedBottom(getLocalBounds().getHeight() * 0.9));
    }

private:

    CustomComboBox menu;
};

