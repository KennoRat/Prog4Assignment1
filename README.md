
# Ms. Pac-Man C++ Project & Custom Game Engine for Programming 4 DAE EXAM

A C++ recreation of Ms. Pac-Man using a custom-built, component-based 2D game engine.
[Link to Github project](https://github.com/KennoRat/Prog4_EngineExam_MissPacman.git)

## Engine Core & Design

*   **Component-Based Architecture:** Game logic and data are encapsulated in `Component`s (e.g., `RenderComponent`, `MovementComponent`, `ColliderComponent`) attached to `GameObject` entities.
*   **SDL2 & Extensions:** Uses SDL2 for windowing, input, and 2D rendering, SDL_image for textures, SDL_ttf for fonts, and SDL_mixer for audio.
*   **Service Locator:** Provides centralized access to global engine services like `InputManager` and `IAudioService`.
*   **Threading for Audio:** The `SDLMixerAudioService` processes sound loading and playback requests on a dedicated thread.
*   **PIMPL Idiom:** Applied to key engine classes (e.g., `ControllerInput`, `SDLMixerAudioService`) to hide third-party library dependencies (XInput, SDL_mixer) from public headers, reducing compile times and coupling.

## Key Game Programming & Design Patterns

*   **Game Loop & Update Method:**
    *   A standard game loop in `Minigin::Run` manages input, updates, and rendering.
    *   `GameObject`s delegate `Update()` calls to their `Component`s, where specific per-frame logic resides. Delta time is used for consistent behavior.
*   **Command Pattern:**
    *   Decouples input events from actions. `InputManager` binds keyboard/controller inputs to `Command` objects (e.g., `MoveCommand`, `UINavigateCommand`).
    *   States (like `MenuState`) dynamically bind and unbind UI-specific commands.
*   **Observer Pattern & Event System:**
    *   `Subject`s notify `Observer`s of `Event`s (identified by hashed IDs).
    *   Used for game events like Ms. Pac-Man's death/respawn (`LivesComponent` as Subject, `BaseGhostComponent` & `PlayingState` as Observers) and power-up activation (`PowerPelletComponent` as Subject, `BaseGhostComponent` as Observer).
*   **Component Pattern:** `GameObject`s are compositions of `BaseComponent` derivatives.
*   **State Pattern (Class-Based):**
    *   Manages overall game flow (e.g., `MenuState`, `PlayingState`, `PausedState`).
    *   `GameStateMachine` controls transitions between `GameState` implementations. States handle their own specific input, logic, UI, and active scene management. Transitions are signaled via a `StateTransition` struct.

## Game Specifics

*   **Level System:** Levels are loaded from CSV files via `LevelGridComponent`, which also tracks pellets.
*   **Ms. Pac-Man & Ghosts:** Player and enemy behaviors are implemented using specialized components (`PlayerMovementComponent`, `BaseGhostComponent`).
*   **Ghost AI:** Ghosts use a state machine (`GhostState`) and compositional `IGhostChaseBehaviour` strategies (e.g., `BlinkyChaseBehaviour`, `PinkyChaseBehaviour`) for unique targeting during their Chase state.
*   **Collision Detection:** A `CollisionManager` checks for overlaps between `ColliderComponent`s, triggering callbacks for game interactions like Ms. Pac-Man dying or eating a frightened ghost.

