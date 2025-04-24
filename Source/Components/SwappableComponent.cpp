/*
  ==============================================================================

    SwappableComponent.cpp
    Created: 15 Apr 2025 8:30:22pm
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponent.h"
#include "../PluginEditor.h"


SwappableComponent::~SwappableComponent() {
    swappableComponentList.erase(
        std::remove(swappableComponentList.begin(), swappableComponentList.end(), this),
        swappableComponentList.end()
    );
}

void SwappableComponent::mouseDown(const juce::MouseEvent& e)
{   //called when the component is clicked on
    initialBounds = getBounds();
    componentDragger.startDraggingComponent(this, e);
}

void SwappableComponent::mouseDrag(const juce::MouseEvent& e)
{   //called while the component is being dragged
    componentDragger.dragComponent(this, e, nullptr);
    draggedBounds = getBounds();
}

void SwappableComponent::mouseUp(const juce::MouseEvent& e)  {
    //called while the component has stopped being dragged

    SwappableComponent* componentToSwap = nullptr;
    int largestIntersectionArea = 0;

    for (auto* comp : swappableComponentList) {
        if (comp != nullptr && comp != this) {
            //compare overlap in area of other components with this component
            auto intersection = draggedBounds.getIntersection(comp->getBounds());
            int area = intersection.getWidth() * intersection.getHeight();

            if (area > largestIntersectionArea && area > area_overlap_threshold) {
                largestIntersectionArea = area;
                componentToSwap = comp;
            }
        }
    }

    setBounds(initialBounds);
    if (componentToSwap != nullptr) {
        swapComponents(componentToSwap);
    }

}

void SwappableComponent::swapComponents(SwappableComponent* otherComp)
{

    auto& list = swappableComponentList;
    auto itA = std::find(list.begin(), list.end(), this);
    auto itB = std::find(list.begin(), list.end(), otherComp);
    //if iterators found both components before reaching the end
    if (itA != list.end() && itB != list.end()) {
        //swap the components in the list
        std::iter_swap(itA, itB);
        //swap the bounds
        auto otherCompBounds = getBounds();
        setBounds(otherComp->getBounds());
        otherComp->setBounds(otherCompBounds);

        sendActionMessage("SWAPPED_" + String(this->getIndexInComponentList()) + "_" +  String(otherComp->getIndexInComponentList()));
    }

}
int SwappableComponent::getIndexInComponentList()  {
    const auto& list = getSwappableComponents();
    for (int i = 0; i < list.size(); ++i)
        if (list[i] == this)
            return i;
    return -1;
}

void SwappableComponent::setBounds(juce::Rectangle<int> bounds)
{
    juce::Component::setBounds(bounds);
    setAreaOverLapThreshold();
}
void SwappableComponent::setBounds(int x, int y, int width, int height)
{
    juce::Component::setBounds(x, y, width, height);
    setAreaOverLapThreshold();
}
void SwappableComponent::setAreaOverLapThreshold() {
    area_overlap_threshold = (getLocalBounds().getWidth() * getLocalBounds().getHeight()) / 2;
}