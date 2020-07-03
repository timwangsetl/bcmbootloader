/*
 * $Copyright Open Broadcom Corporation$
 */
#ifndef IPROC_I2S_DATA_H
#define IPROC_I2S_DATA_H

static unsigned short const sinwave16_800[]={
0x0000, 0x0101, 0x0202, 0x0303, 0x0405, 0x0506, 0x0607, 0x0708,
0x0809, 0x090A, 0x0A0A, 0x0B0B, 0x0C0B, 0x0D0B, 0x0E0B, 0x0F0B,
0x100A, 0x1109, 0x1208, 0x1307, 0x1405, 0x1503, 0x1601, 0x16FE,
0x17FB, 0x18F8, 0x19F4, 0x1AF0, 0x1BEB, 0x1CE6, 0x1DE1, 0x1EDB,
0x1FD4, 0x20CD, 0x21C6, 0x22BE, 0x23B5, 0x24AC, 0x25A2, 0x2698,
0x278D, 0x2882, 0x2975, 0x2A68, 0x2B5B, 0x2C4D, 0x2D3E, 0x2E2E,
0x2F1E, 0x300D, 0x30FB, 0x31E8, 0x32D5, 0x33C1, 0x34AC, 0x3596,
0x367F, 0x3767, 0x384F, 0x3936, 0x3A1B, 0x3B00, 0x3BE4, 0x3CC7,
0x3DA9, 0x3E8A, 0x3F6A, 0x4049, 0x4127, 0x4204, 0x42E0, 0x43BB,
0x4495, 0x456E, 0x4645, 0x471C, 0x47F1, 0x48C6, 0x4999, 0x4A6B,
0x4B3B, 0x4C0B, 0x4CD9, 0x4DA7, 0x4E73, 0x4F3D, 0x5007, 0x50CF,
0x5196, 0x525C, 0x5320, 0x53E3, 0x54A5, 0x5565, 0x5624, 0x56E2,
0x579E, 0x5859, 0x5912, 0x59CB, 0x5A81, 0x5B37, 0x5BEA, 0x5C9D,
0x5D4E, 0x5DFD, 0x5EAB, 0x5F57, 0x6002, 0x60AC, 0x6154, 0x61FA,
0x629F, 0x6342, 0x63E4, 0x6484, 0x6523, 0x65BF, 0x665B, 0x66F4,
0x678D, 0x6823, 0x68B8, 0x694B, 0x69DC, 0x6A6C, 0x6AFA, 0x6B87,
0x6C12, 0x6C9B, 0x6D22, 0x6DA8, 0x6E2B, 0x6EAE, 0x6F2E, 0x6FAD,
0x7029, 0x70A5, 0x711E, 0x7195, 0x720B, 0x727F, 0x72F1, 0x7361,
0x73D0, 0x743D, 0x74A7, 0x7510, 0x7578, 0x75DD, 0x7640, 0x76A2,
0x7701, 0x775F, 0x77BB, 0x7815, 0x786D, 0x78C4, 0x7918, 0x796A,
0x79BB, 0x7A09, 0x7A56, 0x7AA1, 0x7AE9, 0x7B30, 0x7B75, 0x7BB8,
0x7BF9, 0x7C38, 0x7C75, 0x7CB0, 0x7CE9, 0x7D21, 0x7D56, 0x7D89,
0x7DBA, 0x7DE9, 0x7E17, 0x7E42, 0x7E6B, 0x7E92, 0x7EB8, 0x7EDB,
0x7EFC, 0x7F1B, 0x7F39, 0x7F54, 0x7F6D, 0x7F84, 0x7F99, 0x7FAD,
0x7FBE, 0x7FCD, 0x7FDA, 0x7FE5, 0x7FEE, 0x7FF5, 0x7FFA, 0x7FFD,
0x7FFF, 0x7FFD, 0x7FFA, 0x7FF5, 0x7FEE, 0x7FE5, 0x7FDA, 0x7FCD,
0x7FBE, 0x7FAD, 0x7F99, 0x7F84, 0x7F6D, 0x7F54, 0x7F39, 0x7F1B,
0x7EFC, 0x7EDB, 0x7EB8, 0x7E92, 0x7E6B, 0x7E42, 0x7E17, 0x7DE9,
0x7DBA, 0x7D89, 0x7D56, 0x7D21, 0x7CE9, 0x7CB0, 0x7C75, 0x7C38,
0x7BF9, 0x7BB8, 0x7B75, 0x7B30, 0x7AE9, 0x7AA1, 0x7A56, 0x7A09,
0x79BB, 0x796A, 0x7918, 0x78C4, 0x786D, 0x7815, 0x77BB, 0x775F,
0x7701, 0x76A2, 0x7640, 0x75DD, 0x7578, 0x7510, 0x74A7, 0x743D,
0x73D0, 0x7361, 0x72F1, 0x727F, 0x720B, 0x7195, 0x711E, 0x70A5,
0x7029, 0x6FAD, 0x6F2E, 0x6EAE, 0x6E2B, 0x6DA8, 0x6D22, 0x6C9B,
0x6C12, 0x6B87, 0x6AFA, 0x6A6C, 0x69DC, 0x694B, 0x68B8, 0x6823,
0x678D, 0x66F4, 0x665B, 0x65BF, 0x6523, 0x6484, 0x63E4, 0x6342,
0x629F, 0x61FA, 0x6154, 0x60AC, 0x6002, 0x5F57, 0x5EAB, 0x5DFD,
0x5D4E, 0x5C9D, 0x5BEA, 0x5B37, 0x5A81, 0x59CB, 0x5912, 0x5859,
0x579E, 0x56E2, 0x5624, 0x5565, 0x54A5, 0x53E3, 0x5320, 0x525C,
0x5196, 0x50CF, 0x5007, 0x4F3D, 0x4E73, 0x4DA7, 0x4CD9, 0x4C0B,
0x4B3B, 0x4A6B, 0x4999, 0x48C6, 0x47F1, 0x471C, 0x4645, 0x456E,
0x4495, 0x43BB, 0x42E0, 0x4204, 0x4127, 0x4049, 0x3F6A, 0x3E8A,
0x3DA9, 0x3CC7, 0x3BE4, 0x3B00, 0x3A1B, 0x3936, 0x384F, 0x3767,
0x367F, 0x3596, 0x34AC, 0x33C1, 0x32D5, 0x31E8, 0x30FB, 0x300D,
0x2F1E, 0x2E2E, 0x2D3E, 0x2C4D, 0x2B5B, 0x2A68, 0x2975, 0x2882,
0x278D, 0x2698, 0x25A2, 0x24AC, 0x23B5, 0x22BE, 0x21C6, 0x20CD,
0x1FD4, 0x1EDB, 0x1DE1, 0x1CE6, 0x1BEB, 0x1AF0, 0x19F4, 0x18F8,
0x17FB, 0x16FE, 0x1601, 0x1503, 0x1405, 0x1307, 0x1208, 0x1109,
0x100A, 0x0F0B, 0x0E0B, 0x0D0B, 0x0C0B, 0x0B0B, 0x0A0A, 0x090A,
0x0809, 0x0708, 0x0607, 0x0506, 0x0405, 0x0303, 0x0202, 0x0101,
0x0000, 0xFEFE, 0xFDFD, 0xFCFC, 0xFBFA, 0xFAF9, 0xF9F8, 0xF8F7,
0xF7F6, 0xF6F5, 0xF5F5, 0xF4F4, 0xF3F4, 0xF2F4, 0xF1F4, 0xF0F4,
0xEFF5, 0xEEF6, 0xEDF7, 0xECF8, 0xEBFA, 0xEAFC, 0xE9FE, 0xE901,
0xE804, 0xE707, 0xE60B, 0xE50F, 0xE414, 0xE319, 0xE21E, 0xE124,
0xE02B, 0xDF32, 0xDE39, 0xDD41, 0xDC4A, 0xDB53, 0xDA5D, 0xD967,
0xD872, 0xD77D, 0xD68A, 0xD597, 0xD4A4, 0xD3B2, 0xD2C1, 0xD1D1,
0xD0E1, 0xCFF2, 0xCF04, 0xCE17, 0xCD2A, 0xCC3E, 0xCB53, 0xCA69,
0xC980, 0xC898, 0xC7B0, 0xC6C9, 0xC5E4, 0xC4FF, 0xC41B, 0xC338,
0xC256, 0xC175, 0xC095, 0xBFB6, 0xBED8, 0xBDFB, 0xBD1F, 0xBC44,
0xBB6A, 0xBA91, 0xB9BA, 0xB8E3, 0xB80E, 0xB739, 0xB666, 0xB594,
0xB4C4, 0xB3F4, 0xB326, 0xB258, 0xB18C, 0xB0C2, 0xAFF8, 0xAF30,
0xAE69, 0xADA3, 0xACDF, 0xAC1C, 0xAB5A, 0xAA9A, 0xA9DB, 0xA91D,
0xA861, 0xA7A6, 0xA6ED, 0xA634, 0xA57E, 0xA4C8, 0xA415, 0xA362,
0xA2B1, 0xA202, 0xA154, 0xA0A8, 0x9FFD, 0x9F53, 0x9EAB, 0x9E05,
0x9D60, 0x9CBD, 0x9C1B, 0x9B7B, 0x9ADC, 0x9A40, 0x99A4, 0x990B,
0x9872, 0x97DC, 0x9747, 0x96B4, 0x9623, 0x9593, 0x9505, 0x9478,
0x93ED, 0x9364, 0x92DD, 0x9257, 0x91D4, 0x9151, 0x90D1, 0x9052,
0x8FD6, 0x8F5A, 0x8EE1, 0x8E6A, 0x8DF4, 0x8D80, 0x8D0E, 0x8C9E,
0x8C2F, 0x8BC2, 0x8B58, 0x8AEF, 0x8A87, 0x8A22, 0x89BF, 0x895D,
0x88FE, 0x88A0, 0x8844, 0x87EA, 0x8792, 0x873B, 0x86E7, 0x8695,
0x8644, 0x85F6, 0x85A9, 0x855E, 0x8516, 0x84CF, 0x848A, 0x8447,
0x8406, 0x83C7, 0x838A, 0x834F, 0x8316, 0x82DE, 0x82A9, 0x8276,
0x8245, 0x8216, 0x81E8, 0x81BD, 0x8194, 0x816D, 0x8147, 0x8124,
0x8103, 0x80E4, 0x80C6, 0x80AB, 0x8092, 0x807B, 0x8066, 0x8052,
0x8041, 0x8032, 0x8025, 0x801A, 0x8011, 0x800A, 0x8005, 0x8002,
0x8001, 0x8002, 0x8005, 0x800A, 0x8011, 0x801A, 0x8025, 0x8032,
0x8041, 0x8052, 0x8066, 0x807B, 0x8092, 0x80AB, 0x80C6, 0x80E4,
0x8103, 0x8124, 0x8147, 0x816D, 0x8194, 0x81BD, 0x81E8, 0x8216,
0x8245, 0x8276, 0x82A9, 0x82DE, 0x8316, 0x834F, 0x838A, 0x83C7,
0x8406, 0x8447, 0x848A, 0x84CF, 0x8516, 0x855E, 0x85A9, 0x85F6,
0x8644, 0x8695, 0x86E7, 0x873B, 0x8792, 0x87EA, 0x8844, 0x88A0,
0x88FE, 0x895D, 0x89BF, 0x8A22, 0x8A87, 0x8AEF, 0x8B58, 0x8BC2,
0x8C2F, 0x8C9E, 0x8D0E, 0x8D80, 0x8DF4, 0x8E6A, 0x8EE1, 0x8F5A,
0x8FD6, 0x9052, 0x90D1, 0x9151, 0x91D4, 0x9257, 0x92DD, 0x9364,
0x93ED, 0x9478, 0x9505, 0x9593, 0x9623, 0x96B4, 0x9747, 0x97DC,
0x9872, 0x990B, 0x99A4, 0x9A40, 0x9ADC, 0x9B7B, 0x9C1B, 0x9CBD,
0x9D60, 0x9E05, 0x9EAB, 0x9F53, 0x9FFD, 0xA0A8, 0xA154, 0xA202,
0xA2B1, 0xA362, 0xA415, 0xA4C8, 0xA57E, 0xA634, 0xA6ED, 0xA7A6,
0xA861, 0xA91D, 0xA9DB, 0xAA9A, 0xAB5A, 0xAC1C, 0xACDF, 0xADA3,
0xAE69, 0xAF30, 0xAFF8, 0xB0C2, 0xB18C, 0xB258, 0xB326, 0xB3F4,
0xB4C4, 0xB594, 0xB666, 0xB739, 0xB80E, 0xB8E3, 0xB9BA, 0xBA91,
0xBB6A, 0xBC44, 0xBD1F, 0xBDFB, 0xBED8, 0xBFB6, 0xC095, 0xC175,
0xC256, 0xC338, 0xC41B, 0xC4FF, 0xC5E4, 0xC6C9, 0xC7B0, 0xC898,
0xC980, 0xCA69, 0xCB53, 0xCC3E, 0xCD2A, 0xCE17, 0xCF04, 0xCFF2,
0xD0E1, 0xD1D1, 0xD2C1, 0xD3B2, 0xD4A4, 0xD597, 0xD68A, 0xD77D,
0xD872, 0xD967, 0xDA5D, 0xDB53, 0xDC4A, 0xDD41, 0xDE39, 0xDF32,
0xE02B, 0xE124, 0xE21E, 0xE319, 0xE414, 0xE50F, 0xE60B, 0xE707,
0xE804, 0xE901, 0xE9FE, 0xEAFC, 0xEBFA, 0xECF8, 0xEDF7, 0xEEF6,
0xEFF5, 0xF0F4, 0xF1F4, 0xF2F4, 0xF3F4, 0xF4F4, 0xF5F5, 0xF6F5,
0xF7F6, 0xF8F7, 0xF9F8, 0xFAF9, 0xFBFA, 0xFCFC, 0xFDFD, 0xFEFE
};

#endif