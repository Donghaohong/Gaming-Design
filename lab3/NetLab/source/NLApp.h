//
//  NLApp.cpp
//  Network Lab
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
#ifndef __NL_APP_H__
#define __NL_APP_H__
#include <cugl/cugl.h>
#include "NLMenuScene.h"
#include "NLHostScene.h"
#include "NLClientScene.h"
#include "NLGameScene.h"

/**
 * This class represents the application root for the network demo.
 */
class NetApp : public cugl::Application {
protected:
    /**
     * The current active scene
     */
    enum State {
        /** The loading scene */
        LOAD,
        /** The main menu scene */
        MENU,
        /** The scene to host a game */
        HOST,
        /** The scene to join a game */
        CLIENT,
        /** The scene to play the game */
        GAME
    };
    
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::graphics::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network interface */
    std::shared_ptr<cugl::netcode::NetcodeConnection> _network;

    /** The controller for the loading screen */
    cugl::scene2::LoadingScene _loading;
    /** The menu scene to chose what to do */
    MenuScene _mainmenu;
    /** The scene to host a game */
    HostScene _hostgame;
    /** The scene to join a game */
    ClientScene _joingame;
    /** The primary controller for the game world */
    GameScene _gameplay;

    /** The current active scene */
    State _scene;
    
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
    NetApp();
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits. Its
     * simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~NetApp() { }
    
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
     * This is your core loop and should be replaced with your custom
     * implementation. This method should contain graphics API calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void draw() override;

private:
    /**
     * Inidividualized update method for the loading scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void updateLoadingScene(float dt);

    /**
     * Inidividualized update method for the menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the menu scene.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void updateMenuScene(float dt);

    /**
     * Inidividualized update method for the host scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the host scene.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void updateHostScene(float dt);
    
    /**
     * Inidividualized update method for the client scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the client scene.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void updateClientScene(float dt);

    /**
     * Inidividualized update method for the game scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the game scene.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void updateGameScene(float dt);
};

#endif /* __NL_APP_H__ */
