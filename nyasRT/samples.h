#pragma once

#include <stdint.h>
#include "setup.h"
#include "utils.h"



#ifdef LOAD_SHOW_SAMPLE_FUNCTIONS

/* show sample functions */

template<typename ST> Image show_samples(Sampler<ST> const& sampler, uint64_t set = 0);
Image show_gen_perform(SamplerArgs const&);

#endif // LOAD_SHOW_SAMPLE_FUNCTIONS


/******************************************************************************
******************************  abstract Generator  ***************************
******************************************************************************/

class Generator {
public:
    virtual Buffer2D<Vec2> generate(uint64_t n_sets, uint64_t n_samples) const = 0;
};


/******************************************************************************
******************************  abstract SampleType  **************************
******************************************************************************/

template<typename V> class SampleType {
public:
    typedef V vtype;

    virtual vtype map_sample(Vec2 const& uniform_sample) const = 0;
};


/******************************************************************************
************************************  Sampler  ********************************
******************************************************************************/

struct SamplerArgs final {
public:
    uint64_t sets, samples_per_set;
    Generatorp generator_p;

    SamplerArgs();
    explicit SamplerArgs(uint64_t n_samples);
    explicit SamplerArgs(uint64_t n_samples, Generatorp gen);
    explicit SamplerArgs(uint64_t n_sets, uint64_t n_samples);
    explicit SamplerArgs(uint64_t n_sets, uint64_t n_samples, Generatorp gen);
};

template<typename ST> class Sampler final {
public:
    typedef ST stype;
    typedef ST::vtype vtype;

    explicit Sampler(SamplerArgs const&, ST const&);

    uint64_t sets() const;
    uint64_t samples_per_set() const;

    vtype const& next();
    vtype const& operator ()(uint64_t set, uint64_t idx) const;

private:
    Buffer2D<vtype> _samples;
    uint64_t _set, _idx;
};



/******************************************************************************
**********************************  generators  *******************************
******************************************************************************/

namespace generators
{
    /*********************************  Uniform  *****************************/

    class Uniform final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /*******************************  Hammersley  ****************************/

    class Hammersley final : public Generator {
    public:
        uint64_t p;

        Hammersley();
        explicit Hammersley(uint64_t);

        static double phi(uint64_t k, uint64_t p);

        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /*********************************  Halton  ******************************/

    class Halton final : public Generator {
    public:
        uint64_t p1, p2;

        Halton();
        explicit Halton(uint64_t, uint64_t);

        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /*********************************  Random  ******************************/

    class Random final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /********************************  Jittered  *****************************/

    class Jittered final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /*********************************  NRooks  ******************************/

    class NRooks final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /*********************************  MultiJ  ******************************/

    class MultiJ final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };


    /********************************  CorrectMJ  ****************************/

    class CorrectMJ final : public Generator {
    public:
        virtual Buffer2D<Vec2> generate(uint64_t, uint64_t) const override;
    };
}


/******************************************************************************
*********************************  sample types  ******************************
******************************************************************************/

namespace sample_types
{
    /*********************************  Square  ******************************/

    class Square final : public SampleType<Vec2> {
    public:
        virtual Vec2 map_sample(Vec2 const&) const override;
    };


    /*********************************  Circle  ******************************/

    class Circle final : public SampleType<Vec2> {
    public:
        virtual Vec2 map_sample(Vec2 const&) const override;
    };


    /*********************************  Sphere  ******************************/

    class Sphere final : public SampleType<Vec3> {
    public:
        virtual Vec3 map_sample(Vec2 const&) const override;
    };


    /*******************************  Hemisphere  ****************************/

    class Hemisphere final : public SampleType<Vec3> {
    public:
        double c;

        Hemisphere();
        explicit Hemisphere(double);

        virtual Vec3 map_sample(Vec2 const&) const override;
    };
}
