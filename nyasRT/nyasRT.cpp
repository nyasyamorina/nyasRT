#include <iostream>
#include <memory>

#include "setup.h"
#include "nyasRT.h"

using namespace std;
using namespace sample_types;
using namespace generators;


int main()
{
#ifdef LOAD_SHOW_SAMPLE_FUNCTIONS
    auto args = SamplerArgs(83, 256, make_shared<CorrectMJ>());
    show_samples(Sampler(args, Square())).save_to(R"(E:\Codes\C--\nyasRT\outputs\square.png)");
    show_samples(Sampler(args, Circle())).save_to(R"(E:\Codes\C--\nyasRT\outputs\circle.png)");
    show_samples(Sampler(args, Sphere())).save_to(R"(E:\Codes\C--\nyasRT\outputs\sphere.png)");
    show_samples(Sampler(args, Hemisphere(10))).save_to(R"(E:\Codes\C--\nyasRT\outputs\hemisphere.png)");
#endif // LOAD_SHOW_SAMPLE_FUNCTIONS

    return 0;
}
