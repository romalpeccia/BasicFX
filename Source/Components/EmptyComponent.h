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
    //EmptyComponent(int index);
    EmptyComponent(int index, SwappableComponentManager* manager);
    ~EmptyComponent() {}

    void setComponentAttachments(int index) override;

    void resized() override {
        auto bounds = getLocalBounds();

        auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
        menu.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));

        auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    }

private:

    CustomComboBox menu;
};

