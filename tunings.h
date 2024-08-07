#include "notes.h"

#ifndef TUNINGS
#define TUNINGS

// Enumeration for different tuning categories
typedef enum {
    Guitar, // Guitar tuning
    Bass,   // Bass tuning
    Misc,   // Miscellaneous tuning
} TuningCategory;

// Structure to represent a musical note
typedef struct {
    char label[20];  // Label for the note
    float frequency; // Frequency of the note
} NOTE;

// Structure to represent a tuning
typedef struct {
    char label[20];          // Label for the tuning
    TuningCategory category; // Category of the tuning
    int notes_length;        // Number of notes in the tuning
    NOTE notes[20];          // Array of notes in the tuning
} TUNING;

// Predefined tuning for tuning forks
const TUNING TuningForks = {
    "Tuning forks",
    Misc,
    6,
    {
        {"Common A4 (440)", 440.00f},
        {"Sarti's A4 (436)", 436.00f},
        {"1858 A4 (435)", 435.00f},
        {"Verdi's A4 (432)", 432.00f},
        {"1750-1820 A4 (423.5)", 423.50f},
        {"Verdi's C4 (256.00)", 256.00f},
    }
};

// Predefined tuning for scientific pitch
const TUNING ScientificPitch = {
    "Scientific pitch",
    Misc,
    12,
    {
        {"C0 (16Hz)", 16.0f},
        {"C1 (32Hz)", 32.0f},
        {"C2 (64Hz)", 64.0f},
        {"C3 (128Hz)", 128.0f},
        {"C4 (256Hz)", 256.0f},
        {"C5 (512Hz)", 512.0f},
        {"C6 (1024Hz)", 1024.0f},
        {"C7 (2048Hz)", 2048.0f},
        {"C8 (4096Hz)", 4096.0f},
        {"C9 (8192Hz)", 8192.0f},
        {"C10 (16384Hz)", 16384.0f},
        {"C11 (32768Hz)", 32768.0f}
    }
};

// Predefined tuning for standard 6-string guitar
const TUNING GuitarStandard6 = {
    "Guitar Standard 6",
    Guitar,
    6,
    {
        {"String 1", E4},
        {"String 2", B3},
        {"String 3", G3},
        {"String 4", D3},
        {"String 5", A2},
        {"String 6", E2}
    }
};

// Predefined tuning for drop D 6-string guitar
const TUNING GuitarDropD6 = {
    "Guitar Drop D 6",
    Guitar,
    6,
    {
        {"String 1", E4},
        {"String 2", B3},
        {"String 3", G3},
        {"String 4", D3},
        {"String 5", A2},
        {"String 6", D2}
    }
};

// Predefined tuning for D 6-string guitar
const TUNING GuitarD6 = {
    "Guitar D 6",
    Guitar,
    6,
    {
        {"String 1", D4},
        {"String 2", A3},
        {"String 3", F3},
        {"String 4", C3},
        {"String 5", G2},
        {"String 6", D2}
    }
};

// Predefined tuning for drop C 6-string guitar
const TUNING GuitarDropC6 = {
    "Guitar Drop C 6",
    Guitar,
    6,
    {
        {"String 1", D4},
        {"String 2", A3},
        {"String 3", F3},
        {"String 4", C3},
        {"String 5", G2},
        {"String 6", C2}
    }
};

// Predefined tuning for standard 7-string guitar
const TUNING GuitarStandard7 = {
    "Guitar Standard 7",
    Guitar,
    7,
    {
        {"String 1", E4},
        {"String 2", B3},
        {"String 3", G3},
        {"String 4", D3},
        {"String 5", A2},
        {"String 6", E2},
        {"String 7", B1}
    }
};

// Predefined tuning for standard 4-string bass
const TUNING BassStandard4 = {
    "Bass Standard 4",
    Bass,
    4,
    {
        {"String 1", G2}, 
        {"String 2", D2}, 
        {"String 3", A1}, 
        {"String 4", E1}
    }
};

// Predefined tuning for standard tenor 4-string bass
const TUNING BassStandardTenor4 = {
    "Bass Stand Tenor 4",
    Bass,
    4,
    {
        {"String 1", C3}, 
        {"String 2", G2}, 
        {"String 3", D2}, 
        {"String 4", A1}
    }
};

// Predefined tuning for standard 5-string bass
const TUNING BassStandard5 = {
    "Bass Standard 5",
    Bass,
    5,
    {
        {"String 1", G2}, 
        {"String 2", D2}, 
        {"String 3", A1}, 
        {"String 4", E1}, 
        {"String 5", B0}
    }
};

// Predefined tuning for standard tenor 5-string bass
const TUNING BassStandardTenor5 = {
    "Bass Stand Tenor 5",
    Bass,
    5,
    {
        {"String 1", C3}, 
        {"String 2", G2}, 
        {"String 3", D2}, 
        {"String 4", A1}, 
        {"String 5", E1}
    }
};

// Predefined tuning for drop D 4-string bass
const TUNING BassDropD4 = {
    "Bass Drop D 4",
    Bass,
    4,
    {
        {"String 1", G2}, 
        {"String 2", D2}, 
        {"String 3", A1}, 
        {"String 4", D1}
    }
};

// Predefined tuning for D 4-string bass
const TUNING BassD4 = {
    "Bass D 4",
    Bass,
    4,
    {
        {"String 1", F2}, 
        {"String 2", C2}, 
        {"String 3", G1}, 
        {"String 4", D1}
    }
};

// Predefined tuning for drop A 5-string bass
const TUNING BassDropA5 = {
    "Bass Drop A 5",
    Bass,
    5,
    {
        {"String 1", G2}, 
        {"String 2", D2}, 
        {"String 3", A1}, 
        {"String 4", E1}, 
        {"String 5", A0}
    }
};

// Predefined tuning for standard 4-string ukulele
const TUNING UkuleleStandard4 = {
    "Ukulele Standard 4",
    Misc,
    4,
    {
        {"String 1", A4},
        {"String 2", E4},
        {"String 3", C4},
        {"String 4", G4}
    }
};

// Number of predefined tunings
#define TUNINGS_COUNT 15

// Array of all predefined tunings
TUNING TuningList[TUNINGS_COUNT] = {
    ScientificPitch,
    TuningForks,

    GuitarStandard6,
    GuitarDropD6,
    GuitarD6,
    GuitarDropC6,
    GuitarStandard7,

    BassStandard4,
    BassStandardTenor4,
    BassStandard5,
    BassStandardTenor5,
    BassDropD4,
    BassD4,
    BassDropA5,
    UkuleleStandard4
};

#endif //TUNINGS
