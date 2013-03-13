/*                                                                                  
*=====================================================================================
*DetectO - Object Detection     													 |
*Detect Any object in video or Image                                                 |
*Email: cyber.cbm@gmail.com                                                          |
*Github: https://github.com/cyberCBM/DetectO.git                                     |
*License: GNU2 License, Copyright (c) 2012 by ScTeam                                 |
* ScPlayer can be redistributed and/or modified under the terms of the GNU General   |
* Public License (Version 2).                                                        |
*It use JUCE and OpenCV Libraries which holds GNU2                                   |
*A copy of the license is included in the DetectO distribution, or can be found     |
* online at www.gnu.org/licenses.                                                    |
*=====================================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : rotation (0.0f), frame(0), cascade(0), storage(0), capture(0),
    textScrollPos (200), key(0), filename("../../BinaryData/haarcascade_frontalface_alt.xml")
{
	/* load the classifier
       note that I put the file in the same directory with
       this code */
    cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
 
    /* setup memory buffer; needed by the face detector */
    storage = cvCreateMemStorage( 0 );

    openGLContext.setRenderer (this);
	openGLContext.setComponentPaintingEnabled (true);
	openGLContext.attachTo (*this);
	setSize (500, 400);
}

MainContentComponent::~MainContentComponent()
{
    openGLContext.detach();
    /* free memory */
    cvReleaseCapture( &capture );
    cvReleaseHaarClassifierCascade( &cascade );
    cvReleaseMemStorage( &storage );
}

// when the component creates a new internal context, this is called, and
// we'll use the opportunity to create some images to use as textures.
void MainContentComponent::newOpenGLContextCreated()
{
	logoImage = createLogoImage();
	dynamicTextureImage = Image (Image::ARGB, 128, 128, true, OpenGLImageType());
}

void MainContentComponent::openGLContextClosing()
{
	// We have to make sure we release any openGL images before the
	// GL context gets closed..
	logoImage = Image::null;
	dynamicTextureImage = Image::null;
}

void MainContentComponent::mouseDown (const MouseEvent& e)
{
	draggableOrientation.mouseDown (e.getPosition());
}

void MainContentComponent::mouseDrag (const MouseEvent& e)
{
	draggableOrientation.mouseDrag (e.getPosition());
	openGLContext.triggerRepaint();
}

void MainContentComponent::resized()
{
	draggableOrientation.setViewport (getLocalBounds());
}

void MainContentComponent::paint (Graphics&) 
{

}

void MainContentComponent::renderOpenGL()
{
	OpenGLHelpers::clear (Colours::darkgrey.withAlpha (1.0f));

	updateTextureImage();  // this will update our dynamically-changing texture image.
	drawBackground2DStuff(); // draws some 2D content to demonstrate the OpenGLGraphicsContext class

	// Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
	// we'll put back any important settings before doing our normal GL 3D drawing..
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_TEXTURE_2D);

#if JUCE_USE_OPENGL_FIXED_FUNCTION
	OpenGLHelpers::prepareFor2D (getContextWidth(), getContextHeight());
	OpenGLHelpers::setPerspective (45.0, getContextWidth() / (double) getContextHeight(), 0.1, 100.0);

	glTranslatef (0.0f, 0.0f, -5.0f);
	draggableOrientation.applyToOpenGLMatrix();

	// logoImage and dynamicTextureImage are actually OpenGL images, so we can use this utility function to
	// extract the frame buffer which is their backing store, and use it directly.
	OpenGLFrameBuffer* tex1 = OpenGLImageType::getFrameBufferFrom (logoImage);
	OpenGLFrameBuffer* tex2 = OpenGLImageType::getFrameBufferFrom (dynamicTextureImage);

	if (tex1 != nullptr && tex2 != nullptr)
	{
		// This draws the sides of our spinning cube.
		// I've used some of the juce helper functions, but you can also just use normal GL calls here too.
		tex1->draw3D (-1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, Colours::white);
		tex1->draw3D (-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, Colours::white);
		tex1->draw3D (-1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, Colours::white);
		tex2->draw3D (-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, Colours::white);
		tex2->draw3D ( 1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, Colours::white);
		tex2->draw3D (-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, Colours::white);
	}
#endif
}

void MainContentComponent::updateTextureImage()
{
	// This image is a special framebuffer-backed image, so when we draw to it, the context
	// will render directly into its framebuffer

	if (dynamicTextureImage.isValid())
	{
		dynamicTextureImage.clear (dynamicTextureImage.getBounds(),
			Colours::red.withRotatedHue (fabsf (::sinf (rotation / 300.0f))).withAlpha (0.7f));

		Graphics g (dynamicTextureImage);

		g.setFont (dynamicTextureImage.getHeight() / 3.0f);
		g.setColour (Colours::black);
		drawScrollingMessage (g, dynamicTextureImage.getHeight() / 2);
	}
}

void MainContentComponent::drawBackground2DStuff()
{
	// Create an OpenGLGraphicsContext that will draw into this GL window..
	ScopedPointer<LowLevelGraphicsContext> glRenderer (createOpenGLGraphicsContext (openGLContext,
		getContextWidth(),
		getContextHeight()));

	if (glRenderer != nullptr)
	{
		Graphics g (glRenderer);
		g.addTransform (AffineTransform::scale ((float) getScale()));

		// This stuff just creates a spinning star shape and fills it..
		Path p;
		const float scale = getHeight() * 0.4f;
		p.addStar (Point<float> (getWidth() * 0.7f, getHeight() * 0.4f), 7,
			scale * 0.6f, scale,
			rotation / 50.0f);

		g.setGradientFill (ColourGradient (Colours::green.withRotatedHue (fabsf (::sinf (rotation / 300.0f))),
			0, 0,
			Colours::green.withRotatedHue (fabsf (::cosf (rotation / -431.0f))),
			0, (float) getHeight(), false));
		g.fillPath (p);
	}
}

double MainContentComponent::getScale() const         
{ 
	return Desktop::getInstance().getDisplays().getDisplayContaining (getScreenBounds().getCentre()).scale; 
}
int MainContentComponent::getContextWidth() const     
{ 
	return roundToInt (getScale() * getWidth()); 
}
int MainContentComponent::getContextHeight() const    
{ 
	return roundToInt (getScale() * getHeight()); 
}