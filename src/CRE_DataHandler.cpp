// Written by Ethan Anderson -- 2/17/2024

// Setting up the Serial port for debugging if the Arduino.h library is included
#if __has_include(<Arduino.h>)
#include <Arduino.h>
#define DEBUG Serial
#endif

#include "CRE_DataHandler.h"

DataPoint::DataPoint(uint32_t timestamp_ms, float data){
    this->timestamp_ms = timestamp_ms;
    this->data = data;
}

void addDatatoCircularArray(std::vector<DataPoint>& array,
                            uint8_t &head,
                            uint8_t &tail,
                            uint8_t &size,
                            DataPoint data){
    if(size == MAX_DATA_POINTS_READ_ARRAY){
        head = (head + 1) % MAX_DATA_POINTS_READ_ARRAY;
    }
    else{
        size++;
    }
    array[head] = data;
}

ReadCircularArray::ReadCircularArray(){
    head = 0;
    tail = 0;
    size = 0;
    data.reserve(MAX_DATA_POINTS_READ_ARRAY);
}

DataPoint ReadCircularArray::getLatestData(){
    if(size == 0){
        return DataPoint();
    }
    else{
        return data[head];
    }
}

TemporalCircularArray::TemporalCircularArray(uint16_t interval_ms, uint16_t size_ms){
    head = 0;
    tail = 0;

    // Checking if size_ms / interval_ms is larger than the maximum size
    if (size_ms / interval_ms > MAX_DATA_POINTS_TEMPORAL_ARRAY){
        size = MAX_DATA_POINTS_TEMPORAL_ARRAY;
        #ifdef DEBUG
        DEBUG.println("TemporalCircularArray size is too large. Setting to MAX_DATA_POINTS_TEMPORAL_ARRAY");
        #endif
    }
    else{
        size = size_ms / interval_ms;
    }

    interval_ms = interval_ms;
    data.reserve(size);
}


DataPoint TemporalCircularArray::getLatest(){
    if(size == 0){
        return DataPoint();
    }
    else{
        return data[head];
    }
}

DataPoint TemporalCircularArray::getMedian(){
    if(size == 0){
        return DataPoint();
    }
    else{
        // Sorting the data array
        std::sort(data.begin(), data.end(), [](DataPoint a, DataPoint b) -> bool { return a.data < b.data; });
        return data[size / 2];
    }
}

SensorData::SensorData(uint16_t temporalInterval_ms, uint16_t temporalSize_ms) : temporalArray(temporalInterval_ms, temporalSize_ms){
    readArray = ReadCircularArray();
}

bool SensorData::addData(DataPoint data){
    addDatatoCircularArray(readArray.data, readArray.head, readArray.tail, readArray.size, data);
    if(data.timestamp_ms - temporalArray.getLatest().timestamp_ms >= temporalArray.interval_ms){
        addDatatoCircularArray(temporalArray.data, temporalArray.head, temporalArray.tail, temporalArray.size, data);
        return true;
    }
    else{
        return false;
    }
}

DataPoint SensorData::getTemporalArrayMedian(){
    return temporalArray.getMedian();
}

DataPoint SensorData::getLatestData(){
    return readArray.getLatestData();
}




