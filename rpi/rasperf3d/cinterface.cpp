#include <stdexcept>
#include <cstdint>

extern void InitModel();
extern void Render();
extern void SetDefaultSettings();
extern void SetFramebufferSize(uint32_t w, uint32_t h);
//extern void ApplyTextureFilter();

extern "C" {

    int rp_InitModel()
    {
        try {
            InitModel();
        }
        catch(const std::exception &e) {
            return -1; // TODO: actual error reporting
        }
    }

    int rp_Render()
    {
        try {
            Render();
        }
        catch(const std::exception &e) {
            return -1; // TODO: actual error reporting
        }
    }

    int rp_SetFramebufferSize(uint32_t w, uint32_t h)
    {
        try {
            SetFramebufferSize(w, h);
        }
        catch(const std::exception &e) {
            return -1; // TODO: actual error reporting
        }
    }

    int rp_SetDefaultSettings()
    {
        try {
            SetDefaultSettings();
        }
        catch(const std::exception &e) {
            return -1; // TODO: actual error reporting
        }
    }

} // extern "C"