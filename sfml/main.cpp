#include "App.h"

// ── Entry point ───────────────────────────────────────────────────────────────
// On Windows, WinMain is used (set via CMake WIN32 target property) so that
// no console window appears when the user double-clicks the exe.
int main() {
    App app;
    app.run();
    return 0;
}
