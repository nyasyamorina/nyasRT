#include "samples.h"

#include "setup.h"
#include <functional>
#include <math.h>

using namespace std;
using namespace sample_types;
using namespace generators;

constexpr double TwoPi = 2.0 * 3.141592653589793238462643383279502884;
constexpr double PiOverFour = 0.25 * 3.141592653589793238462643383279502884;


#ifdef LOAD_SHOW_SAMPLE_FUNCTIONS

/* show sample functions */

template<> Image show_samples(Sampler<Square> const& sampler, uint64_t set) {
    auto im = Image(512, 512);
    im.clear();
    auto samples = sampler.samples_per_set();
    for (uint64_t idx = 0; idx < samples; ++idx) {
        auto & samp = sampler(set, idx);
        auto x = (uint64_t) (511.0 * samp.x);
        auto y = 511 - (uint64_t) (511.0 * samp.y);
        if (im.inbounds(y, x)) {
            im(y, x) = RGB(1.0f);
        }
    }
    return im;
}
template<> Image show_samples(Sampler<Circle> const& sampler, uint64_t set) {
    auto im = Image(512, 512);
    im.clear();
    for (uint64_t y = 0; y < 512; ++y) {
        auto y_ = y - 255.5;
        y_ *= y_;
        for (uint64_t x = 0; x < 512; ++x) {
            auto x_ = x - 255.5;
            if (x_ * x_ + y_ > 65536.0) {
                im(y, x) = RGB(0.0f, 0.0f, 1.0f);
            }
        }
    }
    auto samples = sampler.samples_per_set();
    for (uint64_t idx = 0; idx < samples; ++idx) {
        auto & samp = sampler(set, idx);
        auto x = (uint64_t) (255.5 * (1.0 + samp.x) + 0.5);
        auto y = (uint64_t) (255.5 * (1.0 - samp.y) + 0.5);
        if (im.inbounds(y, x)) {
            im(y, x) = RGB(1.0f);
        }
    }
    return im;
}
template<> Image show_samples(Sampler<Sphere> const& sampler, uint64_t set) {
    auto im = Image(512, 1024);
    im.clear();
    for (uint64_t y = 0; y < 512; ++y) {
        auto y_ = y - 255.5;
        y_ *= y_;
        for (uint64_t x = 0; x < 512; ++x) {
            auto x_ = x - 255.5;
            if (x_ * x_ + y_ > 65536.0) {
                im(y, x) = im(y, x + 512) = RGB(0.0f, 0.0f, 1.0f);
            }
        }
    }
    auto samples = sampler.samples_per_set();
    for (uint64_t idx = 0; idx < samples; ++idx) {
        auto & samp = sampler(set, idx);
        auto x = (uint64_t) (255.5 * (1.0 + samp.x) + 0.5);
        auto y = (uint64_t) (255.5 * (1.0 - samp.y) + 0.5);
        auto z = (uint64_t) (255.5 * (1.0 - samp.z) + 0.5);
        if (im.inbounds(y, x)) {
            im(y, x + 512) = RGB(1.0f);
        }
        if (im.inbounds(z, x)) {
            im(z, x) = RGB(1.0f);
        }
    }
    return im;
}
template<> Image show_samples(Sampler<Hemisphere> const& sampler, uint64_t set) {
    auto im = Image(512, 1024);
    im.clear();
    for (uint64_t y = 0; y < 512; ++y) {
        auto y_0 = y - 255.5;
        auto y_1 = y - 383.5;
        y_0 *= y_0;
        y_1 *= y_1;
        for (uint64_t x = 0; x < 512; ++x) {
            auto x_ = x - 255.5;
            x_ *= x_;
            if (x_ + y_0 > 65536.0) {
                im(y, x + 512) = RGB(0.0f, 0.0f, 1.0f);
            }
            if (y > 383 || x_ + y_1 > 65536.0) {
                im(y, x) = RGB(0.0f, 0.0f, 1.0f);
            }
        }
    }
    auto samples = sampler.samples_per_set();
    for (uint64_t idx = 0; idx < samples; ++idx) {
        auto & samp = sampler(set, idx);
        auto x = (uint64_t) (255.5 * (1.0 + samp.x) + 0.5);
        auto y = (uint64_t) (255.5 * (1.0 - samp.y) + 0.5);
        auto z = 383 - (uint64_t) (255.5 * samp.z + 0.5);
        if (im.inbounds(y, x)) {
            im(y, x + 512) = RGB(1.0f);
        }
        if (im.inbounds(z, x)) {
            im(z, x) = RGB(1.0f);
        }
    }
    return im;
}

Image show_gen_perform(SamplerArgs const& args) {
    constexpr double scaler = 10.37 / 512;
    auto im = Image(512, 512);
    auto sampler = Sampler(args, sample_types::Square());
    auto n_samples = sampler.samples_per_set();
    for (uint64_t h = 0; h < 512; ++h) {
        for (uint64_t w = 0; w < 512; ++w) {
            auto color = 0.0f;
            for (uint64_t idx = 0; idx < n_samples; ++idx) {
                auto const& samp = sampler.next();
                auto x = (w + samp.x) * scaler;
                auto y = (512 - h - samp.y) * scaler;
                color += (float)sin(x * x * y * y);
            }
            im(h, w) = RGB(0.5f * (color / (float)n_samples + 1.0f));
        }
    }
    return im;
}

#endif // LOAD_SHOW_SAMPLE_FUNCTIONS


/******************************************************************************
************************************  Sampler  ********************************
******************************************************************************/

SamplerArgs::SamplerArgs()
    : samples_per_set(0), sets(0), generator_p(nullptr) {}
SamplerArgs::SamplerArgs(uint64_t n_samples)
    : SamplerArgs(1, n_samples, make_shared<MultiJ>()) {}
SamplerArgs::SamplerArgs(uint64_t n_samples, Generatorp gen)
    : SamplerArgs(1, n_samples, gen) {}
SamplerArgs::SamplerArgs(uint64_t n_samples, uint64_t n_sets)
    : SamplerArgs(n_samples, n_sets, make_shared<MultiJ>()) {}
SamplerArgs::SamplerArgs(uint64_t n_samples, uint64_t n_sets, Generatorp gen)
    : samples_per_set(n_samples), sets(n_sets), generator_p(gen) {}


template class Sampler<Square>;
template class Sampler<Circle>;
template class Sampler<Sphere>;
template class Sampler<Hemisphere>;

template<typename ST> Sampler<ST>::Sampler(SamplerArgs const& args, ST const& type)
    : _samples(), _set(0), _idx(0) {
    if (args.sets > 0 || args.samples_per_set > 0 || args.generator_p != nullptr) {
        auto unif_samples = args.generator_p->generate(args.samples_per_set, args.sets);
        auto n_sets = unif_samples.get_rows(), n_samples = unif_samples.get_cols();
        this->_samples = Buffer2D<vtype>(n_sets, n_samples);

        vtype ** this_rows = this->_samples.data_ptr();
        auto unif_rows = unif_samples.data_ptr();
        for (uint64_t set = 0; set < n_sets; ++set) {
            vtype * this_samples = *this_rows;
            auto unif_samples = *(unif_rows++);
            for (uint64_t idx = 0; idx < n_samples; ++idx) {
                *(this_samples++) = type.map_sample(*(unif_samples++));
            }
            this_samples = *(this_rows++);
            for (uint64_t idx = 0; idx < n_samples; ++idx) {
                uint64_t tar = randint(n_samples);
                swap(this_samples[idx], this_samples[tar]);
            }
        }
    }
}

template<typename ST> uint64_t Sampler<ST>::sets() const {
    return this->_samples.get_rows();
}
template<typename ST> uint64_t Sampler<ST>::samples_per_set() const {
    return this->_samples.get_cols();
}

template<typename ST> Sampler<ST>::vtype const& Sampler<ST>::next() {
    auto set = this->_set, idx = this->_idx++;
    if (this->_idx >= this->samples_per_set()) {
        this->_idx = 0;
        //this->_set = (++this->_set) % this->_samples.get_rows();
        this->_set = randint(this->sets());
    }
    return this->_samples(set, idx);
}
template<typename ST> Sampler<ST>::vtype const& Sampler<ST>::operator ()(uint64_t set, uint64_t idx) const {
    return this->_samples(set, idx);
}


/******************************************************************************
**********************************  generators  *******************************
******************************************************************************/

using namespace generators;

/***********************************  Uniform  *******************************/

Buffer2D<Vec2> Uniform::generate(uint64_t, uint64_t n_samples) const {
    auto n_sides = (uint64_t) sqrt(n_samples);
    auto samples = Buffer2D<Vec2>(1, n_sides * n_sides);
    auto inv_side = 1.0 / n_sides;
    auto samples_p = *samples.data_ptr();
    for (uint64_t y = 0; y < n_sides; ++y) {
        for (uint64_t x = 0; x < n_sides; ++x) {
            *(samples_p++) = Vec2((x + 0.5) * inv_side, (y + 0.5) * inv_side);
        }
    }
    return samples;
}

/*********************************  Hammersley  ******************************/

Hammersley::Hammersley()
    : p(2) {}
Hammersley::Hammersley(uint64_t p_)
    : p(p_) {}
double Hammersley::phi(uint64_t k, uint64_t p) {
    auto res = 0.0;
    auto inv_p = 1.0 / p;
    auto tmp = inv_p;
    while (k > 0) {
        auto quot_rem = lldiv(k, p);
        k = (uint64_t) quot_rem.quot;
        res += quot_rem.rem * tmp;
        tmp *= inv_p;
    }
    return res;
}
Buffer2D<Vec2> Hammersley::generate(uint64_t, uint64_t n_samples) const {
    auto samples = Buffer2D<Vec2>(1, n_samples);
    auto inv_samples = 1.0 / n_samples;
    auto samples_p = *samples.data_ptr();
    for (uint64_t idx = 0; idx < n_samples; ++idx) {
        *(samples_p++) = Vec2(idx * inv_samples, Hammersley::phi(idx, this->p));
    }
    return samples;
}

/***********************************  Halton  ********************************/

Halton::Halton()
    : p1(2), p2(3) {}
Halton::Halton(uint64_t p1_, uint64_t p2_)
    : p1(p1_), p2(p2_) {}
Buffer2D<Vec2> Halton::generate(uint64_t, uint64_t n_samples) const {
    auto samples = Buffer2D<Vec2>(1, n_samples);
    auto samples_p = *samples.data_ptr();
    for (uint64_t idx = 0; idx < n_samples; ++idx) {
        *(samples_p++) = Vec2(Hammersley::phi(idx, this->p1), Hammersley::phi(idx, this->p2));
    }
    return samples;
}

/***********************************  Random  ********************************/

Buffer2D<Vec2> Random::generate(uint64_t n_sets, uint64_t n_samples) const {
    auto samples = Buffer2D<Vec2>(n_sets, n_samples);
    auto rows = samples.data_ptr();
    for (uint64_t set = 0; set < n_sets; ++set) {
        auto samples_p = *(rows++);
        for (uint64_t idx = 0; idx < n_samples; ++idx) {
            *(samples_p++) = Vec2(rand01(), rand01());
        }
    }
    return samples;
}

/**********************************  Jittered  *******************************/

Buffer2D<Vec2> Jittered::generate(uint64_t n_sets, uint64_t n_samples) const {
    auto n_sides = (uint64_t) sqrt(n_samples);
    auto samples = Buffer2D<Vec2>(n_sets, n_sides * n_sides);
    auto inv_side = 1.0 / n_sides;
    auto rows = samples.data_ptr();
    for (uint64_t set = 0; set < n_sets; ++set) {
        auto samples_p = *(rows++);
        for (uint64_t y = 0; y < n_sides; ++y) {
            for (uint64_t x = 0; x < n_sides; ++x) {
                *(samples_p++) = Vec2((x + rand01()) * inv_side, (y + rand01()) * inv_side);
            }
        }
    }
    return samples;
}

/***********************************  NRooks  ********************************/

Buffer2D<Vec2> NRooks::generate(uint64_t n_sets, uint64_t n_samples) const {
    auto samples = Buffer2D<Vec2>(n_sets, n_samples);
    auto inv_sample = 1.0 / n_samples;
    auto rows = samples.data_ptr();
    for (uint64_t set = 0; set < n_sets; ++set) {
        auto samples_p = *rows;
        for (uint64_t idx = 0; idx < n_samples; ++idx) {
            *(samples_p++) = Vec2((idx + rand01()) * inv_sample, (idx + rand01()) * inv_sample);
        }
        samples_p = *(rows++);
        for (uint64_t idx = 0; idx < n_samples; ++idx) {
            auto tar = randint(n_samples);
            swap(samples_p[idx].x, samples_p[tar].x);
        }
    }
    return samples;
}

/***********************************  MultiJ  ********************************/

Buffer2D<Vec2> MultiJ::generate(uint64_t n_sets, uint64_t n_samples) const {
    auto n_sides = (uint64_t) sqrt(n_samples);
    auto samples = Buffer2D<Vec2>(n_sets, n_sides * n_sides);
    auto inv_side = 1.0 / n_sides;
    auto rows = samples.data_ptr();
    for (uint64_t set = 0; set < n_sets; ++set) {
        auto samples_p = *rows;
        for (uint64_t y = 0; y < n_sides; ++y) {
            for (uint64_t x = 0; x < n_sides; ++x) {
                *(samples_p++) = Vec2((x + (y + rand01()) * inv_side) * inv_side,
                                      (y + (x + rand01()) * inv_side) * inv_side);
            }
        }
        samples_p = *(rows++);
        for (uint64_t y = 0; y < n_sides; ++y) {
            for (uint64_t x = 0; x < n_sides; ++x) {
                auto z = randint(n_sides - y);
                swap(samples_p[y * n_sides + x].x, samples_p[z * n_sides + x].x);
            }
        }
        for (uint64_t x = 0; x < n_sides; ++x) {
            for (uint64_t y = 0; y < n_sides; ++y) {
                auto z = randint(n_sides - x);
                swap(samples_p[y * n_sides + x].y, samples_p[y * n_sides + z].y);
            }
        }
    }
    return samples;
}

/**********************************  CorrectMJ  ******************************/

Buffer2D<Vec2> CorrectMJ::generate(uint64_t n_sets, uint64_t n_samples) const {
    auto n_sides = (uint64_t) sqrt(n_samples);
    auto samples = Buffer2D<Vec2>(n_sets, n_sides * n_sides);
    auto inv_side = 1.0 / n_sides;
    auto rows = samples.data_ptr();
    for (uint64_t set = 0; set < n_sets; ++set) {
        auto samples_p = *rows;
        for (uint64_t y = 0; y < n_sides; ++y) {
            for (uint64_t x = 0; x < n_sides; ++x) {
                *(samples_p++) = Vec2((x + (y + rand01()) * inv_side) * inv_side,
                                      (y + (x + rand01()) * inv_side) * inv_side);
            }
        }
        samples_p = *(rows++);
        for (uint64_t y = 0; y < n_sides; ++y) {
            auto z = randint(n_sides - y);
            for (uint64_t x = 0; x < n_sides; ++x) {
                swap(samples_p[y * n_sides + x].x, samples_p[z * n_sides + x].x);
            }
        }
        for (uint64_t x = 0; x < n_sides; ++x) {
            auto z = randint(n_sides - x);
            for (uint64_t y = 0; y < n_sides; ++y) {
                swap(samples_p[y * n_sides + x].y, samples_p[y * n_sides + z].y);
            }
        }
    }
    return samples;
}


/******************************************************************************
*********************************  sample types  ******************************
******************************************************************************/

/***********************************  Square  ********************************/

Vec2 Square::map_sample(Vec2 const& samp) const {
    return samp;
}

/***********************************  Circle  ********************************/

Vec2 Circle::map_sample(Vec2 const& samp) const {
#ifndef USE_SHIRLEY_CIRCLE_MAPPING
    auto r = sqrt(samp.x);
    auto phi = TwoPi * samp.y;
#else
    double r, phi;
    auto x = 2.0 * samp.x - 1.0, y = 2.0 * samp.y - 1.0;
    if (x > y) {
        if (x > -y) {
            r = x;
            phi = y / x;
        }
        else {
            r = -y;
            phi = 6.0 - x / y;
        }
    }
    else {
        if (x > -y) {
            r = y;
            phi = 2.0 - x / y;
        }
        else {
            r = -x;
            phi = 4.0 + y / x;
        }
    }
    phi *= PiOverFour;
#endif // USE_SHIRLEY_CIRCLE_MAPPING
    return Vec2(r * cos(phi), r * sin(phi));
}

/***********************************  Sphere  ********************************/

Vec3 Sphere::map_sample(Vec2 const& samp) const {
    auto ct = 1.0 - 2.0 * samp.x;
    auto st = sqrt(1.0 - ct * ct);
    auto phi = TwoPi * samp.y;
    return Vec3(st * cos(phi), st * sin(phi), ct);
}

/*********************************  Hemisphere  ******************************/

Hemisphere::Hemisphere()
    : c(1.0) {}
Hemisphere::Hemisphere(double c_)
    : c(c_) {}
Vec3 Hemisphere::map_sample(Vec2 const& samp) const {
    auto ct = pow(1.0 - samp.x, 1.0 / (this->c + 1.0));
    auto st = sqrt(1.0 - ct * ct);
    auto phi = TwoPi * samp.y;
    return Vec3(st * cos(phi), st * sin(phi), ct);
}
