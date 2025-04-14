/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicFXAudioProcessorEditor::BasicFXAudioProcessorEditor (BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState & _apvts)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(_apvts)
{

    //initialize the gate's components
    gateButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, GATE_ON_STRING, gateButton);
    gateButton.setClickingTogglesState(true);
    gateMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, GATE_STATE_STRING, gateMenu);
    gateMenu.addItem(BASIC_GATE_STRING, 1); //TODO what is up with these IDs? setting ID to 0 causes a bug, but it doesnt make sense that the indexes im putting are 1-4 when the param returns 0-3
    gateMenu.addItem(RMS_GATE_STRING, 2);
    gateMenu.addItem(ATTACK_HOLD_RELEASE_GATE_STRING, 3);
    gateMenu.setSelectedId(1);
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, THRESHOLD_STRING, thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ATTACK_STRING, attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, RELEASE_STRING, releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, HOLD_STRING, holdSlider);

    //initialize the distortion components
    distortionButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, DISTORTION_ON_STRING, distortionButton);
    distortionButton.setClickingTogglesState(true);
    distortionMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, DISTORTION_STATE_STRING, distortionMenu);
    distortionMenu.addItem(WAVE_RECTIFIER_STRING, 1); //TODO what is up with these IDs? setting ID to 0 causes a bug, but it doesnt make sense that the indexes im putting are 1-4 when the param returns 0-3
    distortionMenu.addItem(BIT_CRUSHER_STRING, 2);
    distortionMenu.addItem(SOFT_CLIPPER_CUBIC_STRING, 3);
    distortionMenu.addItem(SOFT_CLIPPER_ARCTAN_STRING, 4);
    distortionMenu.addItem(SLEW_LIMITER_STRING, 5);
    distortionMenu.setSelectedId(1);
    distortionSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DISTORTION_STRING, distortionSlider);
    
    //initialize the flanger's components
    flangerDelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_DELAY_STRING, flangerDelaySlider);
    flangerMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_MIX_STRING, flangerMixSlider);
    flangerButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, FLANGER_ON_STRING, flangerButton);
    flangerButton.setClickingTogglesState(true);

    for (auto* comp : getVisibleComps())
    {
        addAndMakeVisible(comp);
    }
    setSize (1200, 800);
}

BasicFXAudioProcessorEditor::~BasicFXAudioProcessorEditor()
{
}

//==============================================================================
void BasicFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void BasicFXAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto pluginBounds = getLocalBounds();
    auto gateBounds = pluginBounds.withTrimmedRight(pluginBounds.getWidth() * 0.75);
    auto distortionBounds = pluginBounds.withTrimmedLeft(pluginBounds.getWidth() * 0.25).withTrimmedRight(pluginBounds.getWidth() * 0.5);
    auto flangerBounds = pluginBounds.withTrimmedRight(pluginBounds.getWidth() * 0.25).withTrimmedLeft(pluginBounds.getWidth() * 0.5);
    auto bounds4 = pluginBounds.withTrimmedLeft(pluginBounds.getWidth() * 0.75);

    auto gateBoundsBottom = gateBounds.withTrimmedTop(gateBounds.getHeight() * 0.6);
    thresholdSlider.setBounds(gateBoundsBottom.withTrimmedBottom(gateBoundsBottom.getHeight() * 0.2));
    gateButton.setBounds(gateBoundsBottom.withTrimmedTop(gateBoundsBottom.getHeight() * 0.8));
    auto gateBoundsTop = gateBounds.withTrimmedBottom(gateBounds.getHeight() * 0.4);
    gateMenu.setBounds(gateBoundsTop.withTrimmedTop(gateBoundsTop.getHeight() * 0.9));
    attackSlider.setBounds(gateBoundsTop.withTrimmedRight(gateBoundsTop.getWidth() * 0.66).withTrimmedBottom(gateBoundsTop.getHeight() * 0.1));
    holdSlider.setBounds(gateBoundsTop.withTrimmedLeft(gateBoundsTop.getWidth() * 0.33).withTrimmedRight(gateBoundsTop.getWidth() * 0.33).withTrimmedBottom(gateBoundsTop.getHeight() * 0.1));
    releaseSlider.setBounds(gateBoundsTop.withTrimmedLeft(gateBoundsTop.getWidth() * 0.66).withTrimmedBottom(gateBoundsTop.getHeight() * 0.1));


    auto distortionBoundsBottom = distortionBounds.withTrimmedTop(distortionBounds.getHeight() * 0.6);
    distortionSlider.setBounds(distortionBoundsBottom.withTrimmedBottom(distortionBoundsBottom.getHeight() * 0.2));
    distortionButton.setBounds(distortionBoundsBottom.withTrimmedTop(distortionBoundsBottom.getHeight() * 0.8));
    auto distortionBoundsTop = distortionBounds.withTrimmedBottom(distortionBounds.getHeight() * 0.4);
    distortionMenu.setBounds(distortionBoundsTop.withTrimmedTop(distortionBoundsTop.getHeight() * 0.9));

    auto flangerBoundsBottom = flangerBounds.withTrimmedTop(flangerBounds.getHeight() * 0.6);
    flangerDelaySlider.setBounds(flangerBoundsBottom.withTrimmedBottom(flangerBoundsBottom.getHeight() * 0.2));
    flangerButton.setBounds(flangerBoundsBottom.withTrimmedTop(flangerBoundsBottom.getHeight() * 0.8));
    auto flangerBoundsTop = flangerBounds.withTrimmedBottom(flangerBounds.getHeight() * 0.4);
    flangerMixSlider.setBounds(flangerBoundsTop.withTrimmedRight(flangerBoundsTop.getWidth() * 0.66).withTrimmedBottom(flangerBoundsTop.getHeight() * 0.1));
}

std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&thresholdSlider);
    comps.push_back(&gateMenu);
    comps.push_back(&gateButton);
    comps.push_back(&attackSlider);
    comps.push_back(&releaseSlider);
    comps.push_back(&holdSlider);
    comps.push_back(&distortionButton);
    comps.push_back(&distortionMenu);
    comps.push_back(&distortionSlider);
    comps.push_back(&flangerDelaySlider);
    comps.push_back(&flangerMixSlider);
    comps.push_back(&flangerButton);
    return comps;
}