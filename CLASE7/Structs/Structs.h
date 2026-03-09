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

// ===== ESTRUCTURA PARA EXT2 =====

struct SuperBlock {
    int32_t filesystem_type;     // 2 = EXT2
    int32_t inodes_count;
    int32_t blocks_count;
    int32_t free_inodes_count;
    int32_t free_blocks_count;

    int32_t bm_inode_start;
    int32_t bm_block_start;
    int32_t inode_start;
    int32_t block_start;
};

struct Inode {
    int32_t uid;
    int32_t gid;
    int32_t size;
    int32_t atime;
    int32_t ctime;
    int32_t mtime;

    int32_t block[15];   // 0-11 directos
                         // 12 indirecto simple
                         // 13 doble
                         // 14 triple

    char type;           // 0 carpeta, 1 archivo
    int32_t perm;
};

struct Content {
    char name[12];
    int32_t inode;
};

struct FolderBlock {
    Content content[4];
};

struct FileBlock {
    char content[64];
};

struct PointerBlock {
    int32_t pointers[16];
};

#pragma pack(pop)