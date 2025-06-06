#include <vector>
#include <string_view>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <charconv>

std::vector<std::string_view> split(const std::string_view &str, char delim)
{
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delim);

    while(end != std::string_view::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

struct IPAddress {
    uint8_t oct1, oct2, oct3, oct4;
    
    static IPAddress parse(std::string_view ip_str) {
        auto parts = split(ip_str, '.');
        if (parts.size() != 4) {
            throw std::invalid_argument("IP must have exactly 4 octets");
        }

        auto parse_octet = [](std::string_view octet) -> uint8_t {
            int value;
            auto [ptr, ec] = std::from_chars(octet.data(), octet.data() + octet.size(), value);
            
            if (ec != std::errc() || value < 0 || value > 255) {
                throw std::invalid_argument("Octet must be 0-255");
            }
            return static_cast<uint8_t>(value);
        };

        return IPAddress{
            parse_octet(parts[0]),
            parse_octet(parts[1]),
            parse_octet(parts[2]),
            parse_octet(parts[3])
        };
    }
    
    auto tied() const { return std::tie(oct1, oct2, oct3, oct4); }
    bool operator<(const IPAddress& other) const { return tied() > other.tied(); } // reverse order
};


std::ostream& operator<<(std::ostream& os, const IPAddress& ip) {
    os << static_cast<int>(ip.oct1) << '.'
       << static_cast<int>(ip.oct2) << '.'
       << static_cast<int>(ip.oct3) << '.'
       << static_cast<int>(ip.oct4);
    return os;
}

int main(int argc, char const *argv[])
{
    try
    {
        std::vector<IPAddress> ip_pool;

        for (std::string line; std::getline(std::cin, line);)
        {
            std::string_view line_view(line);
            if (auto tab_pos = line_view.find('\t'); tab_pos != line_view.npos) {
                line_view.remove_suffix(line_view.size() - tab_pos);
            }
            ip_pool.push_back(IPAddress::parse(line_view));
        }

        std::sort(ip_pool.begin(), ip_pool.end());

        auto print_ips = [](const auto& ips) {
            for (const auto& ip : ips) std::cout << ip << '\n';
        };

        print_ips(ip_pool);

        std::vector<IPAddress> filtered_1;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_1),
            [](const IPAddress& ip) { return ip.oct1 == 1; });
        print_ips(filtered_1);

        std::vector<IPAddress> filtered_46_70;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_46_70),
            [](const IPAddress& ip) { return ip.oct1 == 46 && ip.oct2 == 70; });
        print_ips(filtered_46_70);

        std::vector<IPAddress> filtered_any_46;
        std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(filtered_any_46),
            [](const IPAddress& ip) {
                return ip.oct1 == 46 || ip.oct2 == 46 || ip.oct3 == 46 || ip.oct4 == 46;
            });
        print_ips(filtered_any_46);

    }
    catch(const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
