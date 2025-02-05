#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <fstream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

void init()
{
    // Construct the sink
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    // Add a stream to write log to
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("ip_filter.log"));

    // Register the sink in the logging core
    logging::core::get()->add_sink(sink);
}

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

int main(int, char **)
{
    init();

    src::logger lg;
    BOOST_LOG_SEV(lg, logging::trivial::info) << "programm start";

    try
    {
        std::vector<std::vector<std::string>> ip_pool;

        for (std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }
        BOOST_LOG_SEV(lg, logging::trivial::info) << "sorting vector ip";

        std::sort(ip_pool.begin(), ip_pool.end(), [](const std::vector<std::string> &ipStr1, const std::vector<std::string> &ipStr2){
            for (int i = 0; i < 4; i++) {
                if (stoi(ipStr1[i]) < stoi(ipStr2[i])) {
                    return true;
                } else if (stoi(ipStr1[i]) > stoi(ipStr2[i])) {
                    return false;
                }
            }
            return false;
            });

        for (auto ip : ip_pool)
        {
            for (auto ip_part : ip)
            {
                std::cout << ip_part;
                if (ip_part != ip.back())
                {
                    std::cout << ".";
                }
            }
            std::cout << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        BOOST_LOG_SEV(lg, logging::trivial::error) << e.what();
    }

    return 0;
}
