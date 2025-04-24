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
    EmptyComponent(EmptyProcessor* emptyProcessor) : SwappableComponent(emptyProcessor) {};
    void resized() override {};

private:

    //CustomTextButton button{ "X"};
};

