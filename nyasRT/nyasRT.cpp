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
    uint64_t n_sets = 83, n_samples = 256;
    Generatorp gen = make_shared<CorrectMJ>();

    show_samples(Sampler(Square(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\square.png)");
    show_samples(Sampler(Circle(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\circle.png)");
    show_samples(Sampler(Sphere(), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\sphere.png)");
    show_samples(Sampler(Hemisphere(10), n_sets, n_samples, gen)).save_to(R"(outputs\show_samples\hemisphere.png)");

    show_gen_perform(n_sets, 64, make_shared<Uniform>()).save_to(R"(outputs\show_gen_perform\uniform.png)");
    show_gen_perform(n_sets, 64, make_shared<Hammersley>()).save_to(R"(outputs\show_gen_perform\hammersley.png)");
    show_gen_perform(n_sets, 64, make_shared<Halton>()).save_to(R"(outputs\show_gen_perform\halton.png)");
    show_gen_perform(n_sets, 64, make_shared<Random>()).save_to(R"(outputs\show_gen_perform\random.png)");
    show_gen_perform(n_sets, 64, make_shared<Jittered>()).save_to(R"(outputs\show_gen_perform\jittered.png)");
    show_gen_perform(n_sets, 64, make_shared<NRooks>()).save_to(R"(outputs\show_gen_perform\nRooks.png)");
    show_gen_perform(n_sets, 64, make_shared<MultiJ>()).save_to(R"(outputs\show_gen_perform\multij.png)");
    show_gen_perform(n_sets, 64, make_shared<CorrectMJ>()).save_to(R"(outputs\show_gen_perform\correctmj.png)");
#endif // LOAD_SHOW_SAMPLE_FUNCTIONS

    return 0;
}
