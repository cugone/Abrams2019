#pragma once

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <type_traits>
#include <vector>

template<typename T>
class QuadTree {
public:

    QuadTree() = default;
    explicit QuadTree(const AABB2& bounds);
    QuadTree(const QuadTree& other) = delete;
    QuadTree(QuadTree&& other) = default;
    QuadTree& operator=(const QuadTree& other) = delete;
    QuadTree& operator=(QuadTree&& other) = default;
    ~QuadTree() = default;

    void Add(T* new_element);
    void Add(std::vector<T*> new_elements);
    void Remove(const T* old_element);
    void Clear();
    void DebugRender(Renderer& renderer) const;

    void SetWorldBounds(const AABB2& bounds) noexcept;

protected:
private:
    explicit QuadTree(QuadTree<T>* parent, const AABB2& bounds);
    explicit QuadTree(QuadTree<T>* parent, const AABB2& bounds, const std::vector<std::add_pointer_t<T>>& elements);

    enum class ChildID {
        BottomLeft
        , TopLeft
        , TopRight
        , BottomRight
    };

    void DebugRender_helper(Renderer& renderer) const;
    bool IsParent(const QuadTree<T>* node) const;
    bool IsChild(const QuadTree<T>* node) const;
    bool IsLeaf(const QuadTree<T>* node) const;
    bool IsParent() const;
    bool IsChild() const;
    bool IsLeaf() const;
    bool IsElementIntersectingMe(const T* new_element) const;
    bool NeedsSubdivide() const;
    bool NeedsUnSubdivide() const;
    void Subdivide();
    void UnSubdivide();
    void MakeChildren();
    void ClearChildren();
    void AddElement(T* old_element);
    void RemoveElement(const T* old_element);
    void GiveElementsToChildren();
    void TakeElementsFromChildren();
    const QuadTree<T>* GetChild(const ChildID& id) const;
    QuadTree<T>* GetChild(const ChildID& id);
    void CreateChild(const ChildID& id);
    void DeleteChild(const ChildID& id);
    void SetChild(const ChildID& id, std::unique_ptr<QuadTree<T>> child);

    const std::size_t ChildIdToIndex(const ChildID& id) const;
    bool IsLeaf(const QuadTree<T>& node) const;
    AABB2 GetBounds() const;
    AABB2 GetParentBounds() const;

    QuadTree<T>* _parent = nullptr;
    Vector2 _half_extents = Vector2::ONE;
    AABB2 _bounds = AABB2{ -_half_extents, _half_extents };
    std::array<std::unique_ptr<QuadTree>, 4> _children{};
    const int _maxElementsBeforeSubdivide = 2;    
    const int _maxChildren = 4;
    std::vector<std::add_pointer_t<T>> _elements{};
};

template<typename T>
AABB2 QuadTree<T>::GetParentBounds() const {
    return _parent ? _parent->_bounds : AABB2{};
}

template<typename T>
bool QuadTree<T>::IsLeaf(const QuadTree<T>* node) const {
    return node ? node->_children[0] == nullptr : false;
}

template<typename T>
bool QuadTree<T>::IsChild(const QuadTree<T>* node) const {
    return node ? node->_parent != nullptr : false;
}

template<typename T>
bool QuadTree<T>::IsParent(const QuadTree<T>* node) const {
    return node ? node->_children[0] != nullptr : false;
}

template<typename T>
bool QuadTree<T>::IsLeaf() const {
    return _children[0] == nullptr;
}

template<typename T>
bool QuadTree<T>::IsChild() const {
    return _parent != nullptr;
}

template<typename T>
bool QuadTree<T>::IsParent() const {
    return _children[0] != nullptr;
}

template<typename T>
void QuadTree<T>::SetWorldBounds(const AABB2& bounds) noexcept {
    _bounds = bounds;
    _half_extents = _bounds.CalcDimensions() * 0.5f;
    if(IsParent()) {
        auto* bl = GetChild(ChildID::BottomLeft);
        bl->_half_extents = _half_extents * 0.5f;
        bl->_bounds.mins = -_half_extents;
        bl->_bounds.maxs = _half_extents;
        bl->_bounds.Translate(GetParentBounds().CalcCenter() + Vector2{ -_half_extents.x, +_half_extents.y });
        auto* tl = GetChild(ChildID::TopLeft);
        tl->_half_extents = _half_extents * 0.5f;
        tl->_bounds.mins = -_half_extents;
        tl->_bounds.maxs = _half_extents;
        tl->_bounds.Translate(GetParentBounds().CalcCenter() + Vector2{ -_half_extents.x, -_half_extents.y });
        auto* tr = GetChild(ChildID::TopRight);
        tr->_half_extents = _half_extents * 0.5f;
        tr->_bounds.mins = -_half_extents;
        tr->_bounds.maxs = _half_extents;
        tr->_bounds.Translate(GetParentBounds().CalcCenter() + Vector2{ +_half_extents.x, -_half_extents.y });
        auto* br = GetChild(ChildID::BottomRight);
        br->_half_extents = _half_extents * 0.5f;
        br->_bounds.mins = -_half_extents;
        br->_bounds.maxs = _half_extents;
        br->_bounds.Translate(GetParentBounds().CalcCenter() + Vector2{ +_half_extents.x, +_half_extents.y });
        for(auto& child : _children) {
            if(child) {
                child->SetWorldBounds(child->_bounds);
            }
        }
    }
}
template<typename T>
void QuadTree<T>::Clear() {
    for(auto& child : _children) {
        if(child) {
            child->Clear();
        }
    }
    _elements.clear();
    for(auto& child : _children) {
        child.reset(nullptr);
    }
}

template<typename T>
void QuadTree<T>::Remove(const T* old_element) {
    if(!IsLeaf(this)) {
        for(auto& child : _children) {
            if(child) {
                child->RemoveElement(old_element);
                return;
            }
        }
    }
    RemoveElement(old_element);
}

template<typename T>
void QuadTree<T>::Add(T* new_element) {
    if(!IsElementIntersectingMe(new_element)) {
        return;
    }
    if(!IsLeaf(*this)) {
        for(auto& child : _children) {
            if(child) {
                child->Add(new_element);
            }
        }
        return;
    }
    _elements.push_back(new_element);
    Subdivide();
}

template<typename T>
void QuadTree<T>::Add(std::vector<T*> new_elements) {
    _elements.reserve(_elements.size() + new_elements.size());
    std::merge(std::begin(_elements), std::end(_elements), std::begin(new_elements), std::end(new_elements), std::back_inserter(_elements));
    Subdivide();
}

template<typename T>
bool QuadTree<T>::IsLeaf(const QuadTree<T>& node) const {
    return node._children[0] == nullptr;
}

template<typename T>
AABB2 QuadTree<T>::GetBounds() const {
    return _bounds;
}

template<typename T>
void QuadTree<T>::AddElement(T* old_element) {
    _elements.push_back(old_element);
}

template<typename T>
void QuadTree<T>::RemoveElement(const T* old_element) {
    for(auto iter = std::begin(_elements); iter != std::end(_elements); ++iter) {
        if(*iter == old_element) {
            std::iter_swap(iter, std::end(_elements));
            _elements.pop_back();
            break;
        }
    }
}

template<typename T>
void QuadTree<T>::DebugRender(Renderer& renderer) const {
    renderer.SetMaterial(renderer.GetMaterial("__2D"));
    renderer.SetModelMatrix(Matrix4::I);
    DebugRender_helper(renderer);
}

template<typename T>
const std::size_t QuadTree<T>::ChildIdToIndex(const ChildID& id) const {
    switch(id) {
    case ChildID::BottomLeft: return 0;
    case ChildID::BottomRight: return 1;
    case ChildID::TopLeft: return 2;
    case ChildID::TopRight: return 3;
    default:
        ERROR_AND_DIE("QuadTree: ChildToIndex invalid index.")
    }
}

template<typename T>
const QuadTree<T>* QuadTree<T>::GetChild(const ChildID& id) const {
    const auto index = ChildIdToIndex(id);
    return _children[index].get();
}

template<typename T>
QuadTree<T>* QuadTree<T>::GetChild(const ChildID& id) {
    const auto index = ChildIdToIndex(id);
    return _children[index].get();
}

template<typename T>
void QuadTree<T>::CreateChild(const ChildID& id) {
    const auto index = ChildIdToIndex(id);

    auto bounds = _bounds;
    bounds.ScalePadding(0.50f, 0.50f);
    switch(id) {
    case ChildID::BottomLeft:
    {
        const auto tl_corner = _bounds.CalcCenter() + Vector2(-_half_extents.x, 0.0f);
        const auto pos = tl_corner + Vector2(_half_extents.x, _half_extents.y) * 0.5f;
        bounds.SetPosition(pos);
        _children[index] = std::move(std::unique_ptr<QuadTree<T>>(new QuadTree<T>(this, bounds)));
        break;
    }
    case ChildID::BottomRight:
    {
        const auto tl_corner = _bounds.CalcCenter();
        const auto pos = tl_corner + _half_extents * 0.50f;
        bounds.SetPosition(pos);
        _children[index] = std::move(std::unique_ptr<QuadTree<T>>(new QuadTree<T>(this, bounds)));
        break;
    }
    case ChildID::TopLeft:
    {
        //bounds defaults to TopLeft by virtue of scale padding.
        _children[index] = std::move(std::unique_ptr<QuadTree<T>>(new QuadTree<T>(this, bounds)));
        break;
    }
    case ChildID::TopRight:
    {
        const auto tl_corner = _bounds.CalcCenter() + Vector2(0.0f, -_half_extents.y);
        const auto pos = tl_corner + _half_extents * 0.50f;
        bounds.SetPosition(pos);
        _children[index] = std::move(std::unique_ptr<QuadTree<T>>(new QuadTree<T>(this, bounds)));
        break;
    }
    default:
        ERROR_AND_DIE("QuadTree Child ID has changed.");
    }
}

template<typename T>
void QuadTree<T>::DeleteChild(const ChildID& id) {
    const auto index = ChildIdToIndex(id);
    _children[index].reset(nullptr);
}

template<typename T>
void QuadTree<T>::SetChild(const ChildID& id, std::unique_ptr<QuadTree<T>> child) {
    const auto index = ChildIdToIndex(id);
    return _children[index].get();
}

template<typename T>
QuadTree<T>::QuadTree(const AABB2& bounds)
: _half_extents{ bounds.CalcDimensions() * 0.5f }
, _bounds{ bounds }
{
    /* DO NOTHING */
}

template<typename T>
QuadTree<T>::QuadTree(QuadTree<T>* parent, const AABB2& bounds)
    : _parent(parent)
    , _half_extents(bounds.CalcDimensions() * 0.5f)
    , _bounds(bounds)
{
    /* DO NOTHING */
    
}

template<typename T>
QuadTree<T>::QuadTree(QuadTree<T>* parent, const AABB2& bounds, const std::vector<std::add_pointer_t<T>>& elements)
    : _parent(parent)
    , _half_extents(bounds.CalcDimensions() * 0.5f)
    , _bounds(bounds)
    , _elements(elements)
{
    /* DO NOTHING */
}

template<typename T>
void QuadTree<T>::Subdivide() {
    if(NeedsSubdivide()) {
        MakeChildren();
        GiveElementsToChildren();
    }
}

template<typename T>
void QuadTree<T>::UnSubdivide() {
    if(NeedsUnSubdivide()) {
        TakeElementsFromChildren();
        ClearChildren();
    }
}

template<typename T>
void QuadTree<T>::MakeChildren() {
    CreateChild(ChildID::BottomLeft);
    CreateChild(ChildID::TopLeft);
    CreateChild(ChildID::TopRight);
    CreateChild(ChildID::BottomRight);
}

template<typename T>
void QuadTree<T>::ClearChildren() {
    DeleteChild(ChildID::BottomLeft);
    DeleteChild(ChildID::TopLeft);
    DeleteChild(ChildID::TopRight);
    DeleteChild(ChildID::BottomRight);
}

template<typename T>
void QuadTree<T>::GiveElementsToChildren() {
    for(auto& elem : _elements) {
        for(auto& child : _children) {
            if(child) {
                if(MathUtils::Contains(child->GetBounds(), elem->GetBounds())) {
                    child->AddElement(elem);
                    elem = nullptr;
                    break;
                }
            }
        }
    }
    _elements.erase(std::remove_if(std::begin(_elements), std::end(_elements), [](const T* a) { return a == nullptr; }), std::end(_elements));
    for(auto& child : _children) {
        if(child) {
            child->Subdivide();
        }
    }
}

template<typename T>
void QuadTree<T>::TakeElementsFromChildren() {
    const auto max_elems = std::size_t{0};
    for(auto& child : _children) {
        max_elems += child->_elements.size();
    }
    _elements.reserve(max_elems);
    for(auto& child : _children) {
        std::merge(std::begin(child->_elements), std::end(child->_elements), std::begin(_elements), std::end(_elements), std::back_inserter(_elements));
    }
}

template<typename T>
void QuadTree<T>::DebugRender_helper(Renderer& renderer) const {
    renderer.DrawAABB2(_bounds, Rgba::Green, Rgba::NoAlpha);
    for(const auto& child : _children) {
        if(child) {
            child->DebugRender_helper(renderer);
        }
    }
}

template<typename T>
bool QuadTree<T>::NeedsSubdivide() const {
    return !((std::min)(_half_extents.x, _half_extents.y) < 0.5) && _maxElementsBeforeSubdivide < _elements.size();
}

template<typename T>
bool QuadTree<T>::NeedsUnSubdivide() const {
    return !(_maxElementsBeforeSubdivide < _elements.size());
}

template<typename T>
bool QuadTree<T>::IsElementIntersectingMe(const T* new_element) const {
    return MathUtils::DoOBBsOverlap(GetBounds(), new_element->GetBounds());
}

