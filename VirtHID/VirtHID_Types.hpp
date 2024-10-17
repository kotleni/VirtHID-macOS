//
//  VirtHID_Types.hpp
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#ifndef virthid_types_h
#define virthid_types_h

const uint8_t virthid_max_report = 64;

typedef struct virthid_report {
    uint64_t size;
    uint8_t data[virthid_max_report];
} virthid_report;

#endif
