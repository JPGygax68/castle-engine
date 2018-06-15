#include <stdexcept>


extern void InitModel();
extern void Render();
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

} // extern "C"