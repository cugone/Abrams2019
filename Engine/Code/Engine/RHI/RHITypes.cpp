#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Math/MathUtils.hpp"

#include <iomanip>
#include <type_traits>


bool operator==(const ViewportDesc& a, const ViewportDesc& b) noexcept {
    bool x = MathUtils::IsEquivalent(a.x, b.x);
    bool y = MathUtils::IsEquivalent(a.y, b.y);
    bool w = MathUtils::IsEquivalent(a.width, b.width);
    bool h = MathUtils::IsEquivalent(a.height, b.height);
    bool i = MathUtils::IsEquivalent(a.minDepth, b.minDepth);
    bool m = MathUtils::IsEquivalent(a.maxDepth, b.maxDepth);
    return x && y && w && h && i && m;
}

bool operator!=(const ViewportDesc& a, const ViewportDesc& b) noexcept {
    return !(a == b);
}


std::ostream& operator<<(std::ostream& out_stream, const GraphicsCardDesc& graphicsCardDesc) noexcept {
    auto name = graphicsCardDesc.Description;
    out_stream << std::left << std::setw(22) << "Name:" << std::right << std::setw(30) << name << '\n';
    auto vid = graphicsCardDesc.VendorId;
    out_stream << std::hex << std::uppercase;
    out_stream << std::left << std::setw(22) << "Vendor ID:" << std::right << std::setw(30) << vid << '\n';
    auto did = graphicsCardDesc.DeviceId;
    out_stream << std::left << std::setw(22) << "Device ID:" << std::right << std::setw(30) << did << '\n';
    auto subsysid = graphicsCardDesc.SubSysId;
    out_stream << std::left << std::setw(22) << "Subsystem ID:" << std::right << std::setw(30) << subsysid << '\n';
    auto revision = graphicsCardDesc.Revision;
    out_stream << std::left << std::setw(22) << "Revision:" << std::right << std::setw(30) << revision << '\n';
    out_stream << std::dec << std::nouppercase;
    auto ded_vid_mem = graphicsCardDesc.DedicatedVideoMemory;
    out_stream << std::left << std::setw(22) << "Video Memory:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << static_cast<long double>(ded_vid_mem) * MathUtils::GIB_BYTES_RATIO.num / MathUtils::GIB_BYTES_RATIO.den << " GB\n";
    auto ded_sys_mem = graphicsCardDesc.DedicatedSystemMemory;
    out_stream << std::left << std::setw(22) << "System Memory:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << static_cast<long double>(ded_sys_mem) * MathUtils::GIB_BYTES_RATIO.num / MathUtils::GIB_BYTES_RATIO.den << " GB\n";
    auto shared_mem = graphicsCardDesc.SharedSystemMemory;
    out_stream << std::left << std::setw(22) << "Shared System Memory:" << std::right << std::setw(30) << std::fixed << std::setprecision(1) << static_cast<long double>(shared_mem) * MathUtils::GIB_BYTES_RATIO.num / MathUtils::GIB_BYTES_RATIO.den << " GB\n";
    bool is_software = graphicsCardDesc.is_software;
    bool is_unspecified = graphicsCardDesc.is_unspecified;
    out_stream << std::left << std::setw(22) << "Adapter Type:" << std::right << std::setw(30);
    if(!is_unspecified) {
        out_stream << (!is_software ? "Hardware" : "Software") << std::right << std::setw(30);
    } else {
        out_stream << "Unknown" << std::right << std::setw(30);
    }
    return out_stream;
}

bool DisplayDescLTComparator::operator()(const DisplayDesc& a, const DisplayDesc& b) const noexcept {
    if(a.width < b.width) {
        return true;
    }
    if(b.width < a.width) {
        return false;
    }
    if(a.height < b.height) {
        return true;
    }
    if(b.height < a.height) {
        return false;
    }
    if(a.refreshRateHz < b.refreshRateHz) {
        return true;
    }
    if(b.refreshRateHz < a.refreshRateHz) {
        return false;
    }
    return false;
}

bool DisplayDescGTComparator::operator()(const DisplayDesc& a, const DisplayDesc& b) const noexcept {
    return DisplayDescLTComparator{}.operator()(b,a);
}

RHIOutputMode& operator++(RHIOutputMode& mode) noexcept {
    using underlying = std::underlying_type_t<RHIOutputMode>;
    mode = static_cast<RHIOutputMode>(static_cast<underlying>(mode) + 1);
    if(mode == RHIOutputMode::Last_) {
        mode = RHIOutputMode::First_;
    }
    return mode;
}

RHIOutputMode operator++(RHIOutputMode& mode, int) noexcept {
    RHIOutputMode result = mode;
    ++mode;
    return result;
}

BufferUsage& operator&=(BufferUsage& a, const BufferUsage& b) noexcept {
    using underlying = std::underlying_type_t<BufferUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferUsage>(underlying_a & underlying_b);
    return a;
}

BufferUsage& operator|=(BufferUsage& a, const BufferUsage& b) noexcept {
    using underlying = std::underlying_type_t<BufferUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferUsage>(underlying_a | underlying_b);
    return a;
}

BufferUsage operator&(BufferUsage a, const BufferUsage& b) noexcept {
    a &= b;
    return a;
}

BufferUsage operator|(BufferUsage a, const BufferUsage& b) noexcept {
    a |= b;
    return a;
}

BufferBindUsage& operator|=(BufferBindUsage& a, const BufferBindUsage& b) noexcept {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferBindUsage>(underlying_a | underlying_b);
    return a;
}

BufferBindUsage operator|(BufferBindUsage a, const BufferBindUsage& b) noexcept {
    a |= b;
    return a;
}

BufferBindUsage& operator&=(BufferBindUsage& a, const BufferBindUsage& b) noexcept {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BufferBindUsage>(underlying_a & underlying_b);
    return a;
}

BufferBindUsage operator&(BufferBindUsage a, const BufferBindUsage& b) noexcept {
    a &= b;
    return a;
}

PipelineStage& operator|=(PipelineStage& a, const PipelineStage& b) noexcept {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<PipelineStage>(underlying_a | underlying_b);
    return a;
}

PipelineStage operator|(PipelineStage a, const PipelineStage& b) noexcept {
    a |= b;
    return a;
}

PipelineStage& operator&=(PipelineStage& a, const PipelineStage& b) noexcept {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<PipelineStage>(underlying_a & underlying_b);
    return a;
}

PipelineStage operator&(PipelineStage a, const PipelineStage& b) noexcept {
    a &= b;
    return a;
}

PipelineStage& operator^=(PipelineStage& a, const PipelineStage& b) noexcept {
    using underlying = std::underlying_type_t<PipelineStage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<PipelineStage>(underlying_a ^ underlying_b);
    return a;
}

PipelineStage operator^(PipelineStage a, const PipelineStage& b) noexcept {
    a ^= b;
    return a;
}

BlendColorWriteEnable operator~(BlendColorWriteEnable a) noexcept {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    return static_cast<BlendColorWriteEnable>(~underlying_a);
}

BlendColorWriteEnable& operator&=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) noexcept {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a & underlying_b);
    return a;
}

BlendColorWriteEnable operator&(BlendColorWriteEnable a, const BlendColorWriteEnable& b) noexcept {
    a &= b;
    return a;
}

BlendColorWriteEnable& operator|=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) noexcept {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a | underlying_b);
    return a;
}

BlendColorWriteEnable operator|(BlendColorWriteEnable a, const BlendColorWriteEnable& b) noexcept {
    a |= b;
    return a;
}

BlendColorWriteEnable& operator^=(BlendColorWriteEnable& a, const BlendColorWriteEnable& b) noexcept {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    a = static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
    return a;
}

BlendColorWriteEnable operator^(BlendColorWriteEnable a, const BlendColorWriteEnable& b) noexcept {
    using underlying = std::underlying_type_t<BlendColorWriteEnable>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BlendColorWriteEnable>(underlying_a ^ underlying_b);
}

ResourceMiscFlag operator|(const ResourceMiscFlag& a, const ResourceMiscFlag& b) noexcept {
    using underlying = std::underlying_type_t<ResourceMiscFlag>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<ResourceMiscFlag>(underlying_a | underlying_b);
}

ResourceMiscFlag operator&(const ResourceMiscFlag& a, const ResourceMiscFlag& b) noexcept {
    using underlying = std::underlying_type_t<ResourceMiscFlag>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<ResourceMiscFlag>(underlying_a & underlying_b);
}
