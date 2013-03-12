/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_DA1A5994__
#define __MAINCOMPONENT_H_DA1A5994__

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
								public OpenGLRenderer
{
private:
	OpenGLContext openGLContext;
    Image logoImage, dynamicTextureImage;
    float rotation, textScrollPos;
    Draggable3DOrientation draggableOrientation;

    Slider speedSlider, sizeSlider;
    Label infoLabel;
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

	// Component
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
    void paint (Graphics&);
	void resized();
	// OpenGL
	void newOpenGLContextCreated();
	void openGLContextClosing();
	void renderOpenGL();
	void updateTextureImage();
	void drawBackground2DStuff();
	// Other
	double  getScale() const;
	int getContextWidth() const ;
    int getContextHeight() const;

	// Functions to create a couple of images to use as textures..
    static Image createLogoImage()
    {
        Image image (Image::ARGB, 256, 256, true, OpenGLImageType());

        if (image.isValid())
        {
            Graphics g (image);

            g.fillAll (Colours::lightgrey.withAlpha (0.8f));
            g.drawImageWithin (ImageFileFormat::loadFrom (BinaryData::juce_png, BinaryData::juce_pngSize),
                               0, 0, image.getWidth(), image.getHeight(), RectanglePlacement::stretchToFit);

            drawRandomStars (g, image.getWidth(), image.getHeight());
        }

        return image;
    }

	static void drawRandomStars (Graphics& g, int w, int h)
    {
        Random r;
        for (int i = 10; --i >= 0;)
        {
            Path pp;
            pp.addStar (Point<float> (r.nextFloat() * w, r.nextFloat() * h), r.nextInt (8) + 3, 10.0f, 20.0f, 0.0f);
            g.setColour (Colours::pink.withAlpha (0.4f));
            g.fillPath (pp);
        }
    }

	void drawScrollingMessage (Graphics& g, int y) const
    {
        g.drawSingleLineText ("The background, foreground and texture are all being drawn using the OpenGLGraphicsContext class, which "
                              "lets you use a standard JUCE 2D graphics context to render directly onto an OpenGL window or framebuffer...  ",
                              (int) -std::fmod (textScrollPos, 2500.0f), y);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_DA1A5994__
