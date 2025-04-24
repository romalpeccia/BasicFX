/*
  ==============================================================================

    EmptyComponent.cpp
    Created: 23 Apr 2025 8:13:29pm
    Author:  romal

  ==============================================================================
*/

#include "EmptyComponent.h"
#include "../SwappableComponentManager.h" 

EmptyComponent::EmptyComponent(EmptyProcessor* emptyProcessor) : SwappableComponent(emptyProcessor) {
    menu.addItem("EMPTY", 1);
    menu.addItem("GATE", 2);
    menu.addItem("DISTORTION", 3);
    menu.addItem("FLANGER", 4);
    menu.setSelectedId(1);
    addAndMakeVisible(menu);

    menu.onChange = [this]() {
        int index = swappableComponentManager->findComponentIndex(*this);
        int selectedId = menu.getSelectedId();
        juce::String selectedText = menu.getText();
        //signal the UI to do something
        sendActionMessage("CREATECOMPONENT_" + String(index) + "_" + selectedText);
        //signal the processor to do something

        };
}
