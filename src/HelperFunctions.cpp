#include <iostream>

#include "HelperFunctions.h"
#include "TypeCheck.h"


// Epoch / scaling constants
const static int32_t kdb_date_epoch_days = 10957;
const static int64_t kdb_timestamp_epoch_nano = 946684800000000000LL;
const static int64_t ns_us_scale = 1000LL;
const static int64_t ns_ms_scale = ns_us_scale * 1000LL;
const static int64_t ns_sec_scale = ns_ms_scale * 1000LL;
const static int64_t day_as_ns = 86400000000000LL;


// Date32 is int32_t days since the UNIX epoch 
int32_t Date32_KDate(int32_t date32)
{
  return date32 - kdb_date_epoch_days;
}

// Kdb date is days since 2000.01.01
int32_t KDate_Date32(int32_t k_date)
{
  return k_date + kdb_date_epoch_days;
}

// Date64 is int64_t milliseconds since the UNIX epoch 
int64_t Date64_KTimestamp(int64_t date64)
{
  return (date64 * ns_ms_scale) - kdb_timestamp_epoch_nano;
}

// Kdb timestamp is nano since 2000.01.01 00:00:00.0
int64_t KTimestamp_Date64(int64_t k_timestamp)
{
  return (k_timestamp + kdb_timestamp_epoch_nano) / ns_ms_scale;
}

// Exact timestamp encoded with int64 (as number of seconds, milliseconds,
// microseconds or nanoseconds since UNIX epoch)
//
// @@@ Doesn't support timezone strings
int64_t Timestamp_KTimestamp(std::shared_ptr<arrow::TimestampType> datatype, int64_t timestamp)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return (timestamp * ns_sec_scale) - kdb_timestamp_epoch_nano;
  case arrow::TimeUnit::MILLI:
    return (timestamp * ns_ms_scale) - kdb_timestamp_epoch_nano;
  case arrow::TimeUnit::MICRO:
    return (timestamp * ns_us_scale) - kdb_timestamp_epoch_nano;
  case arrow::TimeUnit::NANO:
    return timestamp - kdb_timestamp_epoch_nano;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Kdb timestamp is nano since 2000.01.01 00:00:00.0
int64_t KTimestamp_Timestamp(std::shared_ptr<arrow::TimestampType> datatype, int64_t k_timestamp)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return (k_timestamp + kdb_timestamp_epoch_nano) / ns_sec_scale;
  case arrow::TimeUnit::MILLI:
    return (k_timestamp + kdb_timestamp_epoch_nano) / ns_ms_scale;
  case arrow::TimeUnit::MICRO:
    return (k_timestamp + kdb_timestamp_epoch_nano) / ns_us_scale;
  case arrow::TimeUnit::NANO:
    return k_timestamp + kdb_timestamp_epoch_nano;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Time32 is signed 32-bit integer, representing either seconds or milliseconds since midnight. 
int32_t Time32_KTime(std::shared_ptr<arrow::Time32Type> datatype, int32_t time32)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return time32 * 1000;
  case arrow::TimeUnit::MILLI:
    return time32;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Kdb time is milliseconds from midnight but need to use the correct arrow TimeUnit for the datatype
int32_t KTime_Time32(std::shared_ptr<arrow::Time32Type> datatype, int32_t k_time)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return k_time / 1000;
  case arrow::TimeUnit::MILLI:
    return k_time;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Time64 is signed 64-bit integer, representing either microseconds or nanoseconds since midnight. 
int64_t Time64_KTimespan(std::shared_ptr<arrow::Time64Type> datatype, int64_t time64)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::MICRO:
    return time64 * 1000;
  case arrow::TimeUnit::NANO:
    return time64;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Kdb timespan is nanoseconds from midnight but need to use the correct arrow TimeUnit for the datatype
int64_t KTimespan_Time64(std::shared_ptr<arrow::Time64Type> datatype, int64_t k_timespan)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::MICRO:
    return k_timespan / 1000;
  case arrow::TimeUnit::NANO:
    return k_timespan;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Duration is an int64_t measure of elapsed time in either seconds, milliseconds, microseconds or nanoseconds.
int64_t Duration_KTimespan(std::shared_ptr<arrow::DurationType> datatype, int64_t timespan)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return (timespan * ns_sec_scale);
  case arrow::TimeUnit::MILLI:
    return (timespan * ns_ms_scale);
  case arrow::TimeUnit::MICRO:
    return (timespan * ns_us_scale);
  case arrow::TimeUnit::NANO:
    return timespan;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

// Kdb timestamp is nano since 2000.01.01 00:00:00.0
int64_t KTimespan_Duration(std::shared_ptr<arrow::DurationType> datatype, int64_t k_timespan)
{
  switch (datatype->unit()) {
  case arrow::TimeUnit::SECOND:
    return (k_timespan / ns_sec_scale);
  case arrow::TimeUnit::MILLI:
    return (k_timespan / ns_ms_scale);
  case arrow::TimeUnit::MICRO:
    return (k_timespan / ns_us_scale);
  case arrow::TimeUnit::NANO:
    return k_timespan;
  default:
    std::cerr << "Unknown TimeUnit" << std::endl;
    return 0;
  }
}

int64_t DayTimeInterval_KTimespan(arrow::DayTimeIntervalType::c_type dt_interval)
{
  return dt_interval.days * day_as_ns + dt_interval.milliseconds * ns_ms_scale;
}

arrow::DayTimeIntervalType::c_type KTimespan_DayTimeInterval(int64_t k_timespan)
{
  int32_t days = (int32_t)(k_timespan / day_as_ns);
  int32_t msec = (int32_t)((k_timespan % day_as_ns) / ns_ms_scale);
  return { days, msec };
}

bool IsKdbString(K str)
{
  return str != NULL && (str->t == -KS || str->t == KC);
}

const std::string GetKdbString(K str)
{
  return str->t == -KS ? str->s : std::string((S)kG(str), str->n);
}

KdbType GetKdbType(std::shared_ptr<arrow::DataType> datatype)
{
  switch (datatype->id()) {
  case arrow::Type::NA:
    return 0;
  case arrow::Type::BOOL:
    return KB;
  case arrow::Type::UINT8:
  case arrow::Type::INT8:
    return KG;
  case arrow::Type::UINT16:
  case arrow::Type::INT16:
    return KH;
  case arrow::Type::UINT32:
  case arrow::Type::INT32:
    return KI;
  case arrow::Type::INT64:
    return KJ;
  case arrow::Type::HALF_FLOAT: // uses uint16_t
    return KH;
  case arrow::Type::FLOAT:
    return KE;
  case arrow::Type::DOUBLE:
    return KF;
  case arrow::Type::STRING:
  case arrow::Type::LARGE_STRING:
    return KS;
  case arrow::Type::BINARY:
  case arrow::Type::LARGE_BINARY:
  case arrow::Type::FIXED_SIZE_BINARY:
    return 0;
  case arrow::Type::DATE32:
    return KD;
  case arrow::Type::DATE64:
    return KP;
  case arrow::Type::TIMESTAMP:
    return KP;
  case arrow::Type::TIME32:
    return KT;
  case arrow::Type::TIME64:
    return KN;
  case arrow::Type::DECIMAL: // 128 bit decimal
    return 0; // mixed list of KG lists of length 16
  case arrow::Type::DURATION:
    return KN;
  case arrow::Type::INTERVAL_MONTHS:
    return KM;
  case arrow::Type::INTERVAL_DAY_TIME:
    return KN;
  case arrow::Type::LIST:
  case arrow::Type::LARGE_LIST:
  case arrow::Type::FIXED_SIZE_LIST:
    return 0; 
  case arrow::Type::MAP:
    return 0;
  case arrow::Type::STRUCT:
    return 0;
  case arrow::Type::SPARSE_UNION:
  case arrow::Type::DENSE_UNION:
    return 0;
  default:
    TYPE_CHECK_UNSUPPORTED(datatype->ToString());
  }
  return 0;
}

std::shared_ptr<arrow::DataType> GetArrowType(K k_array)
{
  switch (k_array->t) {
  case KB:
    return arrow::boolean();
  case UU:
    return arrow::fixed_size_binary(sizeof(U));
  case KG:
    return arrow::int8();
  case KH:
    return arrow::int16();
  case KI:
    return arrow::int32();
  case KJ:
    return arrow::int64();
  case KE:
    return arrow::float32();
  case KF:
    return arrow::float64();
  case KC:
    return arrow::uint8();
  case KS:
    return arrow::utf8();
  case KP:
    return arrow::timestamp(arrow::TimeUnit::NANO);
  case KD:
    return arrow::date32();
  case KN:
    return arrow::time64(arrow::TimeUnit::NANO);
  case KT:
    return arrow::time32(arrow::TimeUnit::MILLI);
  case KM:
    return arrow::month_interval();
  case 99:
    return arrow::map(GetArrowType(kK(k_array)[0]), GetArrowType(kK(k_array)[1]));

  // There is fixed mapping from each arrow temporal datatype to the best fit
  // kdb type.  Supporting the below would clutter the type checking and
  // requiring double scaling (from kdb to a fixed time/date representation then
  // from that to the correct TimeUnit specified for that arrow datatype).
  // Casting these in q is simpler and more robust.
  case KU:
    throw TypeCheck("KU unsuppported, cast to KT");
  case KV:
    throw TypeCheck("KV unsupported, cast to KT");
  case KZ:
    throw TypeCheck("KZ unsupported, cast to KP");

  // We can't differentiate a mixed lixed - should it be a null, variable
  // binary, fixed size binary, list, union, struct, etc., array?
  case 0:
    throw TypeCheck("Cannot derive arrow datatype from mixed list");

  default:
    throw TypeCheck("Unsupported kdb datatype");
  }

  return std::shared_ptr<arrow::DataType>();
}