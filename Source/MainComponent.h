/*                                                                                  
*=====================================================================================
*DetectO - Object Detection     													 |
*Detect Any object in video or Image                                                 |
*Email: cyber.cbm@gmail.com                                                          |
*Github: https://github.com/cyberCBM/DetectO.git                                     |
*License: GNU2 License, Copyright (c) 2012 by CyberCBM                               |
* DetectO can be redistributed and/or modified under the terms of the GNU General    |
* Public License (Version 2).                                                        |
*It use JUCE and OpenCV Libraries which holds GNU2                                   |
*A copy of the license is included in the DetectO distribution, or can be found      |
* online at www.gnu.org/licenses.                                                    |
*=====================================================================================
*/

#ifndef __MAINCOMPONENT__
#define __MAINCOMPONENT__

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

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

    // OpenCV declarations
    CvCapture *capture;
    IplImage  *frame;
    int       key ;
    char      *filename;
    // Some specific OpenCV
    CvHaarClassifierCascade *cascade;
    CvMemStorage            *storage;

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


#endif  // __MAINCOMPONENT__
