/*
  ==============================================================================

    SwappableComponent.h
    Created: 14 Apr 2025 7:56:50pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utilities.h"
class BasicFXAudioProcessorEditor;

class SwappableComponent : public juce::Component {
    public:
        SwappableComponent() = delete;
        SwappableComponent(ProcessorType type) : processorType(type) {
            swappableComponentList.push_back(this);
            DBG("SwappableComponent added: " << static_cast<int>(type));
        }
        ~SwappableComponent();





        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void swapComponents(SwappableComponent* otherComp);

        ProcessorType getProcessorType() const { return processorType; }
        static void setEditorReference(BasicFXAudioProcessorEditor* editor) {
            SwappableComponent::editorInstance = editor;
        };
        static void notifyOrderChanged();
        static std::vector<SwappableComponent*>& getSwappableComponents() {return swappableComponentList;}

    private:
        inline static std::vector<SwappableComponent*> swappableComponentList; //shared vector across all instances of swappable Components

        juce::ComponentDragger componentDragger; //adding this member allows us to drag the component using the mouseEvent that triggered it
        juce::Rectangle<int> oldBounds; //bounds of the component before it was dragged
        juce::Rectangle<int> newBounds; //bounds of the component while it is being dragged
        const int AREA_OVERLAP_THRESHOLD = 100;

        ProcessorType processorType;
        inline static BasicFXAudioProcessorEditor* editorInstance = nullptr;

};