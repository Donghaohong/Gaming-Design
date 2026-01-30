//
//  SLApp.h
//  Ship Lab
//
//  This is the root class for your game. You need to use CU_ROOTCLASS in your
//  .cpp file to register this as the root with CUGL. Otherwise, it will not
//  know where to start. All root classes are subclasses of Application.
//
//  With that said, root classes often do not do all that much stuff. They
//  often delegate their work to individual scenes and just handle the scene
//  switching.
//
//  Author: Walker White
//  Version: 1/20/26
//
#ifndef __SL_APP_H__
#define __SL_APP_H__
#include <cugl/cugl.h>
#include "SLGameScene.h"

/**
 * This class represents the application root for the ship demo.
 */
class ShipApp : public cugl::Application {
protected:
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;

    // Player modes
    /** The primary controller for the game world */
    GameScene _gameplay;
    /** The controller for the loading screen */
    cugl::scene2::LoadingScene _loading;

    /** Whether or not we have finished loading all assets */
    bool _loaded;
    
public:
    /**
     * Creates, but does not initialize, a new application.
     *
     * This constructor is where you set all your configuration values such
     * as the game name, the FPS, and so on. Many of these need to be set
     * before the backend is initialized.
     *
     * With that said, it is unsafe to do anything in this constuctor other than
     * initialize attributes. That is because this constructor is called before
     * the backend is initialized, and so much CUGL API calls will fail. Any
     * initialization that requires access to CUGL must happen in onStartup().
     */
    ShipApp();
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits. Its
     * simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~ShipApp() { }
    
#pragma mark Application State
    /**
     * The method called after the backend is initialized, but before running the application.
     *
     * This is the method in which all user-defined program intialization should
     * take place. You should not create a new init() method.
     *
     * When overriding this method, you should call the parent method as the
     * very last line.  This ensures that the state will transition to FOREGROUND,
     * causing the application to run.
     */
    virtual void onStartup() override;
    
    /**
     * The method called when the application is ready to quit.
     *
     * This is the method to dispose of all resources allocated by this
     * application.  As a rule of thumb, everything created in onStartup()
     * should be deleted here.
     *
     * When overriding this method, you should call the parent method as the
     * very last line. This ensures that the state will transition to NONE,
     * causing the application to be deleted.
     */
    virtual void onShutdown() override;
 
    /**
     * The method called when the application is suspended and put in the background.
     *
     * When this method is called, you should store any state that you do not
     * want to be lost. There is no guarantee that an application will return
     * from the background; it may be terminated instead.
     *
     * If you are using audio, it is critical that you pause it on suspension.
     * Otherwise, the audio thread may persist while the application is in
     * the background.
     */
    virtual void onSuspend() override;
    
    /**
     * The method called when the application resumes and put in the foreground.
     *
     * If you saved any state before going into the background, now is the time
     * to restore it. This guarantees that the application looks the same as
     * when it was suspended.
     *
     * If you are using audio, you should use this method to resume any audio
     * paused before app suspension.
     */
    virtual void onResume()  override;
    
#pragma mark Application Loop
    /**
     * The method called to update the application data.
     *
     * This is part of your core loop and should be replaced with your custom
     * implementation. This method should contain any code that is not a
     * graphics API call.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    virtual void update(float dt) override;
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is part of your core loop and should be replaced with your custom
     * implementation. This method should contain all drawing commands and
     * other uses of the graphics API.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void draw() override;
};

#endif /* __SL_APP_H__ */
