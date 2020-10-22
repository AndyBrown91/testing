#include "MainComponent.h"

AnimationDemo::AnimationDemo()
{
    setOpaque (true);

    for (auto i = 0; i < 11; ++i)
    {
        auto* b = createButton();
        componentsToAnimate.add (b);
        addAndMakeVisible (b);
        b->onClick = [this] { triggerAnimation(); };
    }

    addAndMakeVisible (ballGenerator);

    cycleCount = 2;
    startTimerHz (60);

    setSize (620, 620);
}

void AnimationDemo::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));
}

void AnimationDemo::resized()
{
    ballGenerator.centreWithSize (80, 50);
    triggerAnimation();
}

juce::Button* AnimationDemo::createRandomButton()
{
    juce::DrawablePath normal, over;

    juce::Path star1;
    star1.addStar ({}, 5, 20.0f, 50.0f, 0.2f);
    normal.setPath (star1);
    normal.setFill (juce::Colours::red);

    juce::Path star2;
    star2.addStar ({}, 7, 30.0f, 50.0f, 0.0f);
    over.setPath (star2);
    over.setFill (juce::Colours::pink);
    over.setStrokeFill (juce::Colours::black);
    over.setStrokeThickness (5.0f);

    auto juceIcon = getImageFromAssets ("juce_icon.png");

    juce::DrawableImage down;
    down.setImage (juceIcon);
    down.setOverlayColour (juce::Colours::black.withAlpha (0.3f));

    if (juce::Random::getSystemRandom().nextInt (10) > 2)
    {
        auto type = juce::Random::getSystemRandom().nextInt (3);

        auto* d = new juce::DrawableButton ("Button",
                                      type == 0 ? juce::DrawableButton::ImageOnButtonBackground
                                                : (type == 1 ? juce::DrawableButton::ImageFitted
                                                             : juce::DrawableButton::ImageAboveTextLabel));
        d->setImages (&normal,
                      juce::Random::getSystemRandom().nextBool() ? &over : nullptr,
                      juce::Random::getSystemRandom().nextBool() ? &down : nullptr);

        if (juce::Random::getSystemRandom().nextBool())
        {
            d->setColour (juce::DrawableButton::backgroundColourId,   getRandomBrightColour());
            d->setColour (juce::DrawableButton::backgroundOnColourId, getRandomBrightColour());
        }

        d->setClickingTogglesState (juce::Random::getSystemRandom().nextBool());
        return d;
    }

    auto* b = new juce::ImageButton ("ImageButton");

    b->setImages (true, true, true,
                  juceIcon, 0.7f, juce::Colours::transparentBlack,
                  juceIcon, 1.0f, getRandomDarkColour()  .withAlpha (0.2f),
                  juceIcon, 1.0f, getRandomBrightColour().withAlpha (0.8f),
                  0.5f);
    return b;
}

juce::Button* AnimationDemo::createButton()
{
    auto juceIcon = getImageFromAssets ("juce_icon.png").rescaled (128, 128);

    auto* b = new juce::ImageButton ("ImageButton");

    b->setImages (true, true, true,
                  juceIcon, 1.0f, juce::Colours::transparentBlack,
                  juceIcon, 1.0f, juce::Colours::white,
                  juceIcon, 1.0f, juce::Colours::white,
                  0.5f);

    return b;
}

void AnimationDemo::triggerAnimation()
{
    auto width = getWidth();
    auto height = getHeight();

    bool useWidth = (height > width);

    for (auto* component : componentsToAnimate)
    {
        auto newIndex = (componentsToAnimate.indexOf (component) + 3 * cycleCount)
                         % componentsToAnimate.size();

        auto angle = (float) newIndex * juce::MathConstants<float>::twoPi / (float) componentsToAnimate.size();

        auto radius = useWidth ? (float) width  * 0.35f
                               : (float) height * 0.35f;

        juce::Rectangle<int> r (getWidth()  / 2 + (int) (radius * std::sin (angle)) - 50,
                          getHeight() / 2 + (int) (radius * std::cos (angle)) - 50,
                          100, 100);

        animator.animateComponent (component, r.reduced (10), 1.0f,
                                   900 + (int) (300 * std::sin (angle)),
                                   false, 0.0, 0.0);
    }

    ++cycleCount;
}

void AnimationDemo::timerCallback()
{
    if (firstCallback)
    {
        triggerAnimation();
        firstCallback = false;
    }

    // Go through each of our balls and update their position
    for (int i = balls.size(); --i >= 0;)
        if (! balls.getUnchecked (i)->step())
            balls.remove (i);

    // Randomly generate new balls
    if (juce::Random::getSystemRandom().nextInt (100) < 4)
        addAndMakeVisible (balls.add (new BallComponent (ballGenerator.getBounds().getCentre().toFloat())));
}