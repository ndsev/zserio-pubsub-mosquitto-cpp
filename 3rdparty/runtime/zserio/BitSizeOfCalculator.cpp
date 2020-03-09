#include <limits>

#include "zserio/CppRuntimeException.h"
#include "zserio/StringConvertUtil.h"
#include "zserio/BitPositionUtil.h"
#include "zserio/BitSizeOfCalculator.h"

namespace zserio
{

static const uint64_t VarInt16MaxValues[] =
{
    (UINT64_C(1) << (6)) - 1,
    (UINT64_C(1) << (6 + 8)) - 1,
};
static const size_t VarInt16MaxNumValues = sizeof(VarInt16MaxValues) / sizeof(VarInt16MaxValues[0]);

static const uint64_t VarInt32MaxValues[] =
{
    (UINT64_C(1) << (6)) - 1,
    (UINT64_C(1) << (6 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 8)) - 1
};
static const size_t VarInt32MaxNumValues = sizeof(VarInt32MaxValues) / sizeof(VarInt32MaxValues[0]);

static const uint64_t VarInt64MaxValues[] =
{
    (UINT64_C(1) << (6)) - 1,
    (UINT64_C(1) << (6 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7 + 7 + 8)) - 1
};
static const size_t VarInt64MaxNumValues = sizeof(VarInt64MaxValues) / sizeof(VarInt64MaxValues[0]);

static const uint64_t VarUInt16MaxValues[] =
{
    (UINT64_C(1) << (7)) - 1,
    (UINT64_C(1) << (7 + 8)) - 1,
};
static const size_t VarUInt16MaxNumValues = sizeof(VarUInt16MaxValues) / sizeof(VarUInt16MaxValues[0]);

static const uint64_t VarUInt32MaxValues[] =
{
    (UINT64_C(1) << (7)) - 1,
    (UINT64_C(1) << (7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 8)) - 1
};
static const size_t VarUInt32MaxNumValues = sizeof(VarUInt32MaxValues) / sizeof(VarUInt32MaxValues[0]);

static const uint64_t VarUInt64MaxValues[] =
{
    (UINT64_C(1) << (7)) - 1,
    (UINT64_C(1) << (7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7 + 7 + 8)) - 1
};
static const size_t VarUInt64MaxNumValues = sizeof(VarUInt64MaxValues) / sizeof(VarUInt64MaxValues[0]);

static const uint64_t VarIntMaxValues[] =
{
    (UINT64_C(1) << (6)) - 1,
    (UINT64_C(1) << (6 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (6 + 7 + 7 + 7 + 7 + 7 + 7 + 7 + 8)) - 1
};
static const size_t VarIntMaxNumValues = sizeof(VarIntMaxValues) / sizeof(VarIntMaxValues[0]);

static const uint64_t VarUIntMaxValues[] =
{
    (UINT64_C(1) << (7)) - 1,
    (UINT64_C(1) << (7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    (UINT64_C(1) << (7 + 7 + 7 + 7 + 7 + 7 + 7 + 7)) - 1,
    UINT64_MAX
};
static const size_t VarUIntMaxNumValues = sizeof(VarUIntMaxValues) / sizeof(VarUIntMaxValues[0]);

static size_t bitSizeOfVarIntImpl(uint64_t value, const uint64_t* maxValues, size_t numMaxValues)
{
    const uint64_t* maxValue = maxValues;
    size_t byteSize = 1;
    for (; byteSize <= numMaxValues; ++byteSize)
    {
        if (value <= *maxValue)
            break;
        maxValue++;
    }

    if (byteSize > numMaxValues)
    {
        throw CppRuntimeException("VarInt" + convertToString(bytesToBits(numMaxValues)) + " value " +
                convertToString(value) + " is out of range");
    }

    return bytesToBits(byteSize);
}

template <typename T>
static uint64_t convertToAbsValue(T value)
{
    return static_cast<uint64_t>((value < 0) ? -value : value);
}

size_t bitSizeOfVarInt16(int16_t value)
{
    return bitSizeOfVarIntImpl(convertToAbsValue(value), VarInt16MaxValues, VarInt16MaxNumValues);
}

size_t bitSizeOfVarInt32(int32_t value)
{
    return bitSizeOfVarIntImpl(convertToAbsValue(value), VarInt32MaxValues, VarInt32MaxNumValues);
}

size_t bitSizeOfVarInt64(int64_t value)
{
    return bitSizeOfVarIntImpl(convertToAbsValue(value), VarInt64MaxValues, VarInt64MaxNumValues);
}

size_t bitSizeOfVarUInt16(uint16_t value)
{
    return bitSizeOfVarIntImpl(value, VarUInt16MaxValues, VarUInt16MaxNumValues);
}

size_t bitSizeOfVarUInt32(uint32_t value)
{
    return bitSizeOfVarIntImpl(value, VarUInt32MaxValues, VarUInt32MaxNumValues);
}

size_t bitSizeOfVarUInt64(uint64_t value)
{
    return bitSizeOfVarIntImpl(value, VarUInt64MaxValues, VarUInt64MaxNumValues);
}

size_t bitSizeOfVarInt(int64_t value)
{
    if (value == INT64_MIN)
        return 8; // INT64_MIN is stored as -0

    return bitSizeOfVarIntImpl(convertToAbsValue(value), VarIntMaxValues, VarIntMaxNumValues);
}

size_t bitSizeOfVarUInt(uint64_t value)
{
    return bitSizeOfVarIntImpl(value, VarUIntMaxValues, VarUIntMaxNumValues);
}

size_t bitSizeOfString(const std::string& stringValue)
{
    const size_t stringSize = stringValue.size();

    // the string consists of varuint64 for size followed by the UTF-8 encoded string
    return bitSizeOfVarUInt64(static_cast<uint64_t>(stringSize)) + bytesToBits(stringSize);
}

size_t bitSizeOfBitBuffer(const BitBuffer& bitBuffer)
{
    const size_t bitBufferSize = bitBuffer.getBitSize();

    // bit buffer consists of varuint64 for bit size followed by the bits
    return bitSizeOfVarUInt64(static_cast<uint64_t>(bitBufferSize)) + bitBufferSize;
}

} // namespace zserio
