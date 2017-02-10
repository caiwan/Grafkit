#include "stdafx.h"
#include "randomTable.h"

/*
Random data lookup table
4096 bytes of natural noise from random.org
*/

unsigned char _randmomTable[4096] = {
	0xD9, 0x84, 0xCA, 0x27, 0x56, 0x63, 0xE5, 0x8F, 0xF3, 0xC1, 0x2A, 0xAD,
	0x63, 0xB9, 0x10, 0xEA, 0x17, 0xF9, 0x06, 0x77, 0xF1, 0xE0, 0xE0, 0x78,
	0xBB, 0x27, 0x8F, 0xF0, 0xAF, 0x2B, 0x07, 0x98, 0xAF, 0x2C, 0x18, 0x2A,
	0x6A, 0x11, 0x6B, 0x7F, 0x0B, 0x5B, 0xAC, 0xCE, 0x81, 0x69, 0xE7, 0x1D,
	0x9A, 0x5E, 0xA7, 0x0B, 0x36, 0xE1, 0x43, 0xBD, 0x3D, 0xA0, 0x50, 0xF6,
	0xE5, 0xB1, 0xF8, 0x79, 0x74, 0x30, 0xCA, 0x5A, 0x5B, 0x65, 0xE4, 0x71,
	0xAD, 0x64, 0x20, 0xD5, 0xC8, 0x46, 0x8A, 0xF7, 0x83, 0xBE, 0x80, 0xD4,
	0x53, 0xA0, 0xB0, 0xE1, 0xC5, 0x22, 0x02, 0x79, 0xAA, 0xF1, 0x00, 0x44,
	0x51, 0x33, 0x9F, 0xA1, 0x9F, 0x0C, 0x46, 0x9C, 0x82, 0xB6, 0xC7, 0x58,
	0xDF, 0x92, 0xFF, 0x04, 0x71, 0xE4, 0xDA, 0xE5, 0xBA, 0xFA, 0xB9, 0x92,
	0xC9, 0x56, 0x9C, 0x55, 0x91, 0xB5, 0x8D, 0x5A, 0x62, 0x5E, 0x7F, 0xF0,
	0x36, 0xAA, 0x6B, 0x7C, 0x67, 0x02, 0x36, 0x41, 0xD0, 0x8C, 0x7F, 0xCE,
	0xB5, 0x1D, 0x5D, 0xF0, 0x8D, 0x6B, 0x03, 0x6D, 0x73, 0xE2, 0x4A, 0x8E,
	0xEA, 0x3C, 0x1F, 0x25, 0x2A, 0x61, 0x1D, 0x53, 0x41, 0xA6, 0x80, 0x0C,
	0xE2, 0xF2, 0x16, 0xDA, 0xEA, 0x5E, 0x6F, 0xCF, 0x30, 0x62, 0x7E, 0x31,
	0x76, 0x1C, 0xB1, 0x59, 0x56, 0x4C, 0x71, 0x84, 0xFA, 0xC1, 0x41, 0x60,
	0xC2, 0x41, 0xD7, 0x26, 0xEA, 0xF3, 0xF8, 0x7B, 0x12, 0xE0, 0xE4, 0x71,
	0x10, 0x42, 0x84, 0x16, 0xF6, 0x1A, 0x14, 0x37, 0x46, 0xB7, 0xA7, 0x20,
	0xAC, 0xCA, 0xAA, 0xC5, 0xE3, 0x15, 0x75, 0xAB, 0x96, 0xA8, 0xB9, 0x2D,
	0x84, 0x7E, 0x59, 0x07, 0xBE, 0x83, 0x9C, 0x14, 0xF7, 0x8F, 0x86, 0xEE,
	0x39, 0x7C, 0x96, 0x8B, 0xFF, 0xA1, 0x79, 0x6D, 0xC1, 0x78, 0xB0, 0xB3,
	0x93, 0x77, 0x53, 0xB4, 0x70, 0xC3, 0xC3, 0x01, 0x81, 0x28, 0xF7, 0x9A,
	0x68, 0xD0, 0x26, 0x52, 0x27, 0x01, 0x2E, 0x9F, 0x55, 0xC3, 0xAD, 0x5E,
	0xBA, 0x60, 0x76, 0x46, 0x05, 0x3E, 0x2B, 0x8E, 0xC9, 0xFB, 0xBC, 0x76,
	0x2E, 0x66, 0xCC, 0x4C, 0x7E, 0x72, 0x47, 0x15, 0xA1, 0xC6, 0x4C, 0x8F,
	0xA6, 0x44, 0xCC, 0x17, 0x2A, 0x2B, 0x16, 0x78, 0xFA, 0x2A, 0x6E, 0x4C,
	0xA7, 0xF7, 0x1F, 0x3A, 0xFE, 0x90, 0x9E, 0x69, 0xD2, 0xA0, 0xB7, 0x90,
	0xA9, 0x58, 0x7C, 0x3F, 0x3B, 0xB4, 0x32, 0x5F, 0x6D, 0x46, 0x6E, 0xB5,
	0x0B, 0x5E, 0xC2, 0x2F, 0x25, 0x45, 0x6F, 0x80, 0x0C, 0x72, 0xD1, 0x9A,
	0xFD, 0x6B, 0x9F, 0xA6, 0xD1, 0x88, 0xD1, 0x36, 0x70, 0x25, 0xBD, 0xAC,
	0xD6, 0xD7, 0xC9, 0x4A, 0x42, 0xA7, 0xD7, 0xD2, 0xC0, 0x9C, 0x04, 0xB2,
	0x31, 0x1D, 0x37, 0x11, 0xA5, 0x57, 0xD8, 0x17, 0x5D, 0x90, 0xE0, 0x5F,
	0x3E, 0x3E, 0x70, 0x6C, 0xDA, 0x2C, 0xED, 0x13, 0x44, 0x05, 0x70, 0xC5,
	0xEA, 0x26, 0x3B, 0x2C, 0x66, 0x1C, 0xF0, 0x21, 0xCA, 0x72, 0xF1, 0xB8,
	0x02, 0x90, 0x93, 0xE9, 0x74, 0x05, 0x1B, 0xF0, 0xB5, 0x38, 0x7A, 0xA9,
	0x10, 0xA5, 0xC7, 0x3C, 0x90, 0x3A, 0x7C, 0xD6, 0x70, 0x19, 0x00, 0x01,
	0x92, 0x20, 0x19, 0x63, 0x9D, 0x20, 0x7D, 0x7E, 0xAB, 0xAC, 0xF7, 0x43,
	0xFD, 0xC0, 0x4F, 0xED, 0x05, 0xBD, 0xAC, 0xF0, 0x4B, 0x61, 0x4F, 0x29,
	0x06, 0xD4, 0x71, 0xC0, 0x45, 0xC8, 0xA8, 0x0F, 0x33, 0x61, 0xA6, 0xA0,
	0xAB, 0x22, 0x00, 0xC2, 0x5B, 0x05, 0xA6, 0x99, 0x84, 0x2B, 0xD9, 0x89,
	0x43, 0xB1, 0xAA, 0xE3, 0xA7, 0xD5, 0x8D, 0x76, 0x13, 0xF8, 0xB1, 0x5E,
	0xBF, 0x8B, 0x2D, 0x9F, 0x15, 0x64, 0xB1, 0xEE, 0xC0, 0xE4, 0xBC, 0x22,
	0x43, 0x5A, 0xC0, 0x7B, 0x96, 0x54, 0xF3, 0x80, 0x47, 0x94, 0xF0, 0x63,
	0x68, 0x33, 0x11, 0xCA, 0x54, 0x65, 0x67, 0xEC, 0x96, 0xE7, 0x56, 0x94,
	0x34, 0xDA, 0x66, 0x40, 0x86, 0xB5, 0xB3, 0x20, 0x98, 0xF4, 0x29, 0xCB,
	0x9C, 0x01, 0xD7, 0x19, 0x29, 0x33, 0x3F, 0x6F, 0xC2, 0xD4, 0x18, 0x7F,
	0x3E, 0x81, 0x89, 0x09, 0x81, 0xA3, 0xA3, 0xD8, 0x2E, 0x8D, 0x6F, 0x09,
	0x73, 0x62, 0x5A, 0x9E, 0x60, 0xEB, 0x67, 0x98, 0xF3, 0xD5, 0x78, 0x8C,
	0x4D, 0xA7, 0xAB, 0xC2, 0x96, 0xC2, 0x73, 0x0A, 0xDC, 0x4C, 0x26, 0x3D,
	0x2C, 0xCB, 0x9B, 0xA6, 0x78, 0x75, 0x6E, 0xAF, 0xF0, 0x92, 0x2C, 0x40,
	0xBB, 0x6F, 0x3D, 0x8D, 0xF9, 0xD0, 0xEB, 0x53, 0xF4, 0xD6, 0x3E, 0xDC,
	0xEF, 0x4D, 0xA4, 0x2B, 0x93, 0xE6, 0xC6, 0xD2, 0xC9, 0xF8, 0xBB, 0x36,
	0x41, 0x78, 0xF3, 0x54, 0x04, 0x70, 0x10, 0x9F, 0x0B, 0x41, 0xC9, 0x6C,
	0x55, 0x49, 0x56, 0xC2, 0x77, 0x74, 0x5F, 0x80, 0x90, 0xBB, 0xC3, 0xA1,
	0xA1, 0x49, 0xCA, 0xE4, 0xA8, 0x5F, 0x42, 0xF9, 0x3C, 0x97, 0xEB, 0xB3,
	0xE7, 0xD0, 0x26, 0xA8, 0x9B, 0x16, 0x38, 0xF4, 0xE6, 0x9F, 0x28, 0x09,
	0x47, 0x7E, 0x08, 0x00, 0x5D, 0x09, 0xDF, 0xEA, 0xE8, 0xE1, 0x6F, 0x7D,
	0x22, 0xD0, 0x80, 0x3E, 0x6F, 0x7A, 0xE3, 0xA7, 0x39, 0x83, 0x35, 0x85,
	0x0B, 0x4F, 0x65, 0x61, 0x48, 0x62, 0xAE, 0x18, 0x4D, 0x33, 0x9E, 0xD3,
	0x2C, 0xCB, 0x16, 0x24, 0xD6, 0x14, 0x5B, 0x38, 0x2D, 0x05, 0xA1, 0xC0,
	0x41, 0x05, 0xE2, 0xCF, 0x42, 0x41, 0xBC, 0xE2, 0xBC, 0xC3, 0x13, 0x08,
	0xB5, 0xEB, 0x36, 0x78, 0xB0, 0xD9, 0x6E, 0xD8, 0xF2, 0xA1, 0x38, 0x2B,
	0xCA, 0x68, 0x6A, 0xFB, 0x4B, 0x4A, 0xEC, 0x40, 0xCD, 0x2C, 0xC4, 0xE8,
	0xB1, 0xD6, 0x71, 0x88, 0x89, 0x27, 0xCA, 0xBB, 0x7A, 0x57, 0x61, 0x67,
	0xE5, 0xE6, 0x42, 0x47, 0xE3, 0xB4, 0x90, 0xF7, 0xAA, 0x77, 0x72, 0x26,
	0xDA, 0x05, 0x19, 0x61, 0x83, 0x06, 0xA5, 0xB5, 0x6E, 0xA5, 0xAC, 0x3F,
	0x86, 0x0A, 0x18, 0x63, 0x57, 0xD0, 0xF2, 0x84, 0x8A, 0x8A, 0x44, 0xF9,
	0xFD, 0xF4, 0xF0, 0xAB, 0xE8, 0x23, 0xC8, 0x7E, 0x35, 0x6C, 0xCA, 0xE9,
	0x16, 0x20, 0xF6, 0x46, 0xC7, 0xB5, 0x29, 0x63, 0x61, 0x70, 0xA4, 0x88,
	0xED, 0x98, 0x1C, 0x05, 0xB0, 0x61, 0x44, 0x17, 0xE6, 0x5E, 0x6D, 0xDC,
	0x52, 0x1D, 0xA0, 0x8D, 0x09, 0x33, 0x34, 0xBD, 0x84, 0x0E, 0x1F, 0xE8,
	0xFF, 0xFE, 0x53, 0x49, 0x9D, 0x60, 0xE1, 0x53, 0xA7, 0x64, 0x57, 0xED,
	0x81, 0xE0, 0x38, 0x19, 0x31, 0xA6, 0xF4, 0x46, 0xBC, 0x3E, 0x7B, 0xBE,
	0x70, 0xAA, 0xC1, 0xAE, 0x19, 0xD1, 0xA4, 0x85, 0xF7, 0xE3, 0x40, 0xD8,
	0xC7, 0xEB, 0x21, 0xB0, 0xEE, 0xE2, 0x64, 0x96, 0xBD, 0xEB, 0xBD, 0xF5,
	0x69, 0x60, 0xC3, 0x62, 0x71, 0x25, 0x2E, 0x7B, 0xFB, 0x38, 0xA0, 0xBF,
	0x70, 0xBA, 0x81, 0xD5, 0xB0, 0x85, 0x97, 0x58, 0x34, 0x74, 0x08, 0xD9,
	0x9E, 0x0F, 0x9A, 0x30, 0xBF, 0xE2, 0x10, 0x5C, 0xBA, 0x15, 0x83, 0x8A,
	0xE2, 0x84, 0x7B, 0xB7, 0x00, 0xFA, 0xC1, 0xE9, 0x9A, 0x5C, 0x7E, 0x36,
	0xDF, 0x0E, 0x4B, 0x60, 0xB9, 0xD0, 0x40, 0x9F, 0x76, 0x84, 0x8F, 0xE2,
	0x55, 0xB6, 0xD7, 0x87, 0x82, 0xEF, 0x70, 0xB4, 0x7C, 0x0B, 0x45, 0x71,
	0x70, 0x57, 0xF5, 0x36, 0xC1, 0x2C, 0xE3, 0xDA, 0x2B, 0xC3, 0xB3, 0x84,
	0xFB, 0xC2, 0xCA, 0xAA, 0xE2, 0xAE, 0xC7, 0x5C, 0x90, 0xD3, 0xB1, 0x31,
	0x3C, 0xDA, 0x44, 0x0E, 0x42, 0x65, 0xB8, 0xD6, 0xD3, 0xDD, 0x4A, 0xDF,
	0x44, 0x1D, 0x1F, 0xDE, 0xE1, 0x2B, 0x0D, 0x41, 0x5B, 0xB5, 0xCF, 0x4A,
	0xED, 0x5F, 0xC8, 0x1C, 0xA7, 0x19, 0x42, 0x68, 0xB0, 0x4D, 0xD7, 0x8B,
	0x8E, 0xC5, 0x56, 0x70, 0xF9, 0xFF, 0xC3, 0x41, 0x5D, 0x80, 0x5C, 0x3E,
	0x6B, 0x48, 0xBD, 0x4B, 0xF3, 0x2D, 0x68, 0x59, 0xC6, 0xED, 0xD7, 0x2B,
	0x05, 0x26, 0xCB, 0x11, 0x30, 0xDE, 0xCC, 0xCA, 0x40, 0x74, 0x86, 0x4E,
	0x89, 0x30, 0xF5, 0x23, 0x46, 0x6A, 0x7C, 0x5B, 0xD8, 0xFF, 0xD5, 0x7B,
	0xBE, 0x95, 0x48, 0x73, 0xD8, 0x1D, 0x42, 0xC0, 0x45, 0x7D, 0x0E, 0x86,
	0x1D, 0xBF, 0xDD, 0xB2, 0x3F, 0x3E, 0xC3, 0x65, 0xAF, 0xBE, 0x64, 0x86,
	0x96, 0xE9, 0x55, 0xE6, 0x33, 0x57, 0x07, 0xF9, 0x58, 0xFC, 0x32, 0x51,
	0x79, 0xF1, 0x32, 0xBE, 0xD3, 0x56, 0xBD, 0xB6, 0x5F, 0xA3, 0x06, 0x20,
	0xEC, 0x30, 0xE0, 0x6C, 0xAE, 0x3A, 0x2C, 0xFE, 0xBE, 0xE1, 0x50, 0x22,
	0xCA, 0xAE, 0x2A, 0xC8, 0x03, 0x84, 0xC2, 0x7A, 0xE6, 0x76, 0xA7, 0xD7,
	0xC6, 0x2F, 0x55, 0x28, 0xAF, 0x71, 0xCA, 0xFC, 0x26, 0x58, 0x41, 0x9C,
	0xC4, 0xED, 0x63, 0xED, 0x60, 0xED, 0x5E, 0xE0, 0x1F, 0x2B, 0x69, 0x71,
	0xEF, 0x9D, 0x9E, 0x9D, 0xB5, 0x27, 0xE8, 0x1C, 0x72, 0xDB, 0xEE, 0x43,
	0xBC, 0xE7, 0x20, 0xEB, 0x87, 0x7B, 0x40, 0xEE, 0x47, 0x51, 0x77, 0xD1,
	0x5D, 0xC6, 0xBA, 0x35, 0x95, 0x62, 0xF1, 0x45, 0xB2, 0xE0, 0x67, 0xC7,
	0x7D, 0xF7, 0xF0, 0x5D, 0xFD, 0xBE, 0xC5, 0x82, 0x0A, 0xB6, 0x89, 0x83,
	0x83, 0x6C, 0x09, 0x4D, 0xCE, 0x93, 0xB9, 0xF9, 0x1B, 0x16, 0x3C, 0x0A,
	0x63, 0x2D, 0x43, 0x90, 0xCF, 0xCA, 0xA8, 0x32, 0x8E, 0x34, 0x9A, 0x61,
	0x76, 0xCD, 0x7A, 0x6A, 0x8F, 0xF0, 0xC2, 0x1F, 0x88, 0xE0, 0xC5, 0xC7,
	0x87, 0xC8, 0xEF, 0xA7, 0x4A, 0x51, 0xA6, 0xF3, 0x01, 0x5B, 0x7E, 0xDA,
	0x45, 0x97, 0x94, 0x2C, 0x71, 0x2C, 0x82, 0xA5, 0x9B, 0xE3, 0x27, 0xAC,
	0xA0, 0x93, 0x5E, 0x3C, 0xDB, 0x6B, 0x18, 0xDC, 0x17, 0x22, 0xB7, 0x3D,
	0xFA, 0xEB, 0x88, 0x6D, 0x49, 0x37, 0xC7, 0x96, 0x9D, 0x2F, 0x4D, 0x8B,
	0xF7, 0x82, 0xBE, 0x53, 0x18, 0x6E, 0x7E, 0xEA, 0x85, 0xB9, 0x37, 0x57,
	0xF1, 0x19, 0x3D, 0xC6, 0xFA, 0x4F, 0x17, 0xC7, 0xF7, 0xDA, 0x02, 0x62,
	0xD6, 0xE1, 0x77, 0x67, 0x26, 0xB4, 0x44, 0xDE, 0xEB, 0x1A, 0x63, 0x87,
	0x98, 0x97, 0x0E, 0xC0, 0xDD, 0x96, 0xFA, 0xAB, 0xA1, 0x50, 0x1E, 0x19,
	0xF6, 0x21, 0x4B, 0x68, 0xF8, 0x64, 0x4F, 0xBF, 0x0F, 0x77, 0x26, 0xB6,
	0x2B, 0x13, 0xBC, 0xD9, 0x5E, 0x1F, 0xC1, 0xD1, 0x59, 0x5A, 0xB5, 0xA6,
	0xD5, 0x0B, 0xAC, 0x38, 0xD9, 0x90, 0xB0, 0xE2, 0xE8, 0x2C, 0x35, 0xD1,
	0x2B, 0x6B, 0xAF, 0x54, 0x17, 0x0F, 0x88, 0x97, 0xCB, 0x7F, 0xB4, 0x0C,
	0xC3, 0x24, 0xD0, 0x8B, 0x4A, 0x80, 0xA0, 0x36, 0x06, 0x82, 0x74, 0x62,
	0x10, 0xC7, 0x83, 0x47, 0x74, 0x23, 0x48, 0x6D, 0xEA, 0x3B, 0xF3, 0xEA,
	0xC1, 0xA5, 0xF0, 0x8C, 0xFF, 0x28, 0x52, 0x66, 0xAA, 0x2B, 0xF9, 0x31,
	0xC5, 0xAD, 0x41, 0xDD, 0x09, 0xF4, 0xC4, 0x67, 0x06, 0x32, 0xA4, 0x8C,
	0xC2, 0xEC, 0x5B, 0xDC, 0x13, 0x04, 0xA0, 0x64, 0x43, 0xCC, 0x4C, 0x0F,
	0x03, 0xC8, 0x24, 0x1D, 0x1B, 0xBD, 0xB8, 0xF9, 0x21, 0xDF, 0x3F, 0xDA,
	0xC4, 0x1B, 0x94, 0x7C, 0x0A, 0x00, 0x05, 0x92, 0x2E, 0x9F, 0x01, 0x82,
	0x52, 0x1A, 0x72, 0xC1, 0x62, 0x0A, 0x81, 0x5C, 0xC8, 0x1A, 0x0C, 0x58,
	0x45, 0x63, 0x0B, 0x00, 0xD1, 0x44, 0x9E, 0x15, 0x3E, 0x89, 0xCE, 0x69,
	0x2C, 0x98, 0xF9, 0x0E, 0x50, 0x91, 0x5A, 0xE2, 0xE6, 0x3C, 0xE6, 0xC4,
	0xDE, 0xC5, 0xDA, 0x4E, 0x9A, 0x19, 0x9E, 0x5B, 0x04, 0xD3, 0x2B, 0xF3,
	0x73, 0x71, 0x98, 0x30, 0x8D, 0xC8, 0x09, 0x83, 0x8B, 0xCE, 0x50, 0x1C,
	0x24, 0x65, 0x8E, 0x69, 0x33, 0x82, 0xB2, 0x8C, 0xA4, 0x38, 0xB8, 0xE1,
	0x9C, 0x7D, 0x79, 0xF4, 0xEE, 0xE6, 0x99, 0x55, 0x21, 0xF9, 0xCD, 0xAF,
	0xE3, 0xB2, 0xAF, 0xD3, 0x17, 0x3B, 0xE6, 0xE3, 0x23, 0x80, 0xF4, 0xE8,
	0xCA, 0x50, 0x0F, 0x82, 0x99, 0xD5, 0xAA, 0x50, 0x7C, 0xA4, 0xE9, 0xF3,
	0x8C, 0xA9, 0xB2, 0x65, 0x92, 0xEA, 0x0C, 0xA7, 0x68, 0xFD, 0x61, 0xA8,
	0x62, 0x12, 0xA2, 0x7B, 0xAF, 0xA4, 0x2D, 0x7C, 0xAA, 0x3C, 0x35, 0xFC,
	0x16, 0x5F, 0x1A, 0xBF, 0xD0, 0x8A, 0x54, 0xBD, 0x84, 0xC2, 0xBE, 0x12,
	0x2B, 0xDF, 0xA4, 0x3C, 0x38, 0x23, 0xFB, 0x81, 0x08, 0x0C, 0xEA, 0xC8,
	0xA5, 0xDE, 0xCC, 0x33, 0xD0, 0x44, 0x52, 0x03, 0xB2, 0xA0, 0x73, 0x76,
	0xDC, 0x4F, 0xB0, 0x31, 0x32, 0xC2, 0xF0, 0x5E, 0x62, 0x18, 0x40, 0x4C,
	0x07, 0x68, 0xC9, 0x40, 0x5C, 0x2D, 0xA7, 0x35, 0xDD, 0x27, 0x8D, 0xAD,
	0x60, 0x21, 0x76, 0x48, 0xC6, 0x47, 0xDC, 0xD2, 0xBC, 0xB8, 0x99, 0x1E,
	0xF1, 0xC3, 0xB7, 0x0D, 0x2B, 0xDB, 0xA2, 0xB6, 0x19, 0x52, 0x15, 0x36,
	0x89, 0x2F, 0xD4, 0x3A, 0x1C, 0x0A, 0xFF, 0x91, 0xE4, 0x13, 0x24, 0xC7,
	0x63, 0xD3, 0x4D, 0x52, 0xE2, 0xAA, 0xEE, 0xE8, 0x18, 0x8D, 0x8F, 0x94,
	0x0D, 0xBD, 0x5A, 0x77, 0xB2, 0x11, 0x9B, 0x9D, 0xF5, 0x3F, 0xA6, 0xDA,
	0x0D, 0x11, 0x43, 0x79, 0x3A, 0x8F, 0xD8, 0x6C, 0xC6, 0x44, 0xF8, 0x8B,
	0x20, 0xDB, 0x84, 0x63, 0x9D, 0xDA, 0xC9, 0xD0, 0xA2, 0xBA, 0x4E, 0x39,
	0x3B, 0x0F, 0xD2, 0xF3, 0xE0, 0x3D, 0x15, 0x14, 0xDD, 0x5D, 0x3E, 0x73,
	0x35, 0x46, 0x92, 0x8D, 0xCF, 0xAD, 0x89, 0x74, 0x58, 0xEA, 0x81, 0xD8,
	0xBE, 0xD0, 0x90, 0x2E, 0xC5, 0xAA, 0x92, 0x71, 0x58, 0x2F, 0x81, 0x7B,
	0x2E, 0x45, 0x19, 0x31, 0x4F, 0x44, 0x6A, 0xB2, 0xB8, 0x2B, 0x90, 0x82,
	0xFD, 0x0F, 0xD2, 0x23, 0x80, 0xC7, 0xFE, 0x0B, 0xDC, 0xEC, 0x00, 0xC6,
	0x40, 0xCE, 0x2D, 0x8A, 0x4A, 0x88, 0xB0, 0xF5, 0x4D, 0x18, 0x1E, 0x93,
	0x7E, 0x74, 0x63, 0x1B, 0xB3, 0x36, 0x01, 0xD8, 0x2C, 0xEA, 0xDA, 0x0B,
	0x8F, 0x5C, 0x3C, 0x92, 0x77, 0x96, 0x87, 0xFE, 0xAB, 0x87, 0xBC, 0x83,
	0x35, 0xCA, 0x39, 0x7C, 0xB0, 0x80, 0x50, 0x58, 0xA2, 0x67, 0xC8, 0x5A,
	0x84, 0x03, 0x9E, 0xB9, 0x9C, 0xB3, 0x75, 0x04, 0xBB, 0x83, 0x32, 0x1B,
	0xD6, 0x11, 0xC5, 0xE0, 0x7A, 0x32, 0xAB, 0xDD, 0x56, 0x3E, 0x82, 0x80,
	0x5B, 0x2A, 0x39, 0xF8, 0xA9, 0x6F, 0xD6, 0x10, 0x86, 0x60, 0xD1, 0x72,
	0x9A, 0x95, 0xB5, 0x83, 0x6D, 0x3A, 0x05, 0x2C, 0x98, 0xAE, 0xFE, 0x65,
	0x59, 0x6C, 0x30, 0x8D, 0x0C, 0xFD, 0x61, 0xE7, 0x9D, 0x3F, 0xFD, 0x17,
	0xCA, 0x62, 0x0D, 0xF4, 0x22, 0xBF, 0xE6, 0x0D, 0x1A, 0xA0, 0xB3, 0x90,
	0x3A, 0x3F, 0xCA, 0xD5, 0xC2, 0x57, 0x16, 0x0C, 0x74, 0xC6, 0x60, 0x8A,
	0xC1, 0x04, 0x5D, 0xF5, 0xAD, 0x6B, 0xF7, 0x80, 0x85, 0xC5, 0xEA, 0xD2,
	0x9B, 0x20, 0xFC, 0xAA, 0xDC, 0x2B, 0xD7, 0x81, 0x3E, 0xF8, 0x2D, 0x5C,
	0x27, 0x73, 0x6C, 0xB2, 0x36, 0x53, 0x3D, 0x4D, 0x5D, 0xFD, 0xE4, 0x70,
	0xA2, 0x5D, 0x65, 0xDA, 0xA6, 0x19, 0x37, 0x39, 0x28, 0x31, 0x0F, 0xE4,
	0xD8, 0x26, 0xF2, 0x32, 0x9D, 0x71, 0x1C, 0x79, 0xA5, 0x99, 0x8A, 0x48,
	0xD2, 0x95, 0xB4, 0x5C, 0x2E, 0xD1, 0x9E, 0x1D, 0x11, 0x8F, 0xD7, 0x87,
	0x11, 0xC1, 0x5F, 0x07, 0x83, 0x04, 0x4A, 0x58, 0x5F, 0x2A, 0xE1, 0x8E,
	0x2F, 0x17, 0x5F, 0x19, 0xC4, 0xED, 0xFA, 0xC6, 0x8E, 0xDD, 0x36, 0x38,
	0x1A, 0x48, 0xD9, 0x9B, 0x80, 0x79, 0xAD, 0x1C, 0x40, 0xD6, 0x19, 0xDB,
	0x9E, 0x18, 0x9B, 0xA2, 0xA7, 0xAD, 0xA7, 0xD3, 0x49, 0xDB, 0x8A, 0xBC,
	0x10, 0xB0, 0xC9, 0xCA, 0xC9, 0xE2, 0xB9, 0x0A, 0x13, 0x36, 0xFC, 0xBF,
	0xBD, 0xB9, 0x87, 0xC4, 0x8A, 0xD0, 0xF7, 0x16, 0x0F, 0x3C, 0xC5, 0xAE,
	0x15, 0x66, 0x52, 0xB9, 0xC7, 0x7C, 0x6D, 0x7C, 0x14, 0xFB, 0x1B, 0x69,
	0x78, 0x90, 0x40, 0x8F, 0xF5, 0x92, 0x74, 0x6C, 0xDC, 0xBA, 0x1A, 0x52,
	0xAE, 0x0A, 0xC6, 0x9F, 0xC2, 0x0D, 0xE6, 0xF3, 0x85, 0x39, 0x8D, 0xC0,
	0x0B, 0x91, 0x3E, 0x41, 0xCB, 0xCB, 0x50, 0x61, 0x69, 0x95, 0x42, 0x18,
	0x5D, 0x66, 0xFD, 0xAD, 0xFC, 0xC3, 0xEF, 0x97, 0xDC, 0xBB, 0xA6, 0x51,
	0xA5, 0x24, 0x19, 0x3D, 0xD5, 0x22, 0xFB, 0xE4, 0x34, 0x37, 0x3F, 0x8E,
	0xDD, 0x23, 0x23, 0xA9, 0xE6, 0xA7, 0xC6, 0x0F, 0x55, 0xED, 0xBA, 0xF5,
	0x56, 0xA4, 0xC6, 0xAA, 0x64, 0xB9, 0x6C, 0xB6, 0xC2, 0xD4, 0x21, 0x57,
	0xEA, 0x48, 0x6B, 0xB2, 0x33, 0xFF, 0x25, 0x1A, 0xDA, 0x29, 0x66, 0x0A,
	0x94, 0xFF, 0xF7, 0x6E, 0x43, 0x39, 0x98, 0xAC, 0x22, 0x47, 0x7F, 0xDB,
	0x1E, 0xA2, 0x8B, 0x8C, 0x72, 0x2B, 0xE6, 0x32, 0x90, 0x29, 0xD3, 0x3E,
	0xEB, 0x20, 0xAF, 0x4F, 0x46, 0x28, 0xC0, 0x15, 0x0F, 0x5B, 0x16, 0x0B,
	0xAD, 0xAB, 0x2A, 0xF3, 0x32, 0xC4, 0x5A, 0x58, 0xC3, 0x34, 0x86, 0x19,
	0x43, 0x56, 0x9E, 0xD7, 0x1C, 0x10, 0xC5, 0xE9, 0x1B, 0x80, 0xB6, 0x15,
	0xD5, 0xD1, 0xE8, 0x7F, 0x97, 0x93, 0xA9, 0xC8, 0x9E, 0xF5, 0xB4, 0x23,
	0xB1, 0xF5, 0x0C, 0xBF, 0x64, 0x36, 0x2B, 0xBA, 0x72, 0xC5, 0x8D, 0xE1,
	0x7A, 0x66, 0xC6, 0xAA, 0x1D, 0x0E, 0x5D, 0x8B, 0x99, 0x34, 0x3D, 0x72,
	0xA9, 0x7F, 0x24, 0x89, 0xEF, 0xE0, 0x9F, 0xEC, 0x2A, 0xCE, 0xB0, 0x1B,
	0x87, 0x1D, 0x73, 0x58, 0x94, 0x0A, 0xA5, 0xB2, 0x07, 0x31, 0x9E, 0x28,
	0x45, 0xBC, 0x9B, 0xE9, 0x29, 0x6B, 0x06, 0x44, 0xA3, 0x0C, 0x9B, 0xA2,
	0xCE, 0xF1, 0x0B, 0x74, 0xB1, 0xF2, 0xA5, 0x7C, 0xF7, 0x69, 0x1C, 0xDB,
	0x58, 0x1B, 0x2F, 0x0F, 0x29, 0xA7, 0x61, 0x3C, 0xC1, 0x6B, 0xD3, 0xF3,
	0xC8, 0x57, 0x44, 0x89, 0xBD, 0x99, 0x7E, 0x50, 0xD0, 0xC4, 0x5D, 0xAF,
	0x4C, 0x7A, 0x60, 0x5F, 0x35, 0xAF, 0xA9, 0x26, 0xA6, 0x74, 0x12, 0x0E,
	0x96, 0x74, 0x36, 0xF3, 0x79, 0x01, 0x37, 0xDD, 0x7D, 0xAA, 0x1C, 0xFD,
	0x13, 0x24, 0x4A, 0x2F, 0x79, 0x4D, 0xBB, 0x1D, 0x22, 0x83, 0x80, 0xB7,
	0x19, 0x07, 0x56, 0xAC, 0xA4, 0x86, 0x99, 0xDB, 0xCF, 0x82, 0x70, 0x6B,
	0x0F, 0x66, 0x24, 0xE4, 0x2D, 0xA5, 0x3B, 0xA4, 0x74, 0xDB, 0x9C, 0x2B,
	0xDE, 0x96, 0x4A, 0x5B, 0x61, 0xCF, 0x3A, 0x85, 0xF8, 0x36, 0x7F, 0x4B,
	0x05, 0x3A, 0x63, 0x5F, 0x53, 0x40, 0x4B, 0xDE, 0xD5, 0x01, 0x77, 0x25,
	0xB2, 0x52, 0x47, 0xE6, 0x2D, 0xB2, 0x88, 0x1D, 0x4F, 0x5C, 0x90, 0x76,
	0xB1, 0x3E, 0xE7, 0x17, 0xA5, 0x3D, 0xC5, 0x1A, 0x42, 0xA1, 0x83, 0xDB,
	0xE1, 0x81, 0x66, 0x4A, 0x0A, 0xFA, 0x5D, 0xC6, 0x74, 0xFC, 0x77, 0x29,
	0x74, 0xF1, 0x18, 0x3F, 0xCE, 0x55, 0x43, 0x65, 0xC0, 0xA1, 0x5B, 0x78,
	0x92, 0x98, 0x43, 0x79, 0x45, 0x3C, 0xAB, 0xE1, 0x79, 0x57, 0xAD, 0xA8,
	0x64, 0x0A, 0x4A, 0x9E, 0xD7, 0xC3, 0xF3, 0xC2, 0xA6, 0xDB, 0xEB, 0xC4,
	0xD4, 0xCA, 0x70, 0x2B, 0xB0, 0x67, 0xF7, 0x4C, 0x74, 0x77, 0x61, 0x74,
	0x6B, 0x14, 0xC8, 0xDE, 0x2D, 0x3B, 0x81, 0x05, 0x7C, 0xAC, 0xC4, 0x5B,
	0x77, 0x63, 0xA5, 0x50, 0x4C, 0x88, 0x0F, 0x39, 0x62, 0x96, 0x53, 0x09,
	0x02, 0x0E, 0xA8, 0xC8, 0xF0, 0xAA, 0x32, 0xFC, 0x3B, 0xB1, 0x7D, 0x33,
	0x4E, 0xAA, 0xE7, 0x4E, 0xD8, 0x0B, 0x84, 0x67, 0xF9, 0x53, 0xF1, 0x11,
	0x60, 0x50, 0x7A, 0x74, 0x21, 0xA7, 0x90, 0xF6, 0x5C, 0xF0, 0x99, 0x73,
	0x3C, 0x4F, 0xF4, 0x2D, 0x12, 0x03, 0x52, 0xD3, 0x6D, 0x71, 0xDC, 0xE3,
	0x37, 0xCB, 0xE9, 0x65, 0x90, 0xEB, 0x8E, 0x2D, 0xBC, 0x3C, 0xE4, 0xA6,
	0xB2, 0x2D, 0x62, 0x4D, 0x7A, 0xA9, 0x18, 0xC3, 0x8E, 0xB7, 0xF0, 0xC7,
	0x4D, 0xA1, 0xE9, 0x4C, 0xCB, 0x9D, 0x9C, 0xC7, 0x91, 0x5D, 0x6A, 0xB6,
	0xF2, 0x05, 0x72, 0xD3, 0x99, 0x49, 0xF9, 0x91, 0xF8, 0xD1, 0x7E, 0xC7,
	0x8F, 0x7F, 0xCC, 0x18, 0x00, 0x51, 0xD2, 0x60, 0xB9, 0xD6, 0xA2, 0x00,
	0x6A, 0xFF, 0xDC, 0x9F, 0x1E, 0xFB, 0x8C, 0x69, 0xD6, 0x9F, 0x04, 0x36,
	0x26, 0xEB, 0x67, 0xD5, 0xE9, 0xE6, 0x78, 0x1F, 0x03, 0xFA, 0x06, 0x34,
	0xEF, 0x14, 0x81, 0x23, 0x16, 0x5E, 0xB9, 0xE9, 0x5A, 0x8A, 0xC3, 0x4E,
	0xFF, 0x79, 0x55, 0xA7, 0x40, 0x9E, 0x1D, 0x9A, 0xE8, 0xC7, 0x56, 0xD6,
	0xFC, 0xFD, 0xD8, 0xB7, 0x4E, 0x83, 0x4B, 0xF5, 0x36, 0xC9, 0x6F, 0x1B,
	0x55, 0x81, 0x67, 0xFA, 0xF9, 0x10, 0x64, 0xCD, 0xB4, 0x22, 0x0D, 0x81,
	0x90, 0x55, 0x02, 0x54, 0x74, 0xEB, 0x98, 0x74, 0x38, 0xC3, 0x00, 0x8B,
	0xD0, 0x16, 0xAC, 0xD6, 0x5B, 0x04, 0xEA, 0x49, 0xC3, 0x5A, 0x12, 0x1D,
	0xCB, 0xFC, 0xFB, 0x80, 0x83, 0xA5, 0x88, 0xB2, 0xFF, 0xCB, 0xF0, 0xCA,
	0x9D, 0xC1, 0xD0, 0x99, 0x8B, 0x57, 0xCA, 0x0C, 0x35, 0x29, 0xD9, 0xB8,
	0x2C, 0xC8, 0xC1, 0x55, 0xCE, 0xA3, 0x01, 0x86, 0x74, 0xED, 0x68, 0xDF,
	0x88, 0x81, 0x9D, 0x30, 0x43, 0xBA, 0xF1, 0x1A, 0x46, 0xCE, 0x7D, 0x85,
	0xAF, 0x23, 0x9F, 0xC2, 0x68, 0x70, 0x82, 0xE8, 0xD2, 0x53, 0x2C, 0xDE,
	0x85, 0xB7, 0x02, 0xAA, 0x08, 0x5B, 0x8B, 0x75, 0x55, 0xA9, 0x7F, 0x75,
	0x25, 0x51, 0x6A, 0x4A, 0x9A, 0x97, 0xA6, 0xE1, 0xC9, 0x13, 0x51, 0xA4,
	0x02, 0x15, 0x5C, 0x1F, 0xE2, 0xA1, 0x6E, 0xA2, 0x96, 0x36, 0x83, 0xC0,
	0x51, 0x6E, 0x6F, 0x99, 0x46, 0x7C, 0x22, 0x8B, 0x30, 0xF4, 0xC0, 0xD9,
	0xAB, 0x5F, 0x66, 0xF4, 0xB6, 0x37, 0x31, 0x33, 0xF4, 0xE6, 0xE1, 0xAE,
	0x08, 0xE1, 0xFD, 0x7A, 0x29, 0xE1, 0x6C, 0x86, 0x77, 0xB8, 0x27, 0xF5,
	0x4E, 0x18, 0xC1, 0xE9, 0x17, 0x43, 0xCB, 0xAC, 0xB9, 0xFF, 0xAC, 0x24,
	0x11, 0x09, 0x5C, 0x3C, 0x62, 0x22, 0xEF, 0xB1, 0x9F, 0xA3, 0x20, 0x0D,
	0xD4, 0xD1, 0x15, 0x72, 0x7C, 0x49, 0x5D, 0xE7, 0x2A, 0x45, 0xA8, 0xB3,
	0xB0, 0x5E, 0xAD, 0x45, 0x4E, 0xFD, 0xB9, 0x41, 0x48, 0x6B, 0xF9, 0x1A,
	0xFE, 0xD9, 0x9F, 0x25, 0x09, 0xAC, 0xEF, 0x9E, 0x40, 0xB4, 0xE9, 0x1D,
	0xCA, 0x22, 0xB0, 0xA2, 0xBF, 0xC5, 0xA6, 0x55, 0xBC, 0x80, 0x11, 0x58,
	0x65, 0x8C, 0x81, 0x66, 0x36, 0x03, 0x5F, 0x7D, 0x07, 0x96, 0x7A, 0xDF,
	0x22, 0xE6, 0xDB, 0x60, 0x97, 0xE0, 0x92, 0x12, 0xAB, 0x97, 0xE3, 0xCC,
	0xBE, 0xE5, 0xA9, 0x0C, 0x4D, 0x59, 0x92, 0x1B, 0xDD, 0xBB, 0x5E, 0x94,
	0x1C, 0x7F, 0xDA, 0x0F, 0xF0, 0xC3, 0xE1, 0xA9, 0xDD, 0xC0, 0xA2, 0xAB,
	0xD6, 0xE6, 0x6B, 0x9B, 0xE8, 0x89, 0xF9, 0xEA, 0xD0, 0x46, 0x5D, 0xCE,
	0x45, 0xC0, 0xB6, 0xF5, 0x00, 0xD3, 0x7E, 0xAB, 0xC1, 0x74, 0xE4, 0xD3,
	0x58, 0x9C, 0xD5, 0xE5, 0xE5, 0x59, 0xDC, 0x2C, 0x05, 0xD6, 0x6D, 0xDE,
	0x66, 0xAE, 0xEE, 0xB5, 0x02, 0x16, 0xE3, 0xD9, 0x76, 0x74, 0xA0, 0xA7,
	0xC6, 0xD9, 0xDF, 0x1B, 0xFF, 0x90, 0x1B, 0x34, 0x25, 0x30, 0x50, 0x1F,
	0x92, 0xE0, 0x2E, 0x78, 0x21, 0xD5, 0x68, 0x16, 0x7F, 0xC9, 0x13, 0x63,
	0x54, 0x83, 0x6B, 0x0E, 0x1E, 0xA6, 0x95, 0x79, 0x8D, 0xFD, 0xFB, 0xAB,
	0xD6, 0x6C, 0x43, 0x2C, 0xAB, 0xCF, 0x2E, 0x8F, 0x2C, 0xEA, 0x18, 0xFB,
	0x3F, 0x3E, 0x45, 0xF1, 0x51, 0x4E, 0x29, 0xF8, 0x51, 0x45, 0xB9, 0x78,
	0x6E, 0xED, 0x41, 0x40, 0x07, 0xFD, 0x39, 0x3D, 0x69, 0x43, 0x6D, 0xDE,
	0xF6, 0x67, 0xB5, 0xFF, 0x37, 0x29, 0xBD, 0x36, 0x82, 0xE0, 0x71, 0xEE,
	0xFF, 0x35, 0xCF, 0x3E, 0xBA, 0x0D, 0x02, 0xC1, 0x58, 0xAE, 0xB2, 0xDD,
	0x60, 0xBF, 0x79, 0xEB, 0xA8, 0xD0, 0xBB, 0xFF, 0x98, 0x79, 0x2E, 0x9C,
	0x44, 0xF8, 0xB4, 0x00, 0x65, 0xA4, 0x86, 0x9A, 0xFE, 0x43, 0x0F, 0x3A,
	0xC6, 0x43, 0x81, 0x43, 0x95, 0xE9, 0x4D, 0x96, 0xDF, 0x68, 0x60, 0x4C,
	0xC7, 0xA7, 0x09, 0x02, 0x2C, 0xCD, 0x09, 0x02, 0x79, 0xC4, 0x0B, 0xC9,
	0x3B, 0x67, 0x0D, 0xC4, 0xD6, 0xE4, 0x69, 0x1C, 0x52, 0xB5, 0x37, 0x41,
	0x48, 0xEB, 0x32, 0x6F, 0xFB, 0x76, 0x3A, 0x14, 0xCD, 0xD3, 0xAD, 0xF2,
	0x1C, 0x68, 0x26, 0xDA, 0x66, 0xE2, 0xDE, 0x68, 0x6C, 0x71, 0x77, 0x89,
	0x35, 0x0F, 0x6B, 0x09, 0x62, 0x1E, 0x25, 0xE2, 0x24, 0x49, 0xA9, 0xCA,
	0x21, 0xCE, 0x69, 0xBD, 0xAE, 0x5C, 0xC9, 0x43, 0xA9, 0x6E, 0x96, 0xDC,
	0x67, 0x67, 0x9C, 0xCE, 0x51, 0xC7, 0xA0, 0x69, 0xE5, 0x16, 0x0D, 0xC0,
	0x7F, 0x9B, 0x83, 0x7D, 0xB7, 0xEE, 0xBA, 0x0C, 0x84, 0x91, 0xF5, 0x10,
	0x1C, 0x6D, 0x84, 0x72, 0x61, 0x38, 0x30, 0x1A, 0x48, 0x42, 0xC7, 0x73,
	0x36, 0x65, 0x5B, 0x08, 0x74, 0x02, 0x37, 0x67, 0x0C, 0x7B, 0x9D, 0x41,
	0xC1, 0x77, 0xB0, 0x28, 0x24, 0xB9, 0xC8, 0xAD, 0x62, 0x4C, 0x9C, 0x8A,
	0x3E, 0x66, 0x2E, 0x9C, 0x2F, 0xE9, 0x8C, 0x66, 0x9E, 0xF2, 0x5D, 0x57,
	0xC6, 0x57, 0x8C, 0xD3, 0x56, 0x35, 0x9C, 0xFF, 0xED, 0x7B, 0x80, 0xCE,
	0x81, 0xFD, 0x06, 0x1E, 0x99, 0x9D, 0x7B, 0x38, 0x94, 0x3A, 0xB0, 0x03,
	0x66, 0x7F, 0xEB, 0xDB, 0x4D, 0x88, 0x2A, 0xA9, 0x1C, 0x27, 0x7C, 0xCC,
	0x0F, 0x54, 0x03, 0x7A, 0xD6, 0xA5, 0x1E, 0xCA, 0x7F, 0x0B, 0x8A, 0x32,
	0x75, 0x77, 0xBF, 0xF0, 0x01, 0x5D, 0xFF, 0xA2, 0x66, 0x6D, 0xF0, 0x20,
	0x00, 0xD7, 0x33, 0x66, 0x2C, 0xAC, 0xD1, 0x5B, 0x69, 0x83, 0x71, 0x84,
	0x0E, 0x41, 0x0A, 0x69, 0x15, 0x1B, 0x3D, 0x97, 0xA0, 0x1D, 0x67, 0xE1,
	0x05, 0x94, 0x9B, 0x80, 0xC0, 0x49, 0xC3, 0x8A, 0xF7, 0x1B, 0x3E, 0x70,
	0x43, 0x46, 0xF0, 0x77, 0xB1, 0x4A, 0xD2, 0x55, 0x90, 0x97, 0x82, 0xCC,
	0x50, 0xFC, 0xDE, 0xEB, 0x02, 0x33, 0x83, 0x0C, 0x4B, 0xB3, 0x2A, 0x46,
	0xC2, 0x44, 0x87, 0xF2, 0x6B, 0xE4, 0xB1, 0x3B, 0xD3, 0x32, 0x82, 0x94,
	0x6C, 0x69, 0xDE, 0x3F, 0x46, 0x74, 0xB2, 0x52, 0x49, 0x6B, 0xD2, 0xA9,
	0xE4, 0x61, 0x01, 0x24, 0xD9, 0x00, 0xD0, 0x30, 0xCD, 0xB2, 0xE0, 0x14,
	0xAD, 0x47, 0xD1, 0x99, 0x45, 0x53, 0xD6, 0xA2, 0x5F, 0x30, 0x46, 0x35,
	0x3B, 0x78, 0xA8, 0x9C, 0x8E, 0x6C, 0xF4, 0x22, 0x37, 0x18, 0xE7, 0x90,
	0x13, 0x3C, 0xA6, 0x90, 0xD8, 0xAB, 0x29, 0x95, 0x25, 0xAB, 0x4E, 0xE7,
	0xF7, 0xAE, 0x7D, 0x93, 0x26, 0xA4, 0x03, 0xA2, 0xC0, 0x23, 0x1E, 0x8D,
	0x5D, 0x0C, 0xA5, 0xF9, 0x50, 0x3E, 0x8E, 0xF9, 0x4D, 0x6A, 0x73, 0xB4,
	0x1B, 0x0D, 0xC8, 0xCB, 0x12, 0x20, 0x9F, 0x57, 0xE4, 0xC6, 0x0F, 0xF1,
	0xC2, 0x5E, 0x0A, 0xF2, 0x0A, 0xFA, 0x55, 0x8D, 0x69, 0x84, 0x93, 0x2E,
	0x50, 0x33, 0x36, 0xA1, 0x78, 0xA2, 0x6C, 0x5B, 0x13, 0xB0, 0x00, 0xA5,
	0x25, 0x95, 0x53, 0x7B, 0xDB, 0x32, 0x17, 0xE3, 0xB0, 0x02, 0xAF, 0x6A,
	0xFB, 0xC8, 0x13, 0xF6, 0x90, 0xD5, 0x8A, 0xEB, 0x6F, 0x37, 0xBD, 0x87,
	0xA1, 0x51, 0xDF, 0xC7, 0x00, 0x8F, 0x72, 0xAB, 0x26, 0x0A, 0x9B, 0x7A,
	0xE2, 0x12, 0x8E, 0xC3, 0xAA, 0xDD, 0xFD, 0x89, 0x24, 0xC5, 0x87, 0xE9,
	0x13, 0x25, 0xE9, 0x2F, 0x06, 0xD6, 0x79, 0x61, 0x07, 0x1B, 0xFF, 0xD9,
	0xD6, 0xF4, 0x8C, 0xD7, 0x6D, 0x3D, 0x6C, 0x11, 0xBC, 0xE4, 0xB9, 0x1D,
	0x60, 0xC4, 0xFD, 0xE4, 0x96, 0xCB, 0xE5, 0x65, 0x93, 0x8B, 0x0E, 0x50,
	0x40, 0xD9, 0x87, 0xC5, 0xDE, 0x5F, 0xF6, 0x56, 0x9E, 0xB8, 0x9B, 0xC0,
	0x83, 0x95, 0x75, 0x0B, 0xBC, 0x13, 0x56, 0x6B, 0x54, 0x37, 0x6F, 0x1B,
	0x23, 0xCC, 0xA7, 0x08, 0xAE, 0x07, 0x07, 0xF3, 0xC5, 0xD4, 0xCC, 0x69,
	0x24, 0x51, 0xBF, 0xA4, 0x1C, 0xDB, 0x72, 0x47, 0xDB, 0x69, 0xEC, 0x53,
	0xDD, 0x80, 0xF0, 0x4C, 0xC9, 0x8C, 0xD9, 0x9B, 0x62, 0x2F, 0x41, 0xB7,
	0x05, 0x90, 0xE5, 0xF9, 0x32, 0x27, 0x42, 0x30, 0x55, 0x40, 0x91, 0xBF,
	0xA0, 0x57, 0x58, 0xB8, 0xA7, 0xA5, 0xA8, 0xE3, 0xEA, 0x4D, 0xA4, 0x95,
	0x6F, 0x3C, 0x57, 0x68, 0x77, 0xFC, 0x07, 0x3C, 0x71, 0xC6, 0x0F, 0xE5,
	0x81, 0xE2, 0xC1, 0x0B, 0x78, 0x5A, 0x55, 0x7E, 0x94, 0xE5, 0x94, 0x17,
	0xEF, 0x1E, 0x4B, 0xCE, 0xA7, 0x90, 0x6E, 0xF7, 0xC1, 0x2C, 0xCE, 0x9B,
	0xB0, 0x56, 0x91, 0xD8, 0xDC, 0xD2, 0xA7, 0x87, 0x88, 0xBE, 0xA5, 0x3D,
	0xD5, 0x98, 0x1C, 0xCA, 0x49, 0xAA, 0x06, 0xE2, 0x7A, 0x7F, 0xD4, 0x05,
	0xFC, 0x8F, 0x1B, 0x08, 0xF9, 0xEA, 0x97, 0x51, 0x6F, 0x51, 0x70, 0xEE,
	0x8A, 0x06, 0x2E, 0x43, 0x0E, 0x02, 0x43, 0x9E, 0xA8, 0xED, 0xD1, 0x3F,
	0xE2, 0x54, 0xC5, 0x2E, 0x69, 0xDD, 0x4F, 0xD5, 0x11, 0x77, 0xDC, 0x79,
	0x48, 0x16, 0x3E, 0xC7, 0xC4, 0x03, 0x7A, 0x11, 0xF5, 0xF5, 0x1C, 0x53,
	0x18, 0x3E, 0x3A, 0xCE, 0xD0, 0x10, 0xD4, 0xE6, 0xD3, 0xCB, 0xDF, 0x49,
	0xB3, 0x5A, 0x19, 0x38, 0xC0, 0xE5, 0x13, 0xF7, 0x42, 0x4F, 0xDD, 0x9F,
	0x1D, 0x31, 0xDB, 0x01, 0x8A, 0xF8, 0x5D, 0x60, 0xC1, 0x4A, 0x5B, 0xE0,
	0x4F, 0x18, 0x30, 0xF6, 0xE1, 0x09, 0x0E, 0x0D, 0x7F, 0xFA, 0x56, 0x11,
	0xDD, 0x1F, 0xD9, 0x3B, 0x69, 0x57, 0x96, 0x87, 0x80, 0xAA, 0xF4, 0x26,
	0x0A, 0x50, 0xFD, 0x9C, 0xD0, 0xDC, 0x36, 0xEB, 0xB4, 0x4E, 0x8C, 0x7C,
	0xA6, 0x01, 0x1D, 0x79, 0x20, 0x78, 0x71, 0x98, 0x11, 0xAA, 0x08, 0x91,
	0x54, 0x0C, 0xBF, 0x03, 0xE2, 0x20, 0x04, 0x27, 0x33, 0xAA, 0xDA, 0xBD,
	0xA5, 0xF5, 0x20, 0x82, 0xAF, 0x2B, 0x58, 0xDA, 0x3F, 0x9F, 0x96, 0xAF,
	0x37, 0xAC, 0x45, 0xCB, 0xA1, 0xBE, 0xC4, 0xB1, 0xC0, 0x2B, 0x55, 0xA5,
	0x61, 0xCD, 0x66, 0x37, 0xEC, 0xF5, 0xD5, 0x5F, 0x20, 0xFA, 0x19, 0xC5,
	0xF2, 0x16, 0x18, 0x04, 0x05, 0xB4, 0xA9, 0xDC, 0x6C, 0xF9, 0xA1, 0xCB,
	0x3F, 0x96, 0x33, 0xD6, 0xB1, 0x2C, 0xDB, 0x56, 0x91, 0x5A, 0xE7, 0x0B,
	0xFD, 0xDA, 0x07, 0x15, 0x70, 0x4A, 0x65, 0x4F, 0x0F, 0x27, 0xEC, 0xE7,
	0x37, 0x81, 0xD0, 0xEA, 0xDC, 0x2C, 0x03, 0xBE, 0x18, 0xAD, 0xE0, 0x08,
	0xEA, 0xBA, 0x30, 0xCF, 0xFB, 0xF3, 0xED, 0xB0, 0x89, 0x29, 0x2F, 0x75,
	0x53, 0x29, 0x0F, 0x44, 0xDE, 0xD6, 0x45, 0x96, 0xF2, 0xA6, 0xB6, 0x9B,
	0x30, 0x54, 0xED, 0x36, 0x31, 0xA9, 0xC2, 0x93, 0x02, 0xEB, 0x79, 0x5C,
	0xAF, 0xE0, 0xC1, 0x43
};

const unsigned char * const GrafkitData::randmomTable = _randmomTable;
