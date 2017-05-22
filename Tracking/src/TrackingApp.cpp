#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"

#include "CinderOpenCv.h"
#include <opencv2/tracking.hpp>

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TrackingApp : public App {
public:
    void setup() override;
    void cleanup() override;
    void mouseDown(MouseEvent event) override;
    void mouseDrag( MouseEvent event) override;
    void mouseUp(MouseEvent event) override;
    void update() override;
    void draw() override;
    
private:
    CaptureRef capture;
    gl::TextureRef texture;
    cv::Ptr<cv::Tracker> tracker;
    Rectf rect;
    cv::Rect2d trackerRect;
    bool isSelecting;
};

void TrackingApp::setup()
{
    setWindowSize(1280, 720);
    
    // show cv build info
//    console() << "cv info:" << cv::getBuildInformation() << endl;
    
    tracker = cv::Tracker::create("KCF");
    
    console() << "tracker: " << tracker->getDefaultName() << endl;
    
    // print all devices
    const auto devices = Capture::getDevices();
    for( const auto &device : devices ) {
        console() << "Device: " << device->getName() << " Available" << device->checkAvailable() << endl;
    }
    try {
        const auto device = devices.back();
        capture = Capture::create( 1280, 720, device);
        capture->start();
    }
    catch( ci::Exception &exc ) {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc );
    }
}

void TrackingApp::cleanup()
{
    if(capture) {
        capture->stop();
    }
}

void TrackingApp::mouseDown(MouseEvent event)
{
    isSelecting = true;
    rect.x1 = event.getX();
    rect.y1 = event.getY();
}

void TrackingApp::mouseDrag(MouseEvent event)
{
    rect.x2 = event.getX();
    rect.y2 = event.getY();
}

void TrackingApp::mouseUp(MouseEvent event)
{
    isSelecting = false;
    rect.x2 = event.getX();
    rect.y2 = event.getY();

    // init tracker
    trackerRect = toOcv(rect.getInteriorArea());
    cv::Mat input( toOcv( *capture->getSurface() ) ), output;
    cv::cvtColor(input, output, CV_BGRA2BGR);
    tracker = cv::Tracker::create("KCF");
    tracker->init(output, trackerRect);
    
}

void TrackingApp::update()
{
    if( capture && capture->checkNewFrame() ) {
        cv::Mat input( toOcv( *capture->getSurface() ) ), output;
        cv::cvtColor(input, output, CV_BGRA2BGR);
        texture = gl::Texture::create(fromOcv(output), gl::Texture::Format().loadTopDown());
        
        if(!isSelecting) {
            tracker->update(output, trackerRect);
            rect = fromOcv(trackerRect);
        }
        
    }
}

void TrackingApp::draw()
{
    gl::clear();
    
    if(!texture) {
        return;
    }
    
    gl::color(Color(1, 1, 1));
    gl::draw(texture);
    
    if(isSelecting) {
        gl::color(Color( 1.0f, 0.5f, 0.25f ));
        gl::drawStrokedRect(rect, 3);
    }
    else {
        gl::color(Color( 0.0f, 1.0f, 0.5f ));
        gl::drawStrokedRect(rect, 3);
    }
}

CINDER_APP( TrackingApp, RendererGl )
