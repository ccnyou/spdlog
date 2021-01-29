//
// Created by chencongning on 2021/1/29.
//

#ifndef SPDLOG_HOURLY_FILE_SINKS_H
#define SPDLOG_HOURLY_FILE_SINKS_H

#include "../details/file_helper.h"
#include "../details/null_mutex.h"
#include "../fmt/fmt.h"
#include "base_sink.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {

/*
 * Default generator of hourly log file names.
 */
struct default_hourly_file_name_calculator
{
    // Create filename for the form filename.YYYY-MM-DD_hh.ext
    static spdlog::filename_t calc_filename(const spdlog::filename_t &filename)
    {
        std::tm tm = spdlog::details::os::localtime();
        return calc_filename(filename, tm);
    }

    static spdlog::filename_t calc_filename(const spdlog::filename_t &filename, const std::tm &tm)
    {
        spdlog::filename_t basename, ext;
        std::tie(basename, ext) = spdlog::details::file_helper::split_by_extension(filename);
        fmt::BasicMemoryWriter<char> w;
        w.write(
            SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}_{:02d}{}"), basename, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, ext);
        return w.str();
    }
};

/*
 * Rotating file sink based on date. rotates at midnight
 */
template<class Mutex, class FileNameCalc = default_hourly_file_name_calculator>
class hourly_file_sink SPDLOG_FINAL : public spdlog::sinks::base_sink<Mutex>
{
public:
    // create daily file sink which rotates on given time
    hourly_file_sink(const spdlog::filename_t &base_filename, int rotation_minute, int rotation_second)
        : _base_filename(base_filename)
        , _rotation_minute(rotation_minute)
        , _rotation_second(rotation_second)
    {
        if (rotation_minute < 0 || rotation_minute > 59 || rotation_second < 0 || rotation_second > 59)
            throw spdlog::spdlog_ex("hourly_file_sink: Invalid rotation time in ctor");
        _rotation_tp = _next_rotation_tp();
        const auto &fileName = FileNameCalc::calc_filename(_base_filename);
        _file_helper.open(fileName);
    }

    const spdlog::filename_t &log_file_name() const
    {
        return _file_helper.filename();
    }

    virtual const spdlog::filename_t calc_log_file_name(const std::tm &time) const override
    {
        const auto &fileName = FileNameCalc::calc_filename(_base_filename, time);
        return fileName;
    }

protected:
    void _sink_it(const spdlog::details::log_msg &msg) override
    {
        if (std::chrono::system_clock::now() >= _rotation_tp)
        {
            _file_helper.open(FileNameCalc::calc_filename(_base_filename));
            _rotation_tp = _next_rotation_tp();
        }
        _file_helper.write(msg);
    }

    void _flush() override
    {
        _file_helper.flush();
    }

private:
    chrono::time_point<system_clock> _next_rotation_tp()
    {
        chrono::time_point<system_clock> result;
        auto now = std::chrono::system_clock::now();
        __darwin_time_t tnow = std::chrono::system_clock::to_time_t(now);
        tm date = spdlog::details::os::localtime(tnow);
        date.tm_min = _rotation_minute;
        date.tm_sec = _rotation_second;
        auto rotation_time = std::chrono::system_clock::from_time_t(std::mktime(&date));
        if (rotation_time > now)
            result = rotation_time;
        else
            result = chrono::time_point<system_clock>(rotation_time + duration<long, ratio<60>>(60));
        return result;
    }

    spdlog::filename_t _base_filename;
    int _rotation_minute;
    int _rotation_second;
    chrono::time_point<system_clock> _rotation_tp;
    spdlog::details::file_helper _file_helper;
};

using hourly_file_sink_mt = hourly_file_sink<std::mutex>;
using hourly_file_sink_st = hourly_file_sink<details::null_mutex>;

} // namespace sinks
} // namespace spdlog

#endif // SPDLOG_HOURLY_FILE_SINKS_H
