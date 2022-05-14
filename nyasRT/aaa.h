struct LocalCoord final {
public:
    Vec3 u, v, w;

    LocalCoord(Vec3 const& n);
    LocalCoord(Vec3 const& n, Vec3 const& u_);

    LocalCoord(LocalCoord const&) = default;
    LocalCoord(LocalCoord &&) = default;
    LocalCoord & operator =(LocalCoord const&) = default;
    LocalCoord & operator =(LocalCoord &&) = default;

    Vec3 at(Vec3 const&);
};
