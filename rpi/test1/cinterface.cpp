#include <stdexcept>


extern void InitModel();

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

} // extern "C"