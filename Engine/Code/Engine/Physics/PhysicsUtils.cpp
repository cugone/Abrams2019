#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Physics/Collider.hpp"

Vector2 MathUtils::CalcClosestPoint(const Vector2& p, const Collider& collider) {
    return collider.Support(p - collider.CalcCenter());
}

static constexpr int maxGJKIterations = 25;

bool PhysicsUtils::GJKIntersect(const Collider& a, const Collider& b) {
    return GJK(a, b).collides;
}

GJKResult PhysicsUtils::GJK(const Collider& a, const Collider& b) {
    using Simplex = std::vector<Vector3>;
    const auto calcMinkowskiDiff = [](const Vector2& direction, const Collider& a) { return a.Support(direction); };
    const auto support = [&](const Vector2& direction) { return calcMinkowskiDiff(direction, a) - calcMinkowskiDiff(-direction, b); };
    const auto initialDisplacement = b.CalcCenter() - a.CalcCenter();
    const auto initialDirection = initialDisplacement.GetNormalize();
    auto A = support(Vector2::X_AXIS);
    Simplex simplex{Vector3{A}};
    auto D = Vector3{-A};
    const auto doSimplexLine = [&](Simplex& simplex, Vector3& D) {
        const auto pointA = simplex[0];
        const auto pointB = simplex[1];
        const auto lineAB = pointB - pointA;
        const auto lineAO = -pointA;
        if(MathUtils::DotProduct(lineAB, lineAO) > 0.0f) {
            D = MathUtils::TripleProductVector(lineAB, lineAO, lineAB);
        } else {
            D = lineAO;
            simplex = Simplex{Vector3{A}};
        }
        return false;
    };
    const auto doSimplexTriangle = [&](Simplex& simplex, Vector3& D) {
        const auto pointA = simplex[0];
        const auto pointB = simplex[1];
        const auto pointC = simplex[2];
        const auto lineAB = pointB - pointA;
        const auto lineAC = pointC - pointA;
        const auto lineAO = (-pointA);
        const auto abc = MathUtils::CrossProduct(lineAB, lineAC);

        if(MathUtils::DotProduct(MathUtils::CrossProduct(abc, lineAC), lineAO) > 0.0f) {
            if(MathUtils::DotProduct(lineAC, lineAO) > 0.0f) {
                simplex = {pointA, pointC};
                D = MathUtils::TripleProductVector(lineAC, lineAO, lineAC);
            } else {
                simplex = {pointA, pointB};
                return doSimplexLine(simplex, D);
            }
        } else {
            if(MathUtils::DotProduct(MathUtils::CrossProduct(lineAB, abc), lineAO) > 0.0f) {
                simplex = {pointA, pointB};
                return doSimplexLine(simplex, D);
            } else {
                if(MathUtils::DotProduct(abc, lineAO) > 0.0f) {
                    D = abc;
                } else {
                    simplex = {pointA, pointC, pointB};
                    D = -abc;
                }
                return true;
            }
        }
        return false;
    };
    const auto doSimplex = [&](Simplex& simplex, Vector3& D) {
        const auto S = simplex.size();
        switch(S) {
        case 2:
            return doSimplexLine(simplex, D);
        case 3:
            return doSimplexTriangle(simplex, D);
        default:
            return false;
        }
    };
    const auto result = [&](Simplex& simplex, Vector3& D) {
        for(;;) {
            A = support(Vector2{D});
            if(MathUtils::DotProduct(A, Vector2{D}) <= 0.0f) {
                return false;
            }
            simplex.insert(std::begin(simplex), Vector3{A});
            if(doSimplex(simplex, D)) {
                return true;
            }
        }
    }(simplex, D); //IIIL
    return GJKResult{result, simplex};
    //return GJKResult{false, simplex};
}

EPAResult PhysicsUtils::EPA(const GJKResult& gjk, const Collider& a, const Collider& b) {
    if(!gjk.collides) {
        return {};
    }
    using Simplex = std::vector<Vector3>;
    using Edge = std::tuple<float, Vector3, std::size_t>;
    auto simplex_copy = gjk.simplex;
    GUARANTEE_OR_DIE(simplex_copy.size() == 3, "EPA Simplex not a triangle.");
    const auto findClosestEdge = [](const Simplex& simplex) {
        float distance = std::numeric_limits<float>::infinity();
        Edge closest = std::make_tuple(distance, Vector3::ZERO, -1);
        for(std::size_t i = 0; i < simplex.size(); ++i) {
            const auto j = (i + 1) % simplex.size();
            const auto a = simplex[i];
            const auto b = simplex[j];
            const auto e = b - a;
            const auto oa = a;
            const auto n = MathUtils::TripleProductVector(e, oa, e).GetNormalize();
            float d = MathUtils::DotProduct(n, a);
            if(d < distance) {
                distance = d;
                std::get<0>(closest) = d;
                std::get<1>(closest) = n;
                std::get<2>(closest) = j;
            }
        }
        return closest;
    };
    const auto calcMinkowskiDiff = [](const Vector3& direction, const Collider& a) { return a.Support(Vector2{direction}); };
    const auto support = [&](const Vector3& direction) { return calcMinkowskiDiff(direction, a) - calcMinkowskiDiff(-direction, b); };
    for(;;) {
        const auto edge = findClosestEdge(simplex_copy);
        const auto distance = std::get<0>(edge);
        const auto n = std::get<1>(edge);
        GUARANTEE_RECOVERABLE(!MathUtils::IsEquivalentToZero(n), "EPA n was zero.");
        const auto i = std::get<2>(edge);
        const auto p = Vector3{support(n)};
        const auto d = MathUtils::DotProduct(p, Vector3{n});
        //Is "distance" within some tolerance? i.e. is it "close enough"?
        //epsilon defaults to 0.00001, which I think is "close enough".
        if(d - distance < 0.0001f) {
            return {d, n};
        } else {
            simplex_copy.insert(std::begin(simplex_copy) + i, Vector3{p});
        }
    }
}

bool PhysicsUtils::SAT(const Collider& a, const Collider& b) {
    const auto* polyA = dynamic_cast<const ColliderPolygon*>(&a);
    const auto* polyB = dynamic_cast<const ColliderPolygon*>(&b);
    if(!(polyA && polyB)) {
        return false;
    }
    return MathUtils::DoPolygonsOverlap(polyA->GetPolygon(), polyB->GetPolygon());
}
