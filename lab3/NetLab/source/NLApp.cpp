//
//  SGApp.cpp
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
#include "NLApp.h"

using namespace cugl;
using namespace cugl::scene2;
using namespace cugl::graphics;

// The window size
#define GAME_WIDTH 1024
#define GAME_HEIGHT 576

CU_ROOTCLASS(NetApp)

#pragma mark -
#pragma mark Gameplay Loop
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
NetApp::NetApp() : cugl::Application(),
_scene(State::LOAD) {
    // Pre-launch configuration. Nothing here can be reassigned later.
    setName("NetLab");
    setOrganization("GDIAC");
    setAppId("edu.cornell.gdiac.net");
    
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
void NetApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input
#ifdef CU_MOBILE
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
#endif
    Input::activate<Keyboard>();
    Input::activate<TextInput>();

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->loadDirectory("json/loading.json");

    // Create a "loading" screen
    _scene = State::LOAD;
    _loading.init(_assets,"json/assets.json");
    _loading.setSpriteBatch(_batch);
    
    // Queue up the other assets
    _loading.start();
    netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
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
void NetApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _hostgame.dispose();
    _joingame.dispose();
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Input::deactivate<TextInput>();
    Input::deactivate<Keyboard>();
    netcode::NetworkLayer::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
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
void NetApp::update(float dt) {
    // Normally we would make things cleaner than all these if statements.
    // But the logic is 
    switch (_scene) {
        case LOAD:
            updateLoadingScene(dt);
            break;
        case MENU:
            updateMenuScene(dt);
            break;
        case HOST:
            updateHostScene(dt);
            break;
        case CLIENT:
            updateClientScene(dt);
            break;
        case GAME:
            updateGameScene(dt);
            break;
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom
 * implementation. This method should contain graphics API calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void NetApp::draw() {
    switch (_scene) {
        case LOAD:
            _loading.render();
            break;
        case MENU:
            _mainmenu.render();
            break;
        case HOST:
            _hostgame.render();
            break;
        case CLIENT:
            _joingame.render();
            break;
        case GAME:
            _gameplay.render();
            break;
    }
}

#pragma mark -
#pragma mark Scene Management

/**
 * Inidividualized update method for the loading scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the loading scene.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetApp::updateLoadingScene(float timestep) {
    if (_loading.isActive()) {
        _loading.update(timestep);
    } else {
        _loading.dispose(); // Permanently disables the input listeners in this mode
        _mainmenu.init(_assets);
        _mainmenu.setSpriteBatch(_batch);
        _hostgame.init(_assets);
        _hostgame.setSpriteBatch(_batch);
        _joingame.init(_assets);
        _joingame.setSpriteBatch(_batch);
        _gameplay.init(_assets);
        _gameplay.setSpriteBatch(_batch);

        _mainmenu.setActive(true);
        _scene = State::MENU;
    }
}


/**
 * Inidividualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetApp::updateMenuScene(float dt) {
    _mainmenu.update(dt);
    switch (_mainmenu.getChoice()) {
        case MenuScene::Choice::HOST:
            _mainmenu.setActive(false);
            _hostgame.setActive(true);
            _scene = State::HOST;
            break;
        case MenuScene::Choice::JOIN:
            _mainmenu.setActive(false);
            _joingame.setActive(true);
            _scene = State::CLIENT;
            break;
        case MenuScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetApp::updateHostScene(float dt) {
    _hostgame.update(dt);
    switch (_hostgame.getStatus()) {
        case HostScene::Status::ABORT:
            _hostgame.setActive(false);
            _mainmenu.setActive(true);
            _scene = State::MENU;
            break;
        case HostScene::Status::START:
            _hostgame.setActive(false);
            _gameplay.setActive(true);
            _scene = State::GAME;
            // Transfer connection ownership
            _gameplay.setConnection(_hostgame.getConnection());
            _hostgame.disconnect();
            _gameplay.setHost(true);
            break;
        case HostScene::Status::WAIT:
        case HostScene::Status::IDLE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the client scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetApp::updateClientScene(float dt) {
    _joingame.update(dt);
    switch (_joingame.getStatus()) {
        case ClientScene::Status::ABORT:
            _joingame.setActive(false);
            _mainmenu.setActive(true);
            _scene = State::MENU;
            break;
        case ClientScene::Status::START:
            _joingame.setActive(false);
            _gameplay.setActive(true);
            _scene = State::GAME;
            // Transfer connection ownership
            _gameplay.setConnection(_joingame.getConnection());
            _joingame.disconnect();
            _gameplay.setHost(false);
            break;
        case ClientScene::Status::WAIT:
        case ClientScene::Status::IDLE:
        case ClientScene::Status::JOIN:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the game scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the game scene.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void NetApp::updateGameScene(float dt) {
    _gameplay.update(dt);
    if (_gameplay.didQuit()) {
        _gameplay.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.disconnect();
        _scene = State::MENU;
    }
}


