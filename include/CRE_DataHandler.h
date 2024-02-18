// Written by Ethan Anderson -- 2/17/2024
// DataHandler.h

#ifndef CRE_DATAHANDLER_H
#define CRE_DATAHANDLER_H

#include <cstdint>
#include <cstdlib>
#include <vector>

#define MAX_DATA_POINTS_READ_ARRAY 128
#define MAX_DATA_POINTS_TEMPORAL_ARRAY 256

// Holds a single data point
// Contains a timestamp in milliseconds and a float data point
// The timestamp should be from some arbitrary start time
// The millis() function in Arduino is a good example
class DataPoint {
public:
    uint32_t timestamp_ms;  // Will roll over every 49.7 days
    float data; // Data size: 4 bytes

    // Default constructor
    DataPoint() : timestamp_ms(0), data(0) {}

    // Constructor
    DataPoint(uint32_t timestamp_ms, float data);
};

// Internal class for storing data in a circular array
// This class is used to store data from a sensor as it comes in
class ReadCircularArray {
public:
    std::vector<DataPoint> data;
    uint8_t head;
    uint8_t tail;
    uint8_t size;

    ReadCircularArray();
    DataPoint getLatestData();
};

// Internal class for storing data in a circular array
// Data is stored here from the ReadCircularArray at a fixed interval
class TemporalCircularArray {
public:
    std::vector<DataPoint> data;
    uint8_t head;
    uint8_t tail;
    uint8_t size; // Will equal size_ms / interval_ms
    uint16_t interval_ms;  // The interval between each data point

    TemporalCircularArray(uint16_t interval_ms, uint16_t size_ms);
    DataPoint getLatest();
    DataPoint getMedian();
};

// This class is used to store data from a sensor
// Stores data in a pair of circular arrays
// The temporal array stores data with evenly spaced timestamps
//
// @param temporalInterval_ms: The interval between each data point in the temporal array
// @param temporalSize_ms: The size of the temporal array in milliseconds
class SensorData {
public:
    // Constructor
    SensorData(uint16_t temporalInterval_ms, uint16_t temporalSize_ms);

    // Returns true when the temporal array is updated
    // Returns false if the data was only put into the read array
    bool addData(DataPoint data);

    // Returns the median of the temporal array
    // This the value with the middle most value, not time
    DataPoint getTemporalArrayMedian();

    // Returns the latest data point from the read array
    DataPoint getLatestData();
private:
    ReadCircularArray readArray;
    TemporalCircularArray temporalArray;
};

#endif // DATAHANDLER_H