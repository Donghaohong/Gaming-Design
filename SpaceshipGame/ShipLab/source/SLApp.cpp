//
//  SLApp.cpp
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
#include "SLApp.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace cugl::scene2;

/** This is the main application and so we need this macro at the start */
CU_ROOTCLASS(ShipApp)

// 16x9,  Android phones, PC Gaming
#define GAME_WIDTH  1280
#define GAME_HEIGHT 720


#pragma mark -
#pragma mark Gameplay Control
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
ShipApp::ShipApp() : Application(), _loaded(false) {
    // Pre-launch configuration. Nothing here can be reassigned later.
    setName("Ship Lab");
    setOrganization("GDIAC");
    setAppId("edu.cornell.gdiac.code1");

    setHighDPI(true);
    setResizable(false);
    setVSync(true);

    // This one can MAYBE reassigned after launch
    setDisplaySize(GAME_WIDTH, GAME_HEIGHT);
    
    // This one can be reset at any time
    setFPS(120.0f);
}

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
void ShipApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input (DESKTOP ONLY)
    Input::activate<Mouse>();
    Input::activate<Keyboard>();

    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    
    // Needed for loading screen
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->loadDirectory("json/loading.json");

    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets,"json/assets.json");
    _loading.setSpriteBatch(_batch);
    
    // Queue up the other assets
    _loading.start();

    AudioEngine::start();
    Application::onStartup(); // YOU MUST END with call to parent
}

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
void ShipApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
    Input::deactivate<Keyboard>();
    Input::deactivate<Mouse>();

    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

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
void ShipApp::onSuspend() {
    AudioEngine::get()->pause();
}

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
void ShipApp::onResume() {
    AudioEngine::get()->resume();
}

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
void ShipApp::update(float dt) {
    if (!_loaded && _loading.isActive()) {
        _loading.update(dt);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode
        _gameplay.init(_assets);
        _gameplay.setSpriteBatch(_batch);
        _loaded = true;
    } else {
        _gameplay.update(dt);
    }
}

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
void ShipApp::draw() {
    if (!_loaded) {
        _loading.render();
    } else {
        _gameplay.render();
    }
}


