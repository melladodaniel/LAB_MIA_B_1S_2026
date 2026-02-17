#pragma once
#include <cstdint>

#pragma pack(push, 1)

struct Partition {
    char Status[1];
    char Type[1];
    char Fit[2];
    int32_t Start;
    int32_t Size;
    char Name[16];
    int32_t Correlative;
    char Id[4];
};

struct MBR {
    int32_t MbrSize;
    char CreationDate[10];
    int32_t Signature;
    char Fit[2];
    Partition Partitions[4];
};

#pragma pack(pop)