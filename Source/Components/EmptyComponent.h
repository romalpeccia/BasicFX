/*
  ==============================================================================

    EmptyComponent.h
    Created: 23 Apr 2025 8:13:29pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "../SwappableComponentManager.h"
#include "../Processors/SwappableProcessor.h"

class EmptyComponent : public SwappableComponent {

public:
    EmptyComponent(int index);
    void setComponentAttachments();
    ~EmptyComponent() {}
    void resized() override {
        menu.setBounds(getLocalBounds().withTrimmedBottom(getLocalBounds().getHeight() * 0.9));
    }

private:

    CustomComboBox menu;
};

