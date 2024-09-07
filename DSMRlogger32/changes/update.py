timeStruct buildTimeStruct(const char *timeStamp, uint16_t hourSlots, uint16_t daySlots, uint16_t monthSlots) 
{
    timeStruct thisTime;
    
    // Ensure valid slots
    if (hourSlots < _NO_HOUR_SLOTS_)  hourSlots = _NO_HOUR_SLOTS_;
    if (daySlots < _NO_DAY_SLOTS_)    daySlots = _NO_DAY_SLOTS_;
    if (monthSlots < _NO_MONTH_SLOTS_) monthSlots = _NO_MONTH_SLOTS_;
    
    // Copy the timestamp and fill missing parts
    strncpy(thisTime.Timestamp, timeStamp, _TIMESTAMP_LEN - 1);
    thisTime.Timestamp[_TIMESTAMP_LEN - 1] = '\0';
    fillMissingTimestamp(thisTime.Timestamp);
    
    // Extract values from the filled timestamp
    thisTime.Year = atoi(thisTime.Timestamp) + 2000;
    thisTime.Month = atoi(thisTime.Timestamp + 2);
    thisTime.Day = atoi(thisTime.Timestamp + 4);
    thisTime.Hour = atoi(thisTime.Timestamp + 6);
    thisTime.Minute = atoi(thisTime.Timestamp + 8);
    thisTime.Second = atoi(thisTime.Timestamp + 10);
    
    // Convert to epoch time
    struct tm timeinfo = {0};
    timeinfo.tm_year = thisTime.Year - 1900;
    timeinfo.tm_mon = thisTime.Month - 1;
    timeinfo.tm_mday = thisTime.Day;
    timeinfo.tm_hour = thisTime.Hour;
    timeinfo.tm_min = thisTime.Minute;
    timeinfo.tm_sec = thisTime.Second;
    thisTime.epoch = mktime(&timeinfo);
    
    // Calculate additional fields
    thisTime.Weekday = (timeinfo.tm_wday + 1) % 7;  // 0 = Sunday, 6 = Saturday
    thisTime.Months = (thisTime.Year - 1970) * 12 + thisTime.Month - 1;
    thisTime.monthsHist = monthSlots;
    thisTime.monthSlot = thisTime.Months % monthSlots;
    thisTime.Days = thisTime.epoch / SECS_PER_DAY;
    thisTime.daysHist = daySlots;
    thisTime.daySlot = thisTime.Days % daySlots;
    thisTime.Hours = thisTime.epoch / SECS_PER_HOUR;
    thisTime.hoursHist = hourSlots;
    thisTime.hourSlot = thisTime.Hours % hourSlots;
    
    return thisTime;
}