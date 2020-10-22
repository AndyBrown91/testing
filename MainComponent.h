#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "DemoUtilities.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class BallGeneratorComponent    : public juce::Component
{
public:
    BallGeneratorComponent() {}

    void paint (juce::Graphics& g) override
    {
        auto area = getLocalBounds().reduced (2);

        g.setColour (juce::Colours::orange);
        g.drawRoundedRectangle (area.toFloat(), 10.0f, 2.0f);

        g.setColour (findColour (juce::TextButton::textColourOffId));
        g.drawFittedText ("Drag Me!", area, juce::Justification::centred, 1);
    }

    void resized() override
    {
        // Just set the limits of our constrainer so that we don't drag ourselves off the screen
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(),
                                               getHeight(), getWidth());
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        // Prepares our dragger to drag this Component
        dragger.startDraggingComponent (this, e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        // Moves this Component according to the mouse drag event and applies our constraints to it
        dragger.dragComponent (this, e, &constrainer);
    }

private:
    juce::ComponentBoundsConstrainer constrainer;
    juce::ComponentDragger dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallGeneratorComponent)
};

//==============================================================================
struct BallComponent  : public juce::Component
{
    BallComponent (juce::Point<float> pos)
        : position (pos),
          speed (juce::Random::getSystemRandom().nextFloat() *  4.0f - 2.0f,
                 juce::Random::getSystemRandom().nextFloat() * -6.0f - 2.0f),
          colour (juce::Colours::white)
    {
        setSize (20, 20);
        step();
    }

    bool step()
    {
        position += speed;
        speed.y += 0.1f;

        setCentrePosition ((int) position.x,
                           (int) position.y);

        if (auto* parent = getParentComponent())
            return juce::isPositiveAndBelow (position.x, (float) parent->getWidth())
                && position.y < (float) parent->getHeight();

        return position.y < 400.0f && position.x >= -10.0f;
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (colour);
        g.fillEllipse (2.0f, 2.0f, (float) getWidth() - 4.0f, (float) getHeight() - 4.0f);

        g.setColour (juce::Colours::darkgrey);
        g.drawEllipse (2.0f, 2.0f, (float) getWidth() - 4.0f, (float) getHeight() - 4.0f, 1.0f);
    }

    juce::Point<float> position, speed;
    juce::Colour colour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallComponent)
};

//==============================================================================
class AnimationDemo  : public juce::Component,
                       private juce::Timer
{
public:
    AnimationDemo();

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    juce::OwnedArray<Component> componentsToAnimate;
    juce::OwnedArray<BallComponent> balls;
    BallGeneratorComponent ballGenerator;

    juce::ComponentAnimator animator;
    int cycleCount;

    bool firstCallback = true;

    juce::Button* createRandomButton();

    juce::Button* createButton();

    void triggerAnimation();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationDemo)
};
